#include "sdi-12.h"

SDI12Controller::SDI12Controller() {
  this->pin = DATA_PIN;
}

SDI12Controller::SDI12Controller(int pin) {
  this->pin = pin;
}


void SDI12Controller::setup() {
  mySDI12.setDataPin(this->pin);
  mySDI12.begin();
  // @todo
  // validatePins();
}
/**
 * @todo try to dynamically get the pin and the typical size
 */
void SDI12Controller::parsePinResponse( uint8_t pin ) {
  delay(100);
  Serial.print("CHECKING PIN ");Serial.println(pin);
  // we will try 5 times
  for (uint8_t i = 0; i < 5; i++) {
    uint8_t size = 0;
    while(mySDI12.available()) {
     char c = mySDI12.read();
     Serial.print(c);
     uint8_t value = String(c).toInt();
     if (isnan(value) || value > 9) {
      continue;
     }
     pins[value] = pin;
     size++;
    }
    if (size > 0) {
      break;
    }
    delay(100);
  }
}
/**
* @todo dynamically scan to validate the correct pins
*/
void SDI12Controller::validatePins() {
  delay(10000);
  Serial.println("Starting");
  for (uint8_t i = 0; i < PIN_SIZE; i++) {
    uint8_t pin = pinOptions[i];
    mySDI12.setDataPin(pin);
    mySDI12.sendCommand(checkAddress);
    parsePinResponse(pin);
  }
}

bool SDI12Controller::invalidPin(uint8_t value) {
  return value == 0 || isnan(value);
}

void SDI12Controller::setPin(int address) {
  uint8_t pin = pins[address];
  if (invalidPin(pin)) {
   return mySDI12.setDataPin(DATA_PIN);
  }
  mySDI12.setDataPin(pin);
}

void SDI12Controller::init() {
  setup();
  if (PIN_LOW_REQURED && IS_CONNECTED_PIN) {
    pinMode(IS_CONNECTED_PIN, INPUT);
  }
}

void SDI12Controller::end() {
  mySDI12.end();
}

void SDI12Controller::reset() {
  Serial.println("RESETTING");
  end();
  delay(200);
  init();
  delay(200);
  setup();
}

int SDI12Controller::untilAvailable(unsigned long timeout, uint8_t checksize) {
  delay(150);
  unsigned long now = millis();
  int available = 0;
  while (available < checksize && millis() - now < timeout) {
    int thisRead = mySDI12.available();
    if (thisRead != -1) {
      available = thisRead;
    }
    delay(5);
  }
  // available = mySDI12.available();
  if (available == -1) {
    reset();
  }
  delay(50);
  return available;
}

unsigned long SDI12Controller::getSDIString(char* buffer, uint8_t checksize) {
  returnlength = 0;
  int available = untilAvailable(1200, checksize);
  if (available == -1) {
    Serial.println("CLEARING BUFFER FAILED TO GET SDI VALUES");
    mySDI12.clearBuffer();
    return returnlength;
  }
  while (mySDI12.available()) {
    char c = mySDI12.read();
    Serial.print(c);
    buffer[returnlength] = c;
    returnlength++;
  }
  buffer[returnlength] = '\0';
  mySDI12.clearBuffer();
  if (returnlength < checksize) {
    return 0;
  }
  return returnlength;
}

bool SDI12Controller::isConnected() {
  if (!PIN_LOW_REQURED) {
    return true;
  }
  return digitalRead(IS_CONNECTED_PIN) == LOW;
}

uint8_t SDI12Controller::pinAddress(char*cmd) {
 Serial.print("CHECKOUT OUT THIS COND "); Serial.println(cmd);
 char pinBuffer[2];
 pinBuffer[0] = cmd[0];
 pinBuffer[1] = '\0';
 uint8_t selectedPinAddress = (uint8_t)Utils::bufferToInt(pinBuffer);
 if (isnan(selectedPinAddress) || selectedPinAddress > PIN_OPTIONS - 1) {
  return 0;
 }
 return selectedPinAddress;
}

void SDI12Controller::sendCommand(char*cmd, uint8_t selectedPinAddress) {
  setPin(selectedPinAddress);
  mySDI12.sendCommand(cmd);
}

uint8_t SDI12Controller::getCheckSize(uint8_t selectedPinAddress) {
  uint8_t fallback = selectedPinAddress == 0 ? EXPECTED_MIN_LENGTH : EXPECTED_MIN_LENGTH_SOIL;
  uint8_t length = lengths[selectedPinAddress];
  if (invalidPin(length)) {
    return fallback;
  }
  return length;
}

unsigned long SDI12Controller::cmd(char* buffer, unsigned long length) {
  if (!isConnected()) {
    Serial.println("DEVICE DISCONNECTED");
    return 0;
  }
  char cmd[MAX_BUFFER_SIZE];
  Utils::postCommandBuffer(buffer, cmd, length, MAX_BUFFER_SIZE);
  Serial.print("SENDING THIS CMD ");Serial.println(String(cmd));
  uint8_t selectedPinAddress = pinAddress(cmd);
  sendCommand(cmd, selectedPinAddress);
  return getSDIString(buffer, getCheckSize(selectedPinAddress));
}
