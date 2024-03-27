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
#define PIN_SIZE 3
#define PIN_OPTIONS 10

class SDI12Controller {
private:
  int pinOptions[PIN_SIZE] = {10, 11, 13};
  String checkAddress = "?!";
  String queryValue = "R0!";
  String statusCheck = "";
  uint8_t pins[PIN_OPTIONS];
  uint8_t lengths[PIN_OPTIONS];
  int pin = -1;
  SDI12 mySDI12;
  const uint8_t MAX_BUFFER_SIZE = 8;
  unsigned long returnlength = 0;
  int untilAvailable(unsigned long, uint8_t);
  unsigned long getSDIString(char*, uint8_t);
  void reset();
  void validatePins();
  void setPin(int address);
  void sendCommand(char*, uint8_t);
  uint8_t pinAddress(char*);
  void parsePinResponse(uint8_t);
  uint8_t getCheckSize(uint8_t);
  bool invalidPin(uint8_t);
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
