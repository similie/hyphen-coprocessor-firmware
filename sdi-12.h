#ifndef SDI_12_h
#define SDI_12_h

#include "Arduino.h"
#define SDI12_BUFFER_SIZE 116
#include <SDI12.h>
#define DATA_PIN 10
#define EXPECTED_MIN_LENGTH 75
#define IS_CONNECTED_PIN 12
#define PIN_LOW_REQURED true


class SDI12Controller {
private:
  int pin = -1;
  SDI12 mySDI12;
  // const String[] availableCmds = {"xR0!"};
  String stripNewLine(String);
  unsigned long returnlength = 0;
  bool untilAvailable(unsigned long);
  unsigned long getSDIString(char*);
  String parseSpaceCmd(String);
public:
  SDI12Controller();
  SDI12Controller(int);
  void init();
  bool isConnected();
  unsigned long cmd(String cmd, char*);
  bool logExternal(String cmd);
};

#endif
