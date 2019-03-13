#ifndef WIN_TIME_H_
#define WIN_TIME_H_
#include <windows.h>

static int gettimeofday(struct timeval* tv, struct timezone* tz)
{
	static LONGLONG birthunixhnsec = 116444736000000000;  /*in units of 100 ns */

	FILETIME systemtime;
	GetSystemTimeAsFileTime(&systemtime);

	ULARGE_INTEGER utime;
	utime.LowPart = systemtime.dwLowDateTime;
	utime.HighPart = systemtime.dwHighDateTime;

	ULARGE_INTEGER birthunix;
	birthunix.LowPart = (DWORD)birthunixhnsec;
	birthunix.HighPart = birthunixhnsec >> 32;

	LONGLONG usecs;
	usecs = (LONGLONG)((utime.QuadPart - birthunix.QuadPart) / 10);

	tv->tv_sec = (long)(usecs / 1000000);
	tv->tv_usec = (long)(usecs % 1000000);

	return 0;
}
#endif
