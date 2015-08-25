#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <math.h>
#include <qwt_math.h>

#define FPGA_CLOCK_HZ 100.0e6
#define FPGA_CLOCK_T (1.0/100.0e6)

#define EXT_CLOCK_HZ 10.0e6

// #define SW_VER 0x0010 // 04nov14
// #define SW_VER 0x0011 // 16jan15
#define SW_VER 0x0012 // 28may15

long long int round_lli(double x);

#ifdef WIN32
#else
void Sleep(unsigned ms);
#endif

qint64 doubleToConfig(double d);
double configToDouble(qint64 i);

#endif // COMMON_H
