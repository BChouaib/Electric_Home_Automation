#include <EEPROM.h>
#include <ESP8266_Lib.h>
#include <SoftwareSerial.h>


SoftwareSerial EspSerial(2, 3);
ESP8266 wifi(&EspSerial);


void setup() {

  Serial.begin(9600);

  EspSerial.begin(9600);
  delay(10);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  setUpWIFI();
  digitalWrite(LED_BUILTIN, LOW);
  

}

void loop() {




}




