#ifndef _ARDUINO_COMPAT_H
#define _ARDUINO_COMPAT_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <ctype.h>

#include <string.h>

#include <iostream> 
using namespace std;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef bool boolean;

inline void interrupts(void)
{
  // cout << "Interrupts on" << endl;
  ;
}

inline void noInterrupts(void)
{
  // cout << "Interrupts off" << endl;
  ;
}

class Print
{
public:
  inline int write(const uint8_t* buffer, int len) {
    return 0;
  };
};


#define SREG 0x80

#endif
