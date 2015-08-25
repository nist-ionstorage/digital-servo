#include "RS232device.h"
#include <stdexcept>
#include <sstream>

#include <cstdio>
#include <iostream>

#include <qextserialport.h>

class SleepHelper: public QThread
{
public:
   static void msleep(int ms)
   {
      QThread::msleep(ms);
   }
};


#ifdef WIN32
#include <windows.h>
std::string GetWindowsErrorMsg(unsigned error_code);
int read(HANDLE hCOM, char* rcv_buff, unsigned nToRead)
{
   DWORD nRead = 0;

   ReadFile(hCOM, rcv_buff, nToRead, &nRead, 0);
   return nRead;
}

void usleep(unsigned us)
{
   Sleep(us/1000);
}

#else
typedef int HANDLE;
#define INVALID_HANDLE_VALUE (-1)
#include <stdio.h>   /* Standard input/output definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <unistd.h>
#endif


using namespace std;

RS232device::RS232device(const std::string& port_name, unsigned baud,
                   unsigned parity, unsigned data_bits, unsigned delay_ms, char resp_line_end,
                   const char* my_line_end) :
   port_name(port_name),
   baud(baud),
      bDebugRS232(false),
      parity(parity),
      data_bits(data_bits),
      delay_ms(delay_ms),
      resp_line_end(resp_line_end),
      my_line_end(my_line_end),
      pQPort(0)
{
    BaudRateType b;

    switch (baud)
    {
    case   9600: b = BAUD9600; break;
    case  19200: b = BAUD19200; break;
    case  57600: b = BAUD57600; break;
    case 921600: b = BAUD921600; break;
    default: throw runtime_error("unsupported baud_rate");
    }

    PortSettings ps;
    ps.BaudRate = b;
    ps.DataBits = DATA_8;
    ps.StopBits = STOP_1;
    ps.FlowControl = FLOW_OFF;
    ps.Timeout_Millisec = 10;

    pQPort = new QextSerialPort(QString(port_name.c_str()), ps);
    pQPort->open(QIODevice::ReadWrite);

    if(pQPort->isOpen())
        cout << "Port " << port_name << " is open." << endl;
    else
	{
        cout << "Port " << port_name << " failed to open." << endl;
		delete pQPort;
		pQPort = 0;
	}
}

RS232device::~RS232device()
{
	if(pQPort)
		delete pQPort;

    pQPort = 0;
}

void RS232device::msleep(unsigned ms)
{
	if(ms > 0)
		SleepHelper::msleep(ms);
}

void RS232device::readAllAvailableData()
{
	if(!pQPort)
		return;

	if(! pQPort->isOpen())
		return;

	int nn = 0;

	do
	{
		char c[2] = {0, 0};

		nn = pQPort->read(c, 1);
	//	cout << c;
	} while(nn > 0);
}

void RS232device::reopen()
{
	if(pQPort)
		pQPort->open(QIODevice::ReadWrite);
}


void RS232device::close()
{
	if(pQPort)
		pQPort->close();
}


bool RS232device::isPortOpen()
{
    if(pQPort)
        return pQPort->isOpen();
    else
        return false;
}

void RS232device::set_resp_line_end(char c)
{
	resp_line_end = c;
}

void RS232device::set_my_line_end(const char* str)
{
	my_line_end = string(str);
}


std::string RS232device::sendCmd_getResponse(const std::string& s)
{
   return sendCmd_getResponse(s, resp_line_end);
}

std::string RS232device::sendCmd_getResponse(const std::string& s, char line_end)
{
   sendCmd(s);

   msleep(delay_ms);

   return getResponse(line_end);
}

std::string RS232device::sendCmd_getResponseN(const std::string& s, unsigned nToRead)
{
   sendCmd(s);

   if(delay_ms > 0)
       SleepHelper::msleep(delay_ms);

   return getResponseN(nToRead);
}

void RS232device::sendCmd(const std::string& s)
{
	if(s.length() == 0)
		return;

	if(pQPort == 0)
	{
		cerr << "[RS232device::sendCmd] error: " << port_name << " is unavailable" << endl;
		return;
	}

   if ( !isPortOpen() )
   {
      cerr << "[RS232device::sendCmd] error: " << port_name << " is closed" << endl;
	  return;
   }

	string s_out = s + my_line_end;

    int nWrote = 0;

    while(nWrote < s_out.length())
    {
        int nn = pQPort->write(s_out.c_str() + nWrote, 1);
        SleepHelper::msleep(delay_ms);
        if (bDebugRS232) cout << "wrote: " << (int)(s_out[nWrote]) << endl;
        nWrote += nn;
    }

   if (delay_ms) SleepHelper::msleep(delay_ms);
   if (bDebugRS232) printf("[%s] Sent (%d bytes): %s", port_name.c_str(), nWrote, s_out.c_str());
}

std::string RS232device::getResponse()
{
    return getResponse(resp_line_end);
}

std::string RS232device::getResponse(char line_end)
{
   int iRcv = 0;

	unsigned maxTries = 100;
	unsigned nTries = 0;

	if(pQPort == 0)
	{
		cerr << "[RS232device::getResponse] error: " << port_name << " is unavailable" << endl;
		return string("");
	}

   for(unsigned i=0; i<1024; i++)
    rcv_buff[i] = 0;

    while (nTries < maxTries)
   {
        int nNew = 0;

   //     SleepHelper::msleep(1);
  //      if(pQPort->bytesAvailable() >= 1)
           nNew = pQPort->read(rcv_buff + iRcv, 1);
 
      if (nNew > 0)
      {
         nTries = 0;
         iRcv += nNew;
         rcv_buff[iRcv] = 0;
      }
        else
        {
            SleepHelper::msleep(delay_ms);
            nTries++;
        }

      if (iRcv)
         if (rcv_buff[iRcv - 1] == line_end)
         {
            if (bDebugRS232) printf("[%s] Recv (%d bytes): %s", port_name.c_str(), iRcv, rcv_buff);
            iRcv = 0;
            return string(rcv_buff);
         }
   }

      printf("WARNING: failed to receive line-end character: %d on device %s\r\nreceived: %s (%d bytes)\r\n", (int)(line_end), port_name.c_str(), rcv_buff, iRcv);
   return "";
}

std::string RS232device::getResponseN(unsigned nToRead)
{
	if(pQPort == 0)
	{
		cerr << "[RS232device::getResponseN] error: " << port_name << " is unavailable" << endl;
		return string("");
	}


   unsigned nNew = 0;
   int iRcv = 0;

   int maxTries = 100;
   int nTries = 0;

   while (nNew < nToRead && nTries < maxTries)
   {
      int nRead = 0;

      if(pQPort->bytesAvailable() >= nToRead)
		  nRead = pQPort->read(rcv_buff + iRcv, std::min<int>(1024 - iRcv, nToRead - iRcv));
     
      if(nRead > 0)
      {
          nNew += nRead;
          iRcv += nNew;

          rcv_buff[iRcv] = 0;

          if(bDebugRS232)
          {
                 printf("received %d chars: \n", nRead);

                 for(int j=nRead; j>0; j--)
                         printf("%02X ", (int)(rcv_buff[iRcv-j]));

                 printf("\n");
          }
      }

      nTries++;
   }

   if(bDebugRS232)
	   printf("received %d total chars\n", nNew);

   return string(rcv_buff);
}
