#include "ModbusMaster.h"
#ifndef rs_485_h
#define rs_485_h
#define DEFAULT_SERIAL_RS845_PORT 9600
#define INVALID_DEVICE 0

class RS485Manager {
  private: 
   ModbusMaster node;
   const uint16_t START_REG = 0x0000;
   const uint16_t END_REG = START_REG + 16; // 27;
   unsigned long bufferCount = 0;
   void addStringToBuffer(uint16_t, char*);
   static const size_t bufferSize = 8;
   char storeTemp[bufferSize];
  public: 
   RS485Manager();
   void begin();
   void begin(int);
   void end();
  //  int stripAddress(String);
   unsigned long process(int , char*);
};

#endif