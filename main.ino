
#include <Arduino.h>
#include "wire-manager.h"
#include "file-manager.h"
#include "sdi-12.h"

#define CONSOLE_BAUD 115200
#define DEBUG_MODE true
// #define PROCESSOR_TYPE "MO_SAMD_21G18"
#define PROCESSOR_TYPE "FW_32u4"

WireManager wire;
SDI12Controller sdi;
FileManager file;
/*
 * Startup functions
 */
void setup() {
  Serial.begin(CONSOLE_BAUD);
  wire.begin();
  sdi.init();
  // debug delay to salvage the chip
  delay(2000);
}


/**
* pushOpperation
* push a record onto a file
*/
void pushOpperation() {
  bool ok = file.push(wire.getBuffer(), wire.bufferLength());
  wire.setResponseMessage(ok ? "ok" : "!!ERROR::FAILED_TO_PUSH_FILE!!");
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
    return wire.setResponseMessage("!!ERROR::FAILED_TO_POP_FILE!!");
  }
  WireManager::setStagged(size);
}
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
  wire.setResponseMessage(send);
}

void logFile() {
  bool ok = file.logExternal(wire.getBuffer(), wire.bufferLength());
  wire.setResponseMessage(ok ? "ok" : "!!ERROR::FAILED_TO_LOG_TO_FILE!!");
}

/**
* processesSID12CMD
* runs the SDI-12 Command
*/
void processesSID12CMD() {
  WireManager::setStagged(sdi.cmd(wire.receiveCmd(), wire.getBuffer()));
}


/**
 * processSerialCommand - sends the command to the correct function for processing
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void processSerialCommand() {
  String cmd = wire.peakCommand();
  if (cmd.equals("")) {
    return;
  }
  Serial.println(cmd);
  if (cmd.startsWith("pop")) {
    pop();
  } else if (cmd.startsWith("push")) {
    pushOpperation();
  } else if (cmd.startsWith("log")) {
    logFile();
  } else if (cmd.startsWith("request")) {
    processesSID12CMD();
  } else if (cmd.startsWith("ping")) {
    pong();
  } else if (cmd.startsWith("0R")) {
    processesSID12CMD();
  } else if (cmd.startsWith("reset_wire")) {
    wire.reset();
  } else {
    Serial.print("Unknown Command ");
    Serial.println(cmd);
  }
  wire.clearCMD();
}

/**
 * loop - primary arduino loop
 *                
 * @return void
 */
void loop() {
  processSerialCommand();
  file.loop();
}
