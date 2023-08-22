#include "wire-manager.h"

WireManager::WireManager() {}

WireManager::WireManager(uint8_t address) {
  this->address = address;
}

int WireManager::getIndex() {
  if (index > staggedReady) {
    index = 0;
    return -1;
  }
  return index++;
}

void WireManager::reset() {
  end();
  delay(500);
  begin();
  delay(500);
}

void WireManager::sendFailedWire() {
  Wire.write(0xFF);
}

int WireManager::getTotalStringLength(int setIndex) {
  return setIndex == -1 ? staggedReady : setIndex + 1;
}

bool WireManager::breakIndexCycle(int setIndex) {
  return setIndex != 0 && setIndex != 1 && setIndex % WIRE_BUFFER_SIZE == 0;
}

int WireManager::getEndIndex(int startIndex) {
  int setIndex = startIndex;
  while (setIndex != -1) {
    if (breakIndexCycle(setIndex)) {
      break;
    }
    setIndex = getIndex();
  }
  return setIndex;
}

void WireManager::receiveEvent(int byteCount) {
  if (bufferReady) {
    return;
  }
  if (cmdBufferIndex + byteCount > CMD_BUFFER_LENGTH) {
    cmdBufferIndex = 0;
  }
  for (uint16_t i = 0; i < byteCount; i++) {
    char c = Wire.read();
    cmdBuffer[cmdBufferIndex] = c;
    cmdBufferIndex++;
    if (c == '\n') {
      bufferReady = true;
    }
  }
  cmdBuffer[cmdBufferIndex] = '\0';
}

String WireManager::buildPrinterValues() {
  String printer = "";
  int setIndex = getIndex();
  int endIndex = getEndIndex(setIndex);
  for (uint16_t i = setIndex; i < getTotalStringLength(endIndex); i++) {
    char c = cmdBuffer[i];
    printer += String(c);
    Serial.print(i);
    Serial.print(" ");
    Serial.print((int)c);
    Serial.print(" ");
  }
  if (endIndex == -1) {
    clearStagged();
  }
  return printer;
}

void WireManager::requestEvent() {
  if (staggedReady == 0) {
    return sendFailedWire();
  }
  String printer = buildPrinterValues();
  Wire.print(printer);
  Serial.println("\n----");
  Serial.print(printer);
  Serial.println("\n_____");
}

void WireManager::init() {
  Wire.onRequest(WireManager::requestEvent);
  Wire.onReceive(WireManager::receiveEvent);
}

void WireManager::begin() {
  Wire.begin(address);
  init();
}

void WireManager::end() {
  Wire.end();
}

void WireManager::setStagged(unsigned long staggedValue) {
  index = 0;
  staggedReady = staggedValue;
}

void WireManager::clearStagged() {
  setStagged(0);
}

String WireManager::peakCommand() {
  if (!bufferReady) {
    return "";
  }
  uint16_t index = 0;
  String cmd = "";
  while (index < CMD_BUFFER_LENGTH) {
    char c = cmdBuffer[index];
    if (c == ' ' || c == '\n' || c == '\0' || c == 13) {
      break;
    }
    cmd += String(c);
    index++;
  }
  return cmd;
}

char* WireManager::getBuffer() {
  return cmdBuffer;
}

unsigned long WireManager::bufferLength() {
  return cmdBufferIndex;
}

String WireManager::receiveCmd() {
  if (!bufferReady) {
    return "";
  }
  clearCMD();
  return String(cmdBuffer);
}

void WireManager::printBuffer() {
  uint16_t i = 0;
  char c = cmdBuffer[i];
  Serial.println("PRINTING BUFFER");
  while (c != '\0') {
    Serial.print(c);
    i++;
    if (i > CMD_BUFFER_LENGTH) {
      break;
    }
    c = cmdBuffer[i];
  }
  Serial.println("\nEND_PRINT");
}

void WireManager::setResponseMessage(String message) {
  clearCMD();
  if (!message.endsWith("\n")) {
    message += "\n";
  }
  Serial.println("SETTING THE STAGE");
  for (uint16_t i = 0; i < message.length(); i++) {
    char c = message.charAt(i);
    cmdBuffer[i] = c;
  }
  cmdBuffer[message.length()] = '\0';
  WireManager::setStagged(message.length());
  Serial.print("ENDING STATE ");
  Serial.println(message.length());
}

void WireManager::clearCMD() {
  bufferReady = false;
  cmdBufferIndex = 0;
  index = 0;
}
