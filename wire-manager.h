#ifndef WIRE_MAMANGER_h
#define WIRE_MAMANGER_h

#define DEFAULT_SLAVE_ID 9

#include "Arduino.h"
#include <Wire.h>

const uint16_t CMD_BUFFER_LENGTH = 350;
static char cmdBuffer[CMD_BUFFER_LENGTH];
static uint16_t cmdBufferIndex = 0;
static bool bufferReady = false;
static unsigned long staggedReady = 0;
static int _index = 0;

class WireManager {
private:
  uint8_t address = DEFAULT_SLAVE_ID;
  static const uint8_t WIRE_BUFFER_SIZE = 31;
  static void requestEvent(void);
  static void receiveEvent(int);
  static void sendFailedWire();
  static int getIndex();
  static int getTotalStringLength(int);
  static bool breakIndexCycle(int);
  static int getEndIndex(int);
  
  // static inline unsigned long bufferReadySet = 0;
  // static inline size_t BUFFER_READY_FAILED = 10000;
  // static bool readyBuffer();
  // not static
  void init();
  void printBuffer();
  //  String buildPrinterValues();
  static size_t buildPrinterValues(char*);
public:
  WireManager();
  WireManager(uint8_t);
  void begin();
  void end();
  static void setStagged(unsigned long);
  static void clearStagged();
  uint8_t peakCommand(char* buffer,uint8_t);
  unsigned long bufferLength();
  char * getBuffer();
  void clearCMD();
  void setResponseMessage(String);
  void setResponseMessage(const char*);
  void reset();
  static void zeroOutBuffer();
};



#endif