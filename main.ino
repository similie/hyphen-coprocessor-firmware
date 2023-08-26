
#include <Arduino.h>
#include "wire-manager.h"
#include "file-manager.h"
#include "sdi-12.h"
//  #include "rs-485.h"


#define DEBUG true

#define CONSOLE_BAUD 115200
#define DEBUG_MODE true
// #define PROCESSOR_TYPE "MO_SAMD_21G18"
#define PROCESSOR_TYPE "FW_32u4"

unsigned long awaitTimer = 0;
const unsigned long LOG_OUT = 60000;

WireManager wire;
SDI12Controller sdi;
FileManager file;
// RS485Manager rs485;
/*
 * Startup functions
 */
void setup() {
  Serial.begin(CONSOLE_BAUD);
  wire.begin();
  sdi.init();
  sdi.setup();
  // rs485.begin();
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
  // sdi.setup();
  // delay(200);
  WireManager::setStagged(sdi.cmd(wire.receiveCmd(), wire.getBuffer()));
  // sdi.end();
}

void process485() {
//  int address = rs485.stripAddress(wire.receiveCmd());
//  Serial.print("MY ADDRESS ");Serial.println(address);
//  unsigned long size = rs485.process(address, wire.getBuffer());
//  Serial.print("MY SIZE ");Serial.println(size);
//  if (!size) {
//     return wire.setResponseMessage("!!ERROR::FAILED_TO_PULL_RS485_DATA!!");
//  }
//  WireManager::setStagged(size);
}


/**
 * processSerialCommand - sends the command to the correct function for processing
 *                
 * @param SDIReadEvent * eventt - contains the event data
 * @return void
 */
void processCommand() {
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
  } else if (cmd.startsWith("rs485")) {
    process485();
  } else if (cmd.startsWith("request")) {
    processesSID12CMD();
  } else if (cmd.startsWith("0R")) {
    processesSID12CMD();
  } else if (cmd.startsWith("ping")) {
    pong();
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
  processCommand();
  file.loop();

#ifdef DEBUG
  // for debugging
  if (millis() - awaitTimer < LOG_OUT) {
    return;
  }
  awaitTimer = millis();
  Serial.println("I'm Alive");
#endif
}
