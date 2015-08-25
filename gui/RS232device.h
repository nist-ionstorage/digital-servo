#pragma once

#ifdef WIN32
typedef void* com_t;
#else
typedef int com_t;
#endif


#include <string>

class QextSerialPort;

class RS232device
{
public:
RS232device(const std::string& port_name, unsigned baud, unsigned parity=0, unsigned data_bits=8,
            unsigned delay_ms=0, char resp_line_end = '\n', const char* my_line_end = "\r\n");

virtual ~RS232device();

void readAllAvailableData();
bool isPortOpen();
void reopen();

void set_resp_line_end(char c);
void set_my_line_end(const char* str);


//! send command via RS232 port, return response
void sendCmd(const std::string& s);
std::string sendCmd_getResponse(const std::string& s);
std::string sendCmd_getResponse(const std::string& s, char line_end);
std::string sendCmd_getResponseN(const std::string& s, unsigned nToRead);

std::string getResponse();
std::string getResponse(char line_end);
std::string getResponseN(unsigned nToRead);

void close();
void msleep(unsigned ms);
protected:
com_t OpenSerialPort(const std::string& port_name, unsigned baud_rate);

std::string port_name;
unsigned baud;

char rcv_buff[1024];
bool bDebugRS232;
unsigned parity, data_bits,delay_ms;
char resp_line_end; // line-end character signifying end of device msg
std::string my_line_end;

QextSerialPort* pQPort;

};
