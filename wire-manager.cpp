#include "wire-manager.h"



WireManager::WireManager() {

}

WireManager::WireManager(uint8_t address) {
  this->address = address;
}

int WireManager::getIndex() {
  if (_index > staggedReady) {
    _index = 0;
    return -1;
  }
  return _index++;
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

// bool WireManager::readyBuffer() {

//   if (!bufferReady) {
//     return bufferReady;
//   }
//   Serial.print("\n_+__+____");
//   Serial.println( millis() - bufferReadySet > BUFFER_READY_FAILED);
//   if ( millis() - bufferReadySet > BUFFER_READY_FAILED) {
//     Serial.println("MY READY BUFFER WAS VIOLATED");
//     bufferReady = false;
//     cmdBufferIndex = 0;
//   }
//   return bufferReady;
// }

void WireManager::receiveEvent(int byteCount) {
  // Serial.print("I JUST GOT RECEIVED ");Serial.println(byteCount);
  if (bufferReady) {
    return;
  }
  if (cmdBufferIndex + byteCount > CMD_BUFFER_LENGTH) {
    cmdBufferIndex = 0;
  }

  uint16_t i = 0;
  while(Wire.available() && i < byteCount) {
    char c = Wire.read();
    cmdBuffer[cmdBufferIndex] = c;
    cmdBufferIndex++;
    // Serial.print(c);
    if (c == '\n' || c == '\0' || c == '\r') {
      bufferReady = true;
      // bufferReadySet = millis();
      // Serial.println("---");
    }
    i++;
  }
  cmdBuffer[cmdBufferIndex] = '\0';
}

size_t WireManager::buildPrinterValues(char * sendBuffer) {
  int setIndex = getIndex();
  int endIndex = getEndIndex(setIndex);
  size_t localIndex = 0;
  for (uint16_t i = setIndex; i < getTotalStringLength(endIndex); i++) {
    char c = cmdBuffer[i];
    sendBuffer[localIndex] = c;
    localIndex++;
  }
  sendBuffer[localIndex] = '\0';
  if (endIndex == -1) {
    clearStagged();
  }
  return localIndex;
}

void WireManager::requestEvent() {
  if (staggedReady == 0) {
    return sendFailedWire();
  }
  char printer[WIRE_BUFFER_SIZE + 1];
  buildPrinterValues(printer);
  Serial.print("MY VALUES ARE HERE ");Serial.println(printer);
  Wire.print(printer);
  Serial.println("\n----");
  Serial.print(String(printer));
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
  _index = 0;
  staggedReady = staggedValue;
}

void WireManager::clearStagged() {
  WireManager::zeroOutBuffer();
  setStagged(0);
}

uint8_t WireManager::peakCommand(char* buffer, uint8_t bufferSize) {
  if (!bufferReady) {
    return 0;
  }
  uint16_t index = 0;
  uint8_t bufferIndex = 0;
  while (index < bufferSize) {
    char c = cmdBuffer[index];
    index++;
    if (c == ' ' || c == '\n' || c == '\0' || c == 13) {
      break;
    }
    buffer[bufferIndex] = c;
    bufferIndex++;
    if (bufferIndex >= bufferSize - 1) {
      break;
    }
  }
  buffer[bufferIndex] = '\0';
  return bufferIndex;
}

char* WireManager::getBuffer() {
  return cmdBuffer;
}

unsigned long WireManager::bufferLength() {
  return cmdBufferIndex;
}

void WireManager::printBuffer() {
  uint16_t i = 0;
  char c = cmdBuffer[i];
  Serial.println("\nPRINTING BUFFER");
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
  setResponseMessage(message.c_str());
}

void WireManager::setResponseMessage(const char* message) {
  clearCMD();
  WireManager::zeroOutBuffer(); 
  size_t length = strlen(message);
  bool append = message[length - 1] != '\n';
  Serial.print("APPENDING ");Serial.println(append);
  uint16_t i = 0;
  for (i; i < length; i++) {
    char c = message[i];
    cmdBuffer[i] = c;
  }
  if (append) {
   cmdBuffer[i] = '\n';
   i++;
  }
  cmdBuffer[i] = '\0';
  Serial.print("WHYYYYY ");
  Serial.print(length);
  Serial.print(" ");
  Serial.println(i);
  WireManager::setStagged(i);
}

void WireManager::clearCMD() {
  bufferReady = false;
  cmdBufferIndex = 0;
  _index = 0;
  //WireManager::zeroOutBuffer();
}

void WireManager::zeroOutBuffer() {
  // memset(cmdBuffer, 0, sizeof(cmdBuffer));
  for (uint16_t i = 0; i < CMD_BUFFER_LENGTH; i++) {
    cmdBuffer[i] = '\0';
  }
}
