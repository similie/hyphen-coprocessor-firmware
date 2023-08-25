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

void SDI12Controller::reset() {
  Serial.println("RESETTING");
  mySDI12.end();
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

String SDI12Controller::stripNewLine(String cmd) {
  if (!cmd.endsWith("\n")) {
    return cmd;
  }
  return cmd.substring(0, cmd.length() - 1);
}

String SDI12Controller::parseSpaceCmd(String cmd) {
  String send = "";
  size_t length = cmd.length();
  bool commandPart = false;
  for (size_t i = 0; i < length; i++) {
    char c = cmd.charAt(i);
    if (c == ' ') {
      commandPart = true;
      continue;
    } else if (!commandPart) {
      continue;
    }
    send += String(c);
  }
  return send;
}

bool SDI12Controller::isConnected() {
  if (!PIN_LOW_REQURED) {
    return true;
  }
  return digitalRead(IS_CONNECTED_PIN) == LOW;
}

unsigned long SDI12Controller::cmd(String cmd, char* buffer) {
  if (!isConnected()) {
    Serial.println("DEVICE DISCONNECTED");
    return "";
  }

  String thisCmd = stripNewLine(cmd);
  Serial.print("SENDING COMMAND ");
  Serial.println(thisCmd);
  if (thisCmd.equals("")) {
    return;
  }

  if (thisCmd.startsWith("request_")) {
    thisCmd = parseSpaceCmd(thisCmd);
  }
  mySDI12.sendCommand(thisCmd);
  return getSDIString(buffer);
}
