#include "sdi-12.h"

SDI12Controller::SDI12Controller() {
  this->pin = DATA_PIN;
}

SDI12Controller::SDI12Controller(int pin) {
  this->pin = pin;
}


void SDI12Controller::setup() {
  mySDI12.setDataPin(pin);
  mySDI12.begin();
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
  // return true;
  delay(150);
  unsigned long now = millis();
  int available = 0;
  while (available < checksize && millis() - now < timeout) {
    available = mySDI12.available();
    // Serial.print("WAITING ");
    // Serial.println(available);
    delay(5);
  }
  available = mySDI12.available();
  // Serial.print("AVAILABLE NOW ");
  // Serial.println(available);
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
  // Serial.print("I HAVE THIS AVAILABLE ");Serial.println(available);
  while (mySDI12.available()) {
    char c = mySDI12.read();
    Serial.print(c);
    buffer[returnlength] = c;
    returnlength++;
  }
  buffer[returnlength] = '\0';
  mySDI12.clearBuffer();
  // Serial.println("\nBUFFER RECEIVED ");Serial.println(returnlength);Serial.println(" ");Serial.println(buffer);
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

unsigned long SDI12Controller::cmd(char* buffer, unsigned long length) {
  if (!isConnected()) {
    Serial.println("DEVICE DISCONNECTED");
    return 0;
  }
  char cmd[MAX_BUFFER_SIZE];
  Utils::postCommandBuffer(buffer, cmd, length, MAX_BUFFER_SIZE);
  Serial.print("SENDING THIS CMD ");Serial.println(String(cmd));
  mySDI12.sendCommand(cmd);
  uint8_t checksize = String(cmd).startsWith("0") ? EXPECTED_MIN_LENGTH : EXPECTED_MIN_LENGTH_SOIL;
  return getSDIString(buffer, checksize);
}
