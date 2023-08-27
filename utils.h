#ifndef utils_h
#define utils_h
#include "Arduino.h"
class Utils {

  public:
    static unsigned int postCommandBuffer(char*, char*, unsigned int, unsigned int);
    static int bufferToInt(char*);
    static bool startsWith(const char*, char*, unsigned int);


};

#endif