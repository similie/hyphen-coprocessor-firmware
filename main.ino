#define NO_RS485 true
// #define NO_WEATHER true
// #define NO_FILE_STORAGE true
#include "wire-manager.h"
#ifndef NO_FILE_STORAGE
#include "file-manager.h"
#endif
#ifndef NO_WEATHER
#include "sdi-12.h"
#endif
#ifndef NO_RS485
#include "rs-485.h"
#endif
#include "utils.h"

#define DEBUG true
#define CONSOLE_BAUD 115200
#define PROCESSOR_TYPE "MO_SAMD_21G18"
// #define PROCESSOR_TYPE "FW_32u4"
#define DEFAULT_SERIAL_RS845_PORT 9600
#define INVALID_DEVICE 0

#ifdef DEBUG
unsigned long awaitTimer = 0;
const unsigned int LOG_OUT = 60000;
#endif

const uint8_t SEND_BUFFER_SIZE = 24;
char sentCommandsBuffer[SEND_BUFFER_SIZE];

WireManager wire;
#ifndef NO_WEATHER
SDI12Controller sdi;
#endif
#ifndef NO_FILE_STORAGE
FileManager file;
#endif
#ifndef NO_RS485
RS485Manager rs485; 
#endif
/*
 * Startup functions
 */
void setup() {
  Serial.begin(CONSOLE_BAUD);
  wire.begin();
#ifndef NO_RS485
  rs485.begin();
#endif
#ifndef NO_WEATHER
  sdi.init();
#endif
  // debug delay to salvage the chip
  delay(2000);
}

#ifndef NO_FILE_STORAGE
/**
* pushOpperation
* push a record onto a file
*/
void pushOpperation() {
  bool ok = file.push(wire.getBuffer(), wire.bufferLength());
  wire.setResponseMessage(ok ? "ok" : "!!ERROR::FAILED_PUSH_FILE!!");
}

/**
 * pop
 * 
 * Used to pop a record off the file
 * 
 * 
 * @return void
 */
void pop() {
  wire.clearCMD();
  unsigned long size = file.pop(wire.getBuffer());
  if (!size) {
    return wire.setResponseMessage("!!ERROR::FAILED_POP_FILE!!");
  }
  WireManager::setStagged(size);
}

void logFile() {
  bool ok = file.logExternal(wire.getBuffer(), wire.bufferLength());
  wire.setResponseMessage(ok ? "ok" : "!!ERROR::FAILED_TO_LOG_TO_FILE!!");
}

#endif
/**
 * pong
 * 
 * Used to respond to the the primary that's checking if the
 * serial device is available
 * 
 * @return void
 */
void pong() {
  String send = "pong " + String(PROCESSOR_TYPE);
  wire.setResponseMessage(send.c_str());
}


/**
* processesSID12CMD
* runs the SDI-12 Command
*/
void processesSID12CMD() {
#ifndef NO_WEATHER
  WireManager::setStagged(sdi.cmd(wire.getBuffer(), wire.bufferLength()));
#endif
#ifdef NO_WEATHER
  wire.setResponseMessage("!!ERROR::FAILED_ALL_WEATHER!!");
#endif
}

void process485() {
#ifndef NO_RS485  
  clear();
  Utils::postCommandBuffer(wire.getBuffer(), sentCommandsBuffer, wire.bufferLength(), SEND_BUFFER_SIZE);
  int address = Utils::bufferToInt(sentCommandsBuffer);
  unsigned long size = rs485.process(address, wire.getBuffer());
  Serial.print("THIS IS THE SIZE ");Serial.println(size);
  if (!size) {
    return wire.setResponseMessage("!!ERROR::FAILED_RS485!!");
  }
  WireManager::setStagged(size);
#endif
#ifdef NO_RS485
  wire.setResponseMessage("!!ERROR::FAILED_RS485!!");
#endif
}


void clear() {
  for (uint8_t i = 0; i < SEND_BUFFER_SIZE; i++) {
    sentCommandsBuffer[i] = '\0';
  }
}

/**
 * processSerialCommand - sends the command to the correct function for processing
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void processCommand() {
  uint8_t length = wire.peakCommand(sentCommandsBuffer, SEND_BUFFER_SIZE);
  if (!length) {
    return;
  }
  // Serial.print("Length ");
  // Serial.print(length);
  // Serial.print(" ");
  Serial.println(String(sentCommandsBuffer));
  // return;

#ifndef NO_FILE_STORAGE
  if (Utils::startsWith("pop", sentCommandsBuffer, length)) {
    pop();
  } else if (Utils::startsWith("push", sentCommandsBuffer, length)) {
    pushOpperation();
  } else if (Utils::startsWith("log", sentCommandsBuffer, length)) {
    logFile();
  } else
#endif

  if (Utils::startsWith("rs485", sentCommandsBuffer, length)) {
    process485();
  } else if (Utils::startsWith("request", sentCommandsBuffer, length)) {
    processesSID12CMD();
  } else if (Utils::startsWith("ping", sentCommandsBuffer, length)) {
    pong();
  } else if (Utils::startsWith("reset_wire", sentCommandsBuffer, length)) {
    wire.reset();
  } else {
    Serial.println("!Unknown Command!");
    wire.setResponseMessage("!!ERROR::UNKNOWN_CMD!!");
  }

  Serial.println("\n__________");
  clear();
  wire.clearCMD();
}

/**
 * loop - primary arduino loop
 *                
 * @return void
 */
void loop() {
  processCommand();

#ifndef NO_FILE_STORAGE
  file.loop();
#endif

#ifdef DEBUG
  // for debugging
  if (millis() - awaitTimer < LOG_OUT) {
    return;
  }
  awaitTimer = millis();
  Serial.println("I'm Alive");
#endif
}
