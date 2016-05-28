//////////////////////////////////////////////////////////////////////////////
// =======
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

#pragma once

#ifndef _WIN32   // Windows system specific
#include <sys/time.h>
#endif

#include <stdint.h>
 
class Profiler
{
public:
    Profiler();                                    // default constructor

    void   start();                             // start timer
    void   stop();                              // stop the timer


    double getElapsedInMicroSec();         
    double getElapsedInMilliSec()  {return getElapsedInMicroSec() * 0.001;}
    double getElapsedInSec()       {return getElapsedInMicroSec() * 0.000001;}


private:
    double startTimeInMicroSec;                 // starting time in micro-second
    double endTimeInMicroSec;                   // ending time in micro-second
    int    stopped;                             // stop flag 
    
#ifdef _WIN32
    int64_t frequency;                    // ticks per second
    int64_t startCount;                         //
    int64_t endCount;                           //    
#else
    timeval startCount;
    timeval endCount;  
#endif
};
 
