#include "NP_adjuster.h"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <stdexcept>

using namespace std;

#ifdef WIN32
#define  snprintf  _snprintf
#endif

NP_adjuster::NP_adjuster(const std::string& port, unsigned baud, const std::string& cmd, double minT, double maxT, double deltaT) :
 RS232device(port, baud, 0, 8, 10, 0x0a, "\r\n"),
 cmd(cmd),
 minT(minT),
 maxT(maxT),
 T(0),
 deltaT(deltaT),
 numValidT(0)
{
	try
	{
		if(isPortOpen())
		{
                        sendCmd("*IDN?");
                        cout << getResponse() << endl;
                        sendCmd(":SYSTem:PASSword:CENable \"NP\""); //unlock protected commands
						getTempCmd();
						timeOfLastAdj.start();
                        timeOfLastT.start();
		}
		
	}
	catch(runtime_error e)
	{
		cerr << "RUNTIME ERROR: " << endl;
		cerr << "\t" << e.what() << endl;
	}
}

NP_adjuster::~NP_adjuster()
{
}

double NP_adjuster::getTemperature()
{
    double t = 0;

    if(numValidT < 3 && timeOfLastT.elapsed() > 1000)
    {
        string s = getResponse();

        istringstream iss(s.c_str());

        iss >> t;

        if(t != 0 && t > minT && t < maxT)
        {
            numValidT++;
            timeOfLastT.restart();
            T = t;
        }
        else
        {
            if(timeOfLastT.elapsed() > 500)
                getTempCmd();
        }
    }

    return T;
}

void NP_adjuster::getTempCmd()
{
    sendCmd(cmd + ":SPO?");
}

double NP_adjuster::shiftTemperature(double dT)
{
    T = getTemperature();

    if(T != 0)
    {
        T = setTemperature(T+dT);
    }

    return T;
}

double NP_adjuster::setTemperature(double newT)
{
    if(T == newT)
        return newT;

    timeOfLastAdj.restart();

    T = restrict(newT, minT, maxT);

    char buff[128];
    snprintf(buff, 127, "%s:SPO %f", cmd.c_str(), T);
    sendCmd(buff);

    return T;
}

double NP_adjuster::shiftTemperatureUp()
{
	
	return shiftTemperature(deltaT);
}

double NP_adjuster::shiftTemperatureDown()
{
	return shiftTemperature(-1*deltaT);
}

