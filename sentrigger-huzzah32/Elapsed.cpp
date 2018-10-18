/*
 Elapsed.cpp
 
 Written by Nick Gammon on 11 May 2011.
 
 
 PERMISSION TO DISTRIBUTE
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 and associated documentation files (the "Software"), to deal in the Software without restriction, 
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in 
 all copies or substantial portions of the Software.
 
 
 LIMITATION OF LIABILITY
 
 The software is provided "as is", without warranty of any kind, express or implied, 
 including but not limited to the warranties of merchantability, fitness for a particular 
 purpose and noninfringement. In no event shall the authors or copyright holders be liable 
 for any claim, damages or other liability, whether in an action of contract, 
 tort or otherwise, arising from, out of or in connection with the software 
 or the use or other dealings in the software. 
 
 */

#include "Elapsed.h"

#if not (defined(ARDUINO) && ARDUINO >= 100)
  // this blathering on is in case you want to make the class a static variable in a function
  extern "C" {
    __extension__ typedef int __guard __attribute__((mode (__DI__))); 
    int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); };
    void __cxa_guard_release (__guard *g) { *(char *)g = 1; };
    void __cxa_guard_abort (__guard *) { }; 
  } // end extern "C"
#endif

 // constructor resets time
Elapsed::Elapsed () 
{ 
  reset (); 
} // end of Elapsed::Elapsed

// reset time to now
void Elapsed::reset () 
  { 
  startus_ = micros (); 
  startms_ = millis (); 
  } // end of Elapsed::reset 

// return Elapsed time in milliseconds
unsigned long Elapsed::intervalMs () 
  { 
  return millis () - startms_; 
  } // end of Elapsed::intervalMs

// return Elapsed time in microseconds
unsigned long Elapsed::intervalUs () 
  { 
  return micros () - startus_; 
  }   // end of Elapsed::intervalUs

