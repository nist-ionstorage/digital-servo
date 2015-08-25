#include "common.h"

long long int round_lli(double x)
{
	return static_cast<long long int>(x > 0.0 ? x + 0.5 : x - 0.5);
}

#ifdef WIN32

#else
#include <unistd.h>

void Sleep(unsigned ms)
{
  usleep(ms*1000);
}
#endif //WIN32

qint64 doubleToConfig(double d)
{
  union
  {
    float f;
    qint64 i;
  } u;

  u.f = d;

  return u.i;
}

double configToDouble(qint64 i)
{
  union
  {
    float f;
    qint64 i;
  } u;
    u.i = i;

  return u.f;
}




