
#include "file-manager.h"

FileManager::FileManager() {
}

/**
 * kilFile - removes a file from the sd card by filename
 * 
 * @param const * char filename - the file to remove
 * @return bool - true if the file is removed
 */
bool FileManager::kilFile(const char* filename) {
  return SD.remove(filename);
}

/**
 * setPosition - retains the file position index for later retrieval
 * 
 * @param size_t index - the index of the line being returned
 * @return bool - true when everything works out
 */
bool FileManager::setPosition(unsigned long position) {
  if (SD.exists(POSITION_FILE) && !kilFile(POSITION_FILE)) {
    hasFile = false;
    return false;
  }
  File myFile = SD.open(POSITION_FILE, FILE_WRITE);
  myFile.println(String(position));
  myFile.close();
  return true;
}


/**
 * getPosition - gets the value of the file position stored in file 
 * 
 * @return unsigned long - the value of the last saved position
 */
unsigned long FileManager::getPosition() {
  if (!SD.exists(POSITION_FILE)) {
    return 0;
  }
  File myFile = SD.open(POSITION_FILE);
  if (!myFile) {
    hasFile = false;
    return 0;
  }
  char readChar;
  String value;
  while (myFile.available()) {
    readChar = myFile.read();
    value += String(readChar);
  }
  myFile.close();
  return strtoul(value.c_str(), NULL, 10);
}





/**
 * getLineCount - gets the number of lines in the storage file as input
 * 
 * @param int count - the number of lines being requested
 * @return unsigned long - the value of the last traversed position
 */
unsigned long FileManager::getLineCount(char * buffer) {
//  int count = 1; // we are going to restrict to in line
 unsigned long position = getPosition();
  bool exists = SD.exists(FILE_NAME);
  if (!exists)
  {
    File myFile = SD.open(FILE_NAME, FILE_WRITE);
    myFile.close();
    return position;
  }

  File myFile = SD.open(FILE_NAME);
  char readChar;
  size_t i = 0;
  const unsigned long SIZE = myFile.size();
  // Postion the file from the last know position
  myFile.seek(position);
  unsigned long tickIndex = 0;
  while (myFile.available())
  {
    char readChar = myFile.read();
    // write to buffer
    buffer[tickIndex] = readChar;
    Serial.print(readChar);
    tickIndex++;
    position = myFile.position();
    if (readChar == '\n')
    {
      break;
    }
  }
  buffer[tickIndex] = '\0';
  myFile.close();
  if (position < SIZE)
  {
    setPosition(position);
  }
  else
  {
    // now we beach our position file
    kilFile(FILE_NAME);
    kilFile(POSITION_FILE);
  }
  return tickIndex;
}

/**
 * getPopSize - pulls the interger pop value from the request string
 * 
 * @param String ourReading - the string over the serial bus i.e. pop 5
 * @return int - the interger value to pop
 */
int FileManager::getPopSize(String ourReading)
{
  String substring = ourReading.substring(4);
  return atoi(substring.c_str());
}


/**
 * pop - pops lines from the storage file requested over serial
 * 
 * @param SDIReadEvent * event - the event containing the data being filled
 * @return void
 */
unsigned long FileManager::pop(char * buffer) {
  if (!hasFile) {
    return 0;
  }
  unsigned long bufferSize = getLineCount(buffer);
  return bufferSize;
}


// /**
//  * getPopIndex - appends the popped data with with the pop identity
//  * and it's corresponding index
//  * 
//  * @param size_t index - the index of the line being returned
//  * @return String - the concatenated string that contains the index
//  */
// String FileManager::getPopIndex(size_t index) {
//   return String("pop ") + String(index) + String(" ");
// }

bool FileManager::writeToFile(const char* file, char * buffer, unsigned long size) {
 if (!hasFile) {
    return hasFile;
  }
  File myFile = SD.open(file, FILE_WRITE);
  if (!myFile) {
    hasFile = false;
    return hasFile;
  }
  bool startNow = false;
  for (uint16_t i = 0; i < size; i++) {
    char c = buffer[i];
    if (!startNow &&  c == ' ') {
      startNow = true;
      continue;
    }  else if (!startNow) {
      continue;
    }
    myFile.write(c);
    Serial.print(c);
  }
  myFile.close();
  return hasFile;
}

bool FileManager::logExternal(char * buffer, unsigned long length) {
  return writeToFile(LOG_FILE, buffer, length);
}

/**
 * push - pushes data lines requested over serial
 * 
 * @param SDIReadEvent * event - the event containing the data being filled
 * @return void
 */
bool FileManager::push(char * buffer, unsigned long length) {
  return writeToFile(FILE_NAME, buffer, length);
}

/**
 * checkSDCard 
 * 
 * If we pop a new SD card after it has booted. We want to be 
 * able to respond
 * 
 * @param const * char filename - the file to remove
 * @return bool - true if the file is removed
 */
void FileManager::checkSDCard() {
  if (hasFile) {
    return;
  }
  if (sdInstantiator >= SD_CARD_INIT_CHECK) {
    init();
    sdInstantiator = 0;
  } else {
    sdInstantiator++;
  }
}

void FileManager::loop() {
  checkSDCard();
}

/**
 * initializeTheSDCard  
 * 
 * We want the file to instantiate when we
 * have it in the device
 *
 * @return void
 */
void FileManager::init() {
  if (hasFile) {
    return;
  }
  Serial.println("STARTING CHECKS");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    hasFile = false;
  } else {
    Serial.println("SD Card Available");
    hasFile = true;
  }
}