// Import af biblioteker
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <PubSubClient.h>
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <HX711.h>
#include <string>
#include<iostream>
#include "Setup.h"

char c;
int e;
String d;
String Machinenr;

static int ethanolrcp;
static int waterrcp;
static int glycerinrcp;
static int brintoveriltercp;
static int oilrcp;

int ethanolMass;
int waterMass;
int glycerinMass;
int brinoverilteMass;
int oilMass;

int mixingMass;
int mixingMassMin;
/*
  char* WhatIsTheWeight;
*/
//int recipe[5];
int distance;

//OLED
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ D1, /* data=*/ D2);   // pin remapping with ESP8266 HW I2C
//U8G2_SSD1306_128X32_WINSTAR_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//FLAME SENSOR
const int flamePin = D5;
int Flame = HIGH;

//LED'er

int ledRED = D6;
int ledGREEN = D7;

char pumpe1[5];
char pumpe2[5];
char pumpe3[5];
char pumpe4[5];
char pumpe5[5];

String ethanolrcpS;
String waterrcpS;
String glycerinrcpS;
String brintoveriltercpS;
String oilrcpS;

char* pump1 = pumpe1;
char* pump2 = pumpe2;
char* pump3 = pumpe3;
char* pump4 = pumpe4;
char* pump5 = pumpe5;


char *recipe[5] = {pump1, pump2, pump3, pump4, pump5};

/////// FUNKTIONSOPSÆTNING ////////


// Opretter en placeholder for callback-funktionen til brug senere. Den rigtige funktion ses længere nede.
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length);

// Opretter en klient der kan forbinde til en specifik internet IP adresse.
WiFiClient espClient; // Initialiserer wifi bibloteket ESP8266Wifi, som er inkluderet under "nødvendige bibloteker"

// Opretter forbindelse til mqtt klienten:
PubSubClient client(mqtt_server, mqtt_port, callback, espClient); // Initialiserer bibloteket for at kunne modtage og sende beskeder til mqtt. Den henter fra definerede mqtt server og port. Den henter fra topic og beskeden payload

/////// FUNKTIONSOPSÆTNING SLUT /////////

void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length) {
  // Konverterer indkomne besked (topic) til en string:
  String topic;
  topic = String(byteArraytopic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  /*String payloada;
    payloada = String(byteArrayPayload);
    Serial.print("Message payload is: ");
    Serial.print(payloada);
  */
  // Konverterer den indkomne besked (payload) fra en array til en string:
  // Topic == Kraftsensor
  payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
}

void setup() {
  Serial.begin(115200); /* begin serial for debug */
  Wire.begin(D3, D4); /* join i2c bus with SDA=D3 and SCL=D4 of NodeMCU */
  //u8g2.begin();

  setup_wifi(); // Kører WiFi loopet og forbinder herved.
  client.setServer(mqtt_server, mqtt_port); // Forbinder til mqtt serveren (defineret længere oppe)

  client.setCallback(callback); // Ingangsætter den definerede callback funktion hver gang der er en ny besked på den subscribede "cmd"- topic

  bool mixingstatus = LOW;
  bool dispensingstatus = LOW;
  int mixingMassMin = 500;

  //Flame
  pinMode(flamePin, INPUT);

  //LED'er
  pinMode(ledRED, OUTPUT);
  pinMode(ledGREEN, OUTPUT);
  digitalWrite(ledRED, LOW);
  digitalWrite(ledGREEN, LOW);
  Serial.println("Setup Done");

}

void loop() {




  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write("Hello Arduino");  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */

  Wire.requestFrom(8, 13); /* request & read data of size 13 from slave */
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.println();
  delay(1000);
  ///////////////////////// MQTT LOOP FUNCTION /////////////////////

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
