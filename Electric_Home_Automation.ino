//Pins definition
#define ESP_RX_PIN 2 //connected to TX of the ESP8266
#define ESP_TX_PIN 3 //connected to RX of the ESP8266
#define CS_SENDER_PIN 5 // Pin connected to the sender of cs
#define CS_RECEIVER_PIN 4 // Pin connected to the receiver of cs
#define DHT_PIN 12
#define LDR_PIN A0
#define PIZEO_PIN A1 // Pin connected to the piezo

#define LIGHT_ROOM_PIN 11
#define LIGHT_COULOIR_PIN 10
#define LIGHT_CUISINE_PIN 6
#define TV_PIN A5
#define CHAUFFAGE_PIN 7
#define AC_PIN 8
#define DOOR_SERVO_PIN 9

//Virtual Pins definition
#define LIGHT_ROOM_AUTO_CONTROL_VPIN V0
#define LIGHT_ROOM_STATE_VPIN V1
#define LIGHT_ROOM_DENSITY_VPIN V2
#define LIGHT_COULOIR_STATE_VPIN V3
#define LIGHT_COULOIR_DENSITY_VPIN V4
#define LIGHT_CUISINE_STATE_VPIN V5
#define LIGHT_CUISINE_DENSITY_VPIN V6
#define TV_VPIN V7
#define CHAUFFAGE_VPIN V8
#define AC_VPIN V9
#define DOOR_CONTROL_VPIN V10

//-----------------------------------------------------------------------------
#define DHTTYPE DHT11
/////////////////////////////////////////////////////////////////////////////

//Libraries
#include <EEPROM.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SoftwareSerial.h>
#include "DHT.h"
/////////////////////////////////////////////////////////////////////////////

//Variables
//char auth[] = "03a3a009a5764294ad70324d9ac0f06f"; // using blynk-cloud
char auth[] = "116918645ebe4987bf656c5993afd1ae"; // using local server

SoftwareSerial EspSerial(ESP_RX_PIN, ESP_TX_PIN); //RX,TX
ESP8266 wifi(&EspSerial);
BlynkTimer timer;

DHT dht(DHT_PIN, DHTTYPE);


byte ledState = LOW; //for blinkin LED

bool LIGHT_ROOM_AUTO_CONTROL ;
byte LIGHT_ROOM_STATE ;
byte LIGHT_ROOM_DENSITY;
/////////////////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);

  EspSerial.begin(9600);
  delay(10);

  // init Pin Mode ///////////////////////////////////////////////////////////
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT_ROOM_PIN, OUTPUT);
  pinMode(LIGHT_COULOIR_PIN, OUTPUT);
  pinMode(LIGHT_CUISINE_PIN, OUTPUT);
  pinMode(TV_PIN, OUTPUT);
  pinMode(CHAUFFAGE_PIN, OUTPUT);
  pinMode(AC_PIN, OUTPUT);

  ////////////////////////////////////////////////////////////////////////////
  digitalWrite(LED_BUILTIN, HIGH);
  setUpWiFi();
  digitalWrite(LED_BUILTIN, LOW);

  //Blynk.config(wifi, auth);// using blynk-cloud
  Blynk.config(wifi, auth, "192.168.1.72", 8080);// using local server

  while (!Blynk.connect()); //Block if can't connect

  //tone(SPEAKER_PIN, 4000, 100); //Beep

  //dht.begin();

  initSecretKnock(); // init the SecretKnock
  initCSgesture(); // init the CSgesture

  timer.setInterval(500, blinkLED);
  //timer.setInterval(500, checkLight);
  //timer.setInterval(2000, checkTemperature);

}

void loop() {
  Blynk.run();

  timer.run();

  if ( getKnocked() > 0) { // to manage the knock secret
    Blynk.virtualWrite(V7, 1);
  }

}

BLYNK_CONNECTED() {
    Blynk.syncAll();
}

//Methode based on timer
void checkLight() {
//  if (LIGHT_AUTO_CONTROL) {

  //}
}


void checkTemperature() {
  float t = dht.readTemperature();
  if (!isnan(t)) {
    Blynk.virtualWrite(V5, t);
  }
}


void blinkLED() {
  if (ledState == LOW) {
    ledState = HIGH;
  } else {
    ledState = LOW;
  }
  digitalWrite(LED_BUILTIN, ledState);

}
//////////////////////////////////////////////////////////////////////////////////////
//Blynk update Virtual Pins
BLYNK_WRITE(LIGHT_ROOM_AUTO_CONTROL_VPIN) {
  LIGHT_ROOM_AUTO_CONTROL = param.asInt();
}

BLYNK_WRITE(LIGHT_ROOM_STATE_VPIN) {
  LIGHT_ROOM_STATE = param.asInt();
  if(LIGHT_ROOM_STATE == 0){
    digitalWrite(LIGHT_ROOM_PIN, LOW);
  }else{
    digitalWrite(LIGHT_ROOM_PIN, HIGH);
  }
}

BLYNK_WRITE(LIGHT_ROOM_DENSITY_VPIN) {
  LIGHT_ROOM_DENSITY = param.asInt();
  if(LIGHT_ROOM_STATE == 1){
    analogWrite(LIGHT_ROOM_PIN, LIGHT_ROOM_DENSITY);
  }
}



//////////////////////////////////////////////////////////////////////////////////////
