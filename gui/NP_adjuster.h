#pragma once

//! Keeps track of and adjusts NP Photonics laser temperature

#include <QTime>
#include <string>
#include "RS232device.h"

class NP_adjuster : public RS232device
{
public:
	NP_adjuster(const std::string& port, unsigned baud, const std::string& cmd, double minT, double maxT, double deltaT);
        ~NP_adjuster();

	double getTemperature();
        void getTempCmd();
	double shiftTemperature(double K);
	double setTemperature(double newT);

	double shiftTemperatureUp();
	double shiftTemperatureDown();

protected:
	std::string cmd;
	double minT, maxT, T, deltaT;

public:
	QTime timeOfLastAdj;
        QTime timeOfLastT;

protected:
        int numValidT;
};

double restrict(double v, double bottom, double top);
