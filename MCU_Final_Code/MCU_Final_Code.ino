// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the final project of the course, Project 3 - Corona (Final project)


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
#include "Setup.h"
#include <HX711.h> //Bug
#include <string>
#include<iostream>




/////// FUNKTIONSOPSÆTNING ////////


// Opretter en placeholder for callback-funktionen til brug senere. Den rigtige funktion ses længere nede.
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length);

// Opretter en klient der kan forbinde til en specifik internet IP adresse.
WiFiClient espClient; // Initialiserer wifi bibloteket ESP8266Wifi, som er inkluderet under "nødvendige bibloteker"

// Opretter forbindelse til mqtt klienten:
PubSubClient client(mqtt_server, mqtt_port, callback, espClient); // Initialiserer bibloteket for at kunne modtage og sende beskeder til mqtt. Den henter fra definerede mqtt server og port. Den henter fra topic og beskeden payload

/////// FUNKTIONSOPSÆTNING SLUT /////////


//connect to the wifi access point

//
//
//
//
//
//

///////// CALLBACKFUNKTION ////////

// Definerer callback funktionen der modtager beskeder fra mqtt
// OBS: den her funktion kører hver gang MCU'en modtager en besked via mqtt
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length) {
  // Konverterer indkomne besked (topic) til en string:
  String topic;
  topic = String(byteArraytopic);
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload


  //////////////////////////////////////////////////// INGREDIENTS SETUP //////////////////////////////////////////////////////////////


  ///////////////////////// ETHANOL CONFIGURATION /////////////////////////

  if (topic == "Ethanol") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    ethanolrcp = payload.toInt() + 1000; //Cryptation to be used for I2C bus
    ethanolrcpS = String(ethanolrcp);
    ethanolrcpS.toCharArray(pumpe1, 5);

  }

  ///////////////////////// WATER CONFIGURATION /////////////////////////

  if (topic == "Water") {
    // Nulstiller payload, endhour og endmin variablen så forloopet ikke appender til en allerede eksisterende definationer
    payload = "";
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    waterrcp = payload.toInt() + 2000; //Cryptation to be used for I2C bus
    waterrcpS = String(waterrcp);
    waterrcpS.toCharArray(pumpe2, 5);

  }

  ///////////////////////// GLYCERIN CONFIGURATION /////////////////////////

  if (topic == "Glycerin") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    glycerinrcp = payload.toInt() + 3000; //Cryptation to be used for I2C bus
    glycerinrcpS = String(glycerinrcp);
    glycerinrcpS.toCharArray(pumpe3, 5);

  }

  ///////////////////////// H2O2 CONFIGURATION /////////////////////////

  if (topic == "Brintoverilte") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    brintoveriltercp = payload.toInt() + 4000; //Cryptation to be used for I2C bus
    brintoveriltercpS = String(brintoveriltercp);
    brintoveriltercpS.toCharArray(pumpe4, 5);

  }

  ///////////////////////// Oil CONFIGURATION /////////////////////////

  if (topic == "Oil") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    mixingStatus = 1; // When the last ingredient has been configured, the mixingStatus variable becomes 1, which initiated the Wire.Write function that communicates to the Arduino Uno
    mixing = 1; // Same goes for the mixing variable which enables the OLED screen to display "mixing...." untill the mixing process has been complete.
    oilrcp = payload.toInt() + 5000; //Cryptation to be used for I2C bus
    oilrcpS = String(oilrcp);
    oilrcpS.toCharArray(pumpe5, 5);

    Serial.print(pumpe5);
  }

  ///////////////////////// CHOOSING MACHINE /////////////////////////

  if (topic == "Machinenr") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    String Machinenr = payload;//
    Serial.print(payload);


  }
}

///////// CALLBACK SLUT /////////

int len;

void setup() {
  Serial.begin(115200); // begin serial for debug
  setup_wifi(); // Kører WiFi loopet og forbinder herved.
  client.setServer(mqtt_server, mqtt_port); // Forbinder til mqtt serveren (defineret længere oppe)
  client.setCallback(callback); // Ingangsætter den definerede callback funktion hver gang der er en ny besked på den subscribede "cmd"- topic


  //Flame
  pinMode(flamePin, INPUT);

  //LED'er
  pinMode(ledRED, OUTPUT);
  pinMode(ledGREEN, OUTPUT);
  digitalWrite(ledRED, LOW);
  digitalWrite(ledGREEN, LOW);
  Serial.println("Setup Done");
  Wire.begin(D3, D4); // join i2c bus with SDA=D3 and SCL=D4 of NodeMCU
  u8g2.begin(); //Starter OLED skærm
}


void loop() {


  /////////////////// UPDATE OLED SCREEN //////////////////

  if (millis() >= update_time + 5000 && d1 != "3000") { //Every five seconds, if there isn't another screen being run, the OLED should update itself.
    update_time += 5000;
    OLEDStandard();


  }

  /////////////////// Read function I2C //////////////////
  if (millis() >= time_now + 1000) {
    time_now += 1000;
    Wire.requestFrom(8, 8); // request & read data of size 4 from slave //
    int f = 0;
    while (Wire.available()) {
      mixing = 0;
      char c = Wire.read(); //Read charachters to c
      d += c; // Collect characters to a string
      f ++;
      //// Seperate the input to two seperate weight sensors ////
      if (f < 5) {
        d1 += c; // Collect characters to a string
      }
      else if (f >= 5) {
        d2 += c; // Collect characters to a string
      }
    }

    /////////////////// Reads and converts data //////////////////
    Serial.print("D1 is:");
    Serial.println(d1);

    if (d1 == "3000") { //If the message is "3000", the OLED should display dispensing
      dispense_time = millis();
      dispensing();

    }
    else { //If not, the values should considered to be the current values of the weights
      e1 = d1.toInt(); //Convert received string to integer
      e2 = d2.toInt(); //Convert received string to integer


      ///////////////// Interpretation of I2C weight inputs /////////////////

      //Weight 1
      e1 = e1 - 1000;

      int mixingMass = e1;
      client.publish("Weight 1", String(e1).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.

      //Weight 2
      e2 = e2 - 2000;

      int ethanolMass = e2;
      client.publish("Weight 2", String(e2).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
    }
  }

  d = ""; //reset d
  d1 = "";
  d2 = "";

  /*

    This would have been recreated for the reamining 4 weights, had they been working...

  */

  /////////////////// Write function I2C /////////////////
  if (mixingStatus == 1) {
    for (int t = 0; t <= 4; t++) {
      Wire.beginTransmission(8); // begin with device address 8 //
      Wire.write(static_cast<char*>(recipe[t]));  // sends string //
      Wire.endTransmission();  // stop transmitting //
      mixingStatus = 0; // Sets mixingStatus to 0, to avoid spamming the Arduino with data
    }
  }


  if (mixing == 1 && flamePin == LOW) { // When the mixing has started, the OLED should change its display
    mixingScreen();

  }

  ///////////////////////// MQTT LOOP FUNCTION /////////////////////




  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  ////////////////// FLAME SENSOR AND OLED /////////////////////

  //Flame detection
  Flame = digitalRead(flamePin);

  //If Flame is ON, turn ledRED and write "Fire" on OLED
  if (Flame == HIGH)
  {
    flamesensor (); // When there's a fire before the sensor, the OLED should change to warn the users
  }
  else {
    digitalWrite(ledRED, LOW);
  }
}
