#ifndef SDI_12_h
#define SDI_12_h
#define SDI12_BUFFER_SIZE 116
#include <SDI12.h>
#include "utils.h"
#define DATA_PIN 10
#define EXPECTED_MIN_LENGTH 75
#define EXPECTED_MIN_LENGTH_SOIL 15
#define IS_CONNECTED_PIN 12
#define PIN_LOW_REQURED true


class SDI12Controller {
private:
  int pin = -1;
  SDI12 mySDI12;
  const uint8_t MAX_BUFFER_SIZE = 8;
  unsigned long returnlength = 0;
  int untilAvailable(unsigned long, uint8_t);
  unsigned long getSDIString(char*, uint8_t);
  void reset();
public:
  SDI12Controller();
  SDI12Controller(int);
  void init();
  void end();
  void setup();
  bool isConnected();
  unsigned long cmd(char*, unsigned long);
};

#endif
