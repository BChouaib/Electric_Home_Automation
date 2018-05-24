const int SercetKnockTimeOut = 10000; // max time to wait whene recording
const int knockEpsilon = 100; // The max different time between secret knock an th listening knock to accepte
const int KnockThreshold = 1; // the value of the piezo sensor to consider as knock

long secretCode[10]; // Table hold time difference betwen knocks (10 mean 11 knocks)
int secretCodeNumKnock; // The number of knocks in the secret knock
int numKnocked = 0; // The number of knocks whene listening to knock
long lastKnockTime; // The time of the last Knock
long beginRecordKnockTime; // Time whene begin recording the new secret knock
boolean isListeningKnock = true; // if is listening to knock or record it

void initSecretKnock()
{
  loadKnock(); // to load the saved knock in the EEPROM
}

boolean knockManager() { // the basic methode to cal in the main loop and it is return true whene the right knock is knocked
  if (!isListeningKnock) {
    recordKnock();
    return false;
  } else {
    return listenKnock();
  }
}

void saveKnock() { // Save the knock to EEPROM
  EEPROM.update(60, secretCodeNumKnock);
  for (int i = 0; i < 10; i++) {
    EEPROM.put(61 + i * 4, secretCode[i]);
  }
}

void loadKnock() { // Load the knock from EEPROM
  secretCodeNumKnock = EEPROM.read(60);
  for (int i = 0; i < 10; i++) {
    EEPROM.get(61 + i * 4, secretCode[i] );
  }
}

void initRecordKnock() {
  isListeningKnock = false;
  secretCodeNumKnock = 0;
  beginRecordKnockTime = millis();
}


void recordKnock() {
  long currentTime = millis();
  if (beginRecordKnockTime + SercetKnockTimeOut >= currentTime) {
    if (secretCodeNumKnock <= 10) {
      long knockVal = getKnocked();
      if (knockVal > 0) {
        secretCodeNumKnock++;
        if (secretCodeNumKnock > 1) {
          secretCode[secretCodeNumKnock - 2] = knockVal;
        }
      }
    }
  } else { // Whene finish the recording knock
    isListeningKnock = true;
    saveKnock();
  }
}

boolean listenKnock() {
  long currentTime = millis();
  if (currentTime - lastKnockTime > 3000) { // To reset numKnocked for next listening
    numKnocked = 0;
  }
  long knockVal = getKnocked();
  if (knockVal > 0) {
    numKnocked++;
    if (numKnocked > 1) {
      if (abs(knockVal - secretCode[numKnocked - 2]) <= knockEpsilon) {
        if (numKnocked == secretCodeNumKnock) { // if the right knock is knocked
          numKnocked = 0;
          return true;
        }
      } else { // if the knock is wrong the reset numKnocked
        numKnocked = 0;
      }
    }
  }
  return false;
}

long getKnocked() {
  int KnockSensorValue = analogRead(A0);
  if (KnockSensorValue > KnockThreshold) {
    long deltaTime = millis() - lastKnockTime;
    if (deltaTime > 10) {
      lastKnockTime = millis();
      return deltaTime;
    }
  }
  return 0;
}





