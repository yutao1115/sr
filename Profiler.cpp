//////////////////////////////////////////////////////////////////////////////
// Timer.cpp
// =========
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system 
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////

#include "Profiler.h"
  
#ifdef WIN32   // Windows system specific
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// constructor
///////////////////////////////////////////////////////////////////////////////
Profiler::Profiler()
    :startTimeInMicroSec(0),endTimeInMicroSec(0),
     stopped(0),startCount{0},endCount{0}
{
#ifdef WIN32
    QueryPerformanceFrequency((PLARGE_INTEGER)&frequency);
#endif
}
 

///////////////////////////////////////////////////////////////////////////////
// start timer.
// startCount will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Profiler::start()
{
    stopped = 0; // reset stop flag
#ifdef WIN32
    QueryPerformanceCounter(&startCount);
#else
    gettimeofday((timeval*)&startCount, NULL);
#endif
}



///////////////////////////////////////////////////////////////////////////////
// stop the timer.
// endCount will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Profiler::stop()
{
    stopped = 1; // set timer stopped flag
#ifdef WIN32
    QueryPerformanceCounter((PLARGE_INTEGER)&endCount);
#else
    gettimeofday((timeval*)&endCount, NULL);
#endif
}



///////////////////////////////////////////////////////////////////////////////
// compute elapsed time in micro-second resolution.
// other getElapsedTime will call this first, then convert to correspond resolution.
///////////////////////////////////////////////////////////////////////////////
double Profiler::getElapsedInMicroSec()
{
#ifdef WIN32
    if(!stopped)
        QueryPerformanceCounter((PLARGE_INTEGER)&endCount);

    startTimeInMicroSec = startCount * (1000000.0 / frequency);
    endTimeInMicroSec = endCount * (1000000.0 / frequency);
#else
    if(!stopped)
        gettimeofday((timeval*)&endCount, NULL);

    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

    return endTimeInMicroSec - startTimeInMicroSec;
}



