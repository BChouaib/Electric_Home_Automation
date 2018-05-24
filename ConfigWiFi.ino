char ssid[30] ;
char pass[30] ;

//this methode
//  - read ssid and pass from EEPROM
//  - try to join to the AP
//  - if failed it will call configWIFI() and save new ssid and pass into EEPROM for th next join (after power off the arduino)
void setUpWiFi() {
  wifi.setEcho(0);
  wifi.enableMUX();
  readSsidAndPassEEPROM();
  while (!wifi.joinAP(ssid, pass)) {
    delay(10000);
    configWIFI();
    writeSsidAndPassEEPROM();
  }
}

//save the ssid and pass into EEPROM
void writeSsidAndPassEEPROM() {
  for (int i = 0; i < 30; i++) {
    EEPROM.write(i, ssid[i]);
  }
  for (int i = 0; i < 30; i++) {
    EEPROM.write(30 + i, pass[i]);
  }
}

//read the ssid and pass from EEPROM
void readSsidAndPassEEPROM() {
  for (int i = 0; i < 30; i++) {
    ssid[i] = EEPROM.read(i);
  }
  for (int i = 0; i < 30; i++) {
    pass[i] = EEPROM.read(30 + i);
  }
}

//create html page (192.168.4.1) to config the wifi
//this methode block until wifi config is done (page saveed showed up)
void configWIFI() {
  wifi.setOprToSoftAP();
  wifi.startServer(80);
  while (true) {
    if (EspSerial.available()) {
      if (EspSerial.find("+IPD,")) {
        if (EspSerial.find("ssid=")) {
          EspSerial.readStringUntil('&').toCharArray(ssid, 30);
          if (EspSerial.find("pass=")) {
            EspSerial.readStringUntil(' ').toCharArray(pass, 30);
            wifi.send(0, "<h1>Saved</h1>", 14);
            wifi.releaseTCP(0);
            break;
          }
        }
        //This is commented for reason of limited memory of the Arduino Uno
        //wifi.send(0, "<!DOCTYPE html> <html> <style> input[type=text], select { width: 100%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; } input[type=submit] { width: 100%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; } div { padding: 20px; } body{ margin: auto; width: 40%;} </style> <body> <h3>Wifi configuration</h3> <div> <form action=\"/\"> SSID <input type=\"text\" name=\"ssid\" placeholder=\"Your SSID..\"> <br> PASS <input type=\"text\"name=\"pass\" placeholder=\"Your Password..\"> <br><br> <input type=\"submit\" value=\"Save\"> </form> </div> </body> </html>"
        //          , 694);
        //wifi.releaseTCP(0);
      }
    }
  }
  wifi.stopServer();
  wifi.setOprToStation();
  wifi.restart();
  wifi.enableMUX();
}

