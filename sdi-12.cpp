#include "sdi-12.h"

SDI12Controller::SDI12Controller() {
  this->pin = DATA_PIN;
}

SDI12Controller::SDI12Controller(int pin) {
  this->pin = pin;
}


void SDI12Controller::setup() {
  mySDI12.begin();
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

bool SDI12Controller::untilAvailable(unsigned long timeout) {
  // return true;
  bool available = false;
  delay(100);
  unsigned long now = millis();
  while (mySDI12.available() < EXPECTED_MIN_LENGTH && millis() - now < timeout) {
    delay(5);
  }
  delay(50);
  Serial.print("AVAILABLE ");
  Serial.println(mySDI12.available());
  if (mySDI12.available() == -1) {
    reset();
  }

  available = mySDI12.available() > EXPECTED_MIN_LENGTH;
  return available;
}

unsigned long SDI12Controller::getSDIString(char* buffer) {
  returnlength = 0;
  if (!untilAvailable(1000)) {
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
  if (returnlength < EXPECTED_MIN_LENGTH) {
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
  return getSDIString(buffer);
}
