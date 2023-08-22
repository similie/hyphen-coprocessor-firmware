#include "sdi-12.h"

SDI12Controller::SDI12Controller() {
  this->pin = DATA_PIN;
}

SDI12Controller::SDI12Controller(int pin) {
  this->pin = pin;
}

void SDI12Controller::init() {
  mySDI12.begin();
  //  mySDI12.forceListen()
  mySDI12.setDataPin(pin);
  if (PIN_LOW_REQURED && IS_CONNECTED_PIN) {
    pinMode(IS_CONNECTED_PIN, INPUT_PULLUP);
  }
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
  available = mySDI12.available() > EXPECTED_MIN_LENGTH;
  return available;
}

unsigned long SDI12Controller::getSDIString(char * buffer) {
  returnlength = 0;
  if (!untilAvailable(1000)) {
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
  Serial.print("MY BUFFER SIZE ");
  Serial.println(returnlength);
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



unsigned long SDI12Controller::cmd(String cmd, char * buffer) {
  if (!isConnected()) {
    return "";
  }

  String thisCmd = stripNewLine(cmd);
  // Serial.print("SENDING CMD ");
  // Serial.print(thisCmd);
  // Serial.print(" ");
  // Serial.print(cmd);
  // vlidate command
  if (thisCmd.equals("")) {
    return;
  }

  if (thisCmd.startsWith("request_")) {
    thisCmd = parseSpaceCmd(thisCmd);
  }
  mySDI12.sendCommand(thisCmd);
  return getSDIString(buffer);
}

//mySDI12.sendCommand(cmd);