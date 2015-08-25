#include <stdlib.h>
#include <fstream>
#include <stdexcept>
#include <time.h>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#endif

#ifdef HAS_SAPI
#include <sapi.h>
#include <atlbase.h>

CComPtr<ISpRecoContext> cpRecoCtxt = 0;
CComPtr<ISpVoice> cpVoice = 0;

#endif

#include "ionizer_utils.h"

#include <cstring>
#include <iostream>

using namespace std;



struct spoken_message
{
   double t;
   std::string s;
};

void creepy_speak(const std::string& s)
{
	return;
#ifdef HAS_SAPI
   USES_CONVERSION;

   //keep track of when what was said to avoid auditory overload

   static deque<spoken_message> spoken_messages;

   //get the voice if it's not yet initialized
   if(!cpVoice)
   {
      ::CoInitialize(0);

      //initialize voice
      HRESULT hr = cpRecoCtxt.CoCreateInstance(CLSID_SpSharedRecoContext);

      if(SUCCEEDED(hr))
      {
         hr = cpRecoCtxt->GetVoice(&cpVoice);

         if(!SUCCEEDED(hr))
            cpVoice = 0;
      }
   }


   //use the voice if it's available
   if(cpVoice)
   {
      //keep a list of messages that were spone during the last 5 s
      double tRemove = CurrentTime_s() - 5;

      while(!spoken_messages.empty())
         if(spoken_messages.front().t < tRemove)
            spoken_messages.pop_front();
         else
            break;

      spoken_message sm;
      sm.t = CurrentTime_s();
      sm.s = s;

      bool bRecentlySpoken = false;

	  //figure out if the same thing was said recently
      for(size_t i=0; i<spoken_messages.size(); i++)
         if(sm.s == spoken_messages[i].s)
            bRecentlySpoken = true;


	  //only speak messages that weren't said within the last 5 s
      if(!bRecentlySpoken)
      {
         cpVoice->Speak(A2W(s.c_str()), SPF_ASYNC, NULL);

         spoken_messages.push_back(sm);
      }
   }
#else
   // cerr << s << endl;
#endif
}

#ifdef WIN32
working_dir::working_dir(const std::string& dir) : dir(dir)
{
   size_t i0 = 0;

   for(size_t i=0; i<dir.length(); i++)
   {
      if(dir[i] == '\\')
      {
         if(i > 0)
            _mkdir(dir.substr(i0, i-i0).c_str());

         i0 = i;
      }
   }

   if(i0 < dir.length())
      _mkdir(dir.substr(i0, dir.length()-i0).c_str());

   _chdir(dir.c_str());
}
#endif

//double df_timer = 0;
//LARGE_INTEGER lif_timer = {0, 0};

#ifdef WIN32

//return current time in 100 ns units since 1970
unsigned long long CurrentTime_100ns()
{
   ULARGE_INTEGER uliNow;
   FILETIME ftNow;
   SYSTEMTIME stNow;

   GetSystemTime(&stNow);
   SystemTimeToFileTime(&stNow, &ftNow);
   uliNow.LowPart = ftNow.dwLowDateTime;
   uliNow.HighPart = ftNow.dwHighDateTime;

/*	if(lif_timer.QuadPart != 0)
   {
      LARGE_INTEGER liNow;
      QueryPerformanceCounter(&liNow);
      liNow.QuadPart = liNow.QuadPart % lif_timer.QuadPart; // in ticks beyond one second
      double dt = static_cast<double>(liNow.QuadPart);
      uliNow.QuadPart = uliNow.QuadPart + static_cast<unsigned __int64>(dt * 1e7 / df_timer);
   }
   else
   {
      QueryPerformanceFrequency(&lif_timer);
      df_timer = static_cast<double>(lif_timer.QuadPart);
   }
*/
   return uliNow.QuadPart;
}
#else

//return current time in 100 ns units since 1970
unsigned long long CurrentTime_100ns()
{
   timespec t;
   clock_gettime(CLOCK_REALTIME, &t);

   unsigned long long r = t.tv_sec;
   r *= 10000000;
   r += t.tv_nsec / 100;

   return r;
}

#endif

//return current time in seconds since 1970
double CurrentTime_s()
{
   unsigned long long ns100 = CurrentTime_100ns() % 10000000;
	time_t now;
	time(&now);

   return now + ns100 * 1e-7;
}

int sign(int i)
{
   if(i == 0) return i;
   else return i > 0 ? 1 : -1;
}

double factorial(int n)
{
   double d = 1;

   while(n > 0)
      d *= n--;

   return d;
}

//append the file named by inputName to ofs
void AppendFile(std::ofstream& ofs, const std::string& inputName)
{
   ifstream inputFile(inputName.c_str());

   if(!inputFile.is_open())
      throw runtime_error("[ExpLockAl::AppendFile] unable to open file: " + inputName);

   while(!inputFile.eof() && inputFile.is_open())
   {
      char buff[1024];
      buff[0] = 0;

      inputFile.getline(buff, 1024);

      if(strlen(buff))
      {
         ofs.write(buff, static_cast<streamsize>(strlen(buff)));
         ofs << endl;
      }
   }
}

//return the current date and time as

//format = 0 : YYYYMMDD_hhmmss
//format = 1 : DD/MM/YYYY hh:mm:ss.sss

std::string GetDateTimeString(time_t t, int format /* = 0 */)
{
   if(t == 0)
      t = time(0);

   switch(format) {
      case 0 : return GetDateString(t) + "_" + GetTimeString(t);
      case 1 :
      {
         char szTimeString[256];
         strftime(szTimeString, sizeof(szTimeString), "%m/%d/%Y %H:%M:%S", localtime(&t));
         return szTimeString;
      }
   }

   return "";
}

//return the current time as hhmmss
std::string GetTimeString(time_t t)
{
   if(t == 0)
      t = time(0);

   char szTimeString[256];

   strftime(szTimeString, sizeof(szTimeString), "%H%M%S", localtime(&t));

   return szTimeString;
}

//return the current time as hh:mm:ss
std::string GetNiceTimeString(time_t t)
{
   if(t == 0)
      t = time(0);

   char szTimeString[256];

   strftime(szTimeString, sizeof(szTimeString), "%H:%M:%S", localtime(&t));

   return szTimeString;
}

//return the current date as YYYYMMDD
std::string GetDateString(time_t t, bool bShort)
{
   if(t == 0)
      t = time(0);

   char szTimeString[256];

   if(bShort)
      strftime(szTimeString, sizeof(szTimeString), "%y%m%d", localtime(&t));
   else
      strftime(szTimeString, sizeof(szTimeString), "%Y%m%d", localtime(&t));

   return szTimeString;
}

/*
template<class T> T bound(T val, T minimum, T maximum)
{
   return ::min<T>(::max<T>(val, minimum), maximum);
}

template int		bound<int>		(int, int, int);
template unsigned	bound<unsigned>	(unsigned, unsigned, unsigned);
template double		bound<double>	(double, double, double);
*/
template double const&	bound<double>	(double const&, double const&, double const&);


template<class T> inline int sign(const T& v)
{
    return v > 0;
}

double restrict(double v, double bottom, double top)
{
	if(v > top)
		v = top;

	if(v < bottom)
		v = bottom;

	return v;
}

