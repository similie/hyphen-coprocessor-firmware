#include "utils.h"

unsigned int Utils::postCommandBuffer(char* from, char* to, unsigned int maxlength ,unsigned int maxSize) {
    uint16_t index = 0;
    uint8_t toIndex = 0;
    bool start = false;
    while(index < maxlength) {
      char c = from[index];
      // Serial.print(c);
      index++;
      if (c == ' ' && !start) {
        start = true;
        continue;
      } else if (!start) {
        continue;
      } else if (c == '\n') {
        break;
      }
      
      to[toIndex] = c;
      toIndex++;
      if (c == '\0' || toIndex >= maxSize - 1) {
        break;
      }
    }
    to[toIndex] ='\0';
    return toIndex;
}

int Utils::bufferToInt(char* buffer) {
  return atoi(buffer);
}

bool Utils::startsWith(const char* starts, char* against, unsigned int startsSize) {
  for (unsigned int i = 0; i < startsSize; i++) {
    char check = starts[i];
    if (check == '\0' && i > 0) {
      break;
    }
    if (check != against[i]) {
      return false;
    }
  }
  return true;
}