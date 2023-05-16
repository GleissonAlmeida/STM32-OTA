/*
  <2022> <Gleisson Almeida>

  basead  from original code CS.NOL
  2017  CS.NOL  https://github.com/csnol/STM32-OT

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  3. Neither the name of the copyright holders nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    ESP8266/8285 Pin       STM32 MCU      NodeMCU Pin(ESP8266 based)
    RXD                    PA9             RXD
    TXD                    PA10            TXD
    Pin4                   BOOT0           D2
    Pin5                   RST             D1
    Vcc                    3.3V            3.3V
    GND                    GND             GND
    En -> 10K -> 3.3V
    TX-SERIAL (PC)                         D6
    RX-SERIAL (PC)                         D7

*/
#include "stm32ota.h"
#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include "ArduinoJson.h"  //Version 6.15.2
#define ARDUINOJSON_USE_LONG_LONG 1

SoftwareSerial Debug;  //For debug only
#define MYPORT_TX 14   //For debug only
#define MYPORT_RX 13   //For debug only

stm32ota STM32(5, 4, 2);  //For use with libray STM32OTA


const char* ssid = "you ssid";  //you ssid
const char* password = "you password";  //you password
const char* link_Updt = link_Updt = "you Link";
char link_bin[100];
boolean MandatoryUpdate = false;
//----------------------------------------------------------------------------------
const int buttonPin = 9;
const int ledPin = 2;
boolean aux = false;
unsigned long lastTime;
int button = true;

//----------------------------------------------------------------------------------
void wifiConnect() {
  Debug.println("");
  WiFi.disconnect(true);  
  WiFi.mode(WIFI_STA);
  delay(2000);  //Aguarda a estabilizaçao do modulo.
  WiFi.begin(ssid, password);
  byte b = 0;
  while (WiFi.status() != WL_CONNECTED && b < 60) {  //Tempo de tentativa de conecxao - 60 segundos
    b++;
    Debug.print(".");
    delay(500);
  }
  Debug.println("");
  Debug.print("IP:");
  Debug.println(WiFi.localIP());
}
//----------------------------------------------------------------------------------
void checkupdt(boolean all = true) {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, link_Updt);
  int httpCode = http.GET();
  String s = "";
  s = http.getString();
  http.end();
  s.trim();

  if (all) {
    Debug.println(s);  //usar apenas no debug
  }


  if (httpCode != HTTP_CODE_OK) {
    return;
  }

  StaticJsonDocument<800> doc;
  deserializeJson(doc, s);
  strlcpy(link_bin, doc["link"] | "", sizeof(link_bin));
  MandatoryUpdate = doc["mandatory"] | false;
  Debug.println(link_bin);  //For debug only
  //Debug.println(MandatoryUpdate);                   //For debug only
}

//----------------------------------------------------------------------------------
void setup() {
  Debug.begin(9600, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);  //For debug only
  Serial.begin(9600, SERIAL_8E1);
  Debug.println("DEBUG SOFTWARESERIAL");
  SPIFFS.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  delay(200);
  wifiConnect(); 
  delay(200);
  //STM32.RunMode();
  
  checkupdt();
  Debug.println("END OF INITIALIZATION");
}

void loop() {

  button = digitalRead(buttonPin);
  if (!button) {
    digitalWrite(ledPin, HIGH);
    Debug.println("START UPDATE");
    delay(2000);
    checkupdt(false);
    String myString = String(link_bin);
    Debug.println(STM32.otaUpdate(myString));  //For debug only
    Debug.println("END OF UPDT");              //For debug only
  }
  //_______________________________________________________________________

  if (millis() - lastTime > 500) {             //BLINK LED BULTIN
    if (aux) {
      aux = false;
    } else aux = true;
    lastTime = millis();
    digitalWrite(ledPin, aux);
  }
}
