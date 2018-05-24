#include <CapacitiveSensor.h>

const int MIN_Value_Detec = 2; // the minimum value of the CS sensor to read
const int SercetCSTimeOut = 10000; // max time to wait whene recording

int secretCSCode[20] ; // Table hold code of CS
int secretCSCodeNumbr = 0; // The number of guesture in the secret cs code
int numCSgesture = 0; // The number of gesture listened
int lastReadedCSVal = 0; // the laste value of CS sensor (use to know when user take off his hand)
long lastCSReadTime ; //the time of laste reading to avoid wrong value
long beginRecordCSTime; // Time whene begin recording the new secret gesture
boolean isListeningCS = true; // if is listening to CS or record it

CapacitiveSensor   cs = CapacitiveSensor(CS_SENDER_PIN, CS_RECEIVER_PIN);

void initCSgesture() {
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
  loadCS();
}

boolean CSManager() { // the basic methode to cal in the main loop and it is return true whene the right knock is knocked
  if (!isListeningCS) {
    recordCS();
    return false;
  } else {
    return checkGestureCode();
  }
}

void saveCS() { // Save the CS to EEPROM
  EEPROM.update(200, secretCSCodeNumbr);
  for (int i = 0; i < 20; i++) {
    EEPROM.update(201 + i, secretCSCode[i]);
  }
}

void loadCS() { // Load the CS from EEPROM
  secretCSCodeNumbr = EEPROM.read(200);
  for (int i = 0; i < 20; i++) {
    secretCSCode[i] = EEPROM.read(201 + i);
  }
}

void initRecordCS() {
  Serial.println("begin recored");
  isListeningCS = false;
  secretCSCodeNumbr = 0;
  beginRecordCSTime = millis();
}

void recordCS() {
  long currentTime = millis();
  if (beginRecordCSTime + SercetCSTimeOut >= currentTime) {
    if (secretCSCodeNumbr < 20) {
      int csVal = listenToCS();
      if (csVal > 0) {
        Serial.println(csVal);
        secretCSCodeNumbr++;
        secretCSCode[secretCSCodeNumbr - 1] = csVal;
      }
    }
  } else { // Whene finish the recording cs
    isListeningCS = true;
    saveCS();
    Serial.println("finish recored");
  }
}

boolean checkGestureCode() {
  long currentTime = millis();
  if (currentTime - lastCSReadTime > 3000) { // To reset secretCSCodeNumbr for next listening
    numCSgesture = 0;
  }
  int csVal = listenToCS();
  if (csVal > 0) {
    Serial.println(csVal);
    numCSgesture++;
    if (abs(secretCSCode[numCSgesture - 1] - csVal) <= 1) { // for smooth read (accept the error of 1 )
      if (numCSgesture == secretCSCodeNumbr) {
        numCSgesture = 0;
        return true;
      }
    } else {
      numCSgesture = 0;
    }
  }
  return false;
}



int listenToCS() {
  if (millis() - lastCSReadTime > 200) { // overlook the value from the sensor after send cs val until 200ms
    long csVal =  cs.capacitiveSensor(20) / 100;
    if (csVal >= MIN_Value_Detec) {
      if (csVal > lastReadedCSVal) {
        lastReadedCSVal = csVal;
      }
    } else {
      if (lastReadedCSVal != 0) {
        int returnVal = lastReadedCSVal;
        lastReadedCSVal = 0;
        lastCSReadTime = millis();
        return returnVal;
      }
    }
  }
  return 0;
}



















