#ifndef IONIZER_UTILS_H
#define IONIZER_UTILS_H

#include <string>
#include <algorithm>

void creepy_speak(const std::string& s);

#ifdef WIN32
//create an object of this class to create a directory and switch to it
class working_dir
{
public:
	working_dir(const std::string& dir);

	const std::string dir;
};
#endif

double restrict(double v, double bottom, double top);

//return current time in 100 ns units since 1900
unsigned long long CurrentTime_100ns();

//Returns current time in seconds since 1970 as a double.
//This is standard UNIX time with additional digits.
//Currently (Windows XP) the resolution seems to be 16 ms.
double CurrentTime_s();

template<class T> inline int sign(const T& v);

double factorial(int);

//template<class T> inline  const T& bound(const T& val, const T& minimum, const T& maximum);

template<class T> const T& bound(const T& val, const T& minimum, const T& maximum)
{
    return std::min<T>(std::max<T>(val, minimum), maximum);
}

//append the file named by inputName to ofs
void AppendFile(std::ofstream& ofs, const std::string& inputName);

//return the date and time in "t" as YYYYMMDD_hhmmss
//use the current time by default
std::string GetDateTimeString(time_t t=0, int format=0);

//return the time in "t" as hhmmss
//use the current time by default
std::string GetTimeString(time_t t=0);

//return the time in "t" as hh:mm:ss
//use the current time by default
std::string GetNiceTimeString(time_t t=0);

//return the date in "t" as YYYYMMDD (bShort=false) or YYMMDD (bShort=true)
//use the current time by default
std::string GetDateString(time_t t=0, bool bShort=false);

#ifndef WIN32
struct RECT
{
	int top, left, bottom, right;
};

typedef struct RECT tagRECT;
typedef RECT* LPRECT;
#endif

/*
class Rect : public tagRECT
{
public:
	Rect() {};
	Rect(const Point& p, const Point& sz)
	{
		left = p.x;
		top = p.y;
		right = p.x + sz.x;
		bottom = p.y + sz.y;
	};

	Point Size() { return Point(right-left, bottom-top); }
	
	operator LPRECT () {return (LPRECT)this;}
	operator const RECT& () {return *this;}
};
*/

#endif //IONIZER_UTILS_H
