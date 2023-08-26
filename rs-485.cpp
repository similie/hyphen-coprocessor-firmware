#include "rs-485.h"

RS485Manager::RS485Manager() {}

void RS485Manager::begin(int port) {
  Serial1.begin(port);
  // node.begin(0, Serial1);
}

void RS485Manager::begin() {
  begin(DEFAULT_SERIAL_RS845_PORT);
}

void RS485Manager::end() {
  Serial1.end();
}
void RS485Manager::addStringToBuffer(uint16_t received, char* buffer) {
  String multiple = "+" + String(received);
  Serial.print("STORING VALUE");Serial.println(multiple);
  for (size_t i = 0; i < multiple.length(); i++) {
    buffer[bufferCount] = multiple.charAt(i);
    bufferCount++;
  }
  buffer[bufferCount] = '\0';
}

int RS485Manager::stripAddress(String cmd) {
  int index = cmd.indexOf(" ");
  String values = cmd.substring(index);
  return atoi(values.c_str());
}

unsigned long RS485Manager::process(int address, char* buffer) {

  return 0;

  // if (isnan(address)) {
  //    return INVALID_DEVICE;
  // }

  // Serial.print("PROCRESSING RS485 ");
  // Serial.println(address);
  // node.setSlave(address);
  // delay(50);
  // uint8_t result = node.readHoldingRegisters(this->START_REG, this->END_REG);
  // Serial.print("RESULT ");
  // Serial.println(result);
  // if (result != node.ku8MBSuccess) {
  //   return INVALID_DEVICE;
  // }
  // size_t index = 0;
  // while (node.available()) {
  //   uint16_t received = node.receive();
  //   index++;
  //   if (received == 0) {
  //     continue;
  //   }
  //   addStringToBuffer(received, buffer);
  // }
  // unsigned long sendValue = bufferCount;
  // bufferCount = 0;
  // return sendValue;
}