#ifndef FILE_MANAGER_h
#define FILE_MANAGER_h

#include "Arduino.h"
#include <SD.h>


class FileManager {
private:
  const unsigned long SD_CARD_INIT_CHECK = 100000;
  const char* POSITION_FILE = "position";
  const char* FILE_NAME = "reads.txt";
  const char* TEMP_FILE = "temp.txt";
  const char* LOG_FILE = "log.txt";
  bool hasFile = false;
  unsigned long sdInstantiator = 0;
  int address = 9;
  int readInSize = 0;
  unsigned long requestBufferLength = 0;
  const int chipSelect = 4;
  void checkSDCard();
  bool kilFile(const char* );
  bool setPosition(unsigned long);
  unsigned long getPosition();
  unsigned long getLineCount(char*);
  void appendToBuffer(char*);
  int getPopSize(String ourReading);
  bool writeToFile(const char*, char*, unsigned long);
public:
  FileManager();
  void init();
  bool isConnected();
  void loop();
  bool logExternal(char * , unsigned long);
  bool push(char * , unsigned long);
  unsigned long pop(char *);
};

#endif
