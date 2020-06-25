// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the second project of the course, Project 2 - IoT


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
#include <DFRobot_HX711.h>
#include <LiquidCrystal.h>

/////// FUNKTIONSOPSÆTNING ////////


// Opretter en placeholder for callback-funktionen til brug senere. Den rigtige funktion ses længere nede.
void callback(char* byteArraytopic, byte* byteArrayPayload, unsigned int length);

// Opretter en klient der kan forbinde til en specifik internet IP adresse.
WiFiClient espClient; // Initialiserer wifi bibloteket ESP8266Wifi, som er inkluderet under "nødvendige bibloteker"

// Opretter forbindelse til mqtt klienten:
PubSubClient client(mqtt_server, mqtt_port, callback, espClient); // Initialiserer bibloteket for at kunne modtage og sende beskeder til mqtt. Den henter fra definerede mqtt server og port. Den henter fra topic og beskeden payload

/////// FUNKTIONSOPSÆTNING SLUT /////////

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
  // Konverterer den indkomne besked (payload) fra en array til en string:
  // Topic == Temperaturmaaler, Topic == Kraftsensor
  payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload


  /////////////////////////////// INGREDIENTS SETUP ///////////////////////////////


  ///////////////////////// ETHANOL CONFIGURATION /////////////////////////

  if (topic == "Ethanol") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
      // Det regnes med at beskeden er i et bestemt 5 char format således: 12:30
    }
    Serial.println(payload);
    client.publish("RecievedStartTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  ///////////////////////// WATER CONFIGURATION /////////////////////////

  if (topic == "Water") {
    // Nulstiller payload, endhour og endmin variablen så forloopet ikke appender til en allerede eksisterende definationer
    payload = "";
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    Serial.println(payload);
    client.publish("RecievedEndTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }


  ///////////////////////// GLYCERIN CONFIGURATION /////////////////////////


  // Giver clienten en mulighed for at ændre bookingstatus. Tænkes at skal bruges når en booking anulleres.
  if (topic == "Glycerin") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    Serial.print(payload);
    client.publish("RecievedEndTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }


  ///////////////////////// H2O2 CONFIGURATION /////////////////////////

  // Giver clienten en mulighed for at ændre bookingstatus. Tænkes at skal bruges når en booking anulleres.
  if (topic == "Brintoverilte") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    Serial.print(payload);
    client.publish("RecievedEndTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  ///////////////////////// CHOOSING MACHINE /////////////////////////

  // Gets the number og the cabinet : I tilfælde af at der er flere skabe, så kan skabet nr. defineres.
  if (topic == "Machinenr") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    String Machinenr = payload; //
    Serial.print(payload);
    client.publish("LokcerNumber", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  ///////////////////////// MIXING CONFIGURATION /////////////////////////

  if (topic == "Mix") {
    // Nulstiller payload, endhour og endmin variablen så forloopet ikke appender til en allerede eksisterende definationer
    payload = "";
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    Serial.println(payload);
    if (payload == "ON") {
      mixingstatus = HIGH;
    }
    else if (payload == "OFF") {
      mixingstatus = LOW;
    }


    client.publish("RecievedEndTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  ///////////////////////// DISPENSE CONFIGURATION /////////////////////////

  if (topic == "Dispense") {
    // Nulstiller payload, endhour og endmin variablen så forloopet ikke appender til en allerede eksisterende definationer
    payload = "";
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }
    if (payload == "ON") {
      dispensingstatus = HIGH;
    }
    else if (payload == "OFF") {
      dispensingstatus = LOW;
    }
    Serial.println(payload);
    client.publish("RecievedEndTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }





}

///////// CALLBACK SLUT /////////




void setup() {
  Serial.begin(115200);
  u8g2.begin(); //Tænder skærmen

  //connect to the wifi access point
  setup_wifi(); // Kører WiFi loopet og forbinder herved.
  client.setServer(mqtt_server, mqtt_port); // Forbinder til mqtt serveren (defineret længere oppe)
  client.setCallback(callback); // Ingangsætter den definerede callback funktion hver gang der er en ny besked på den subscribede "cmd"- topic
  pinMode(GreenLedPin, OUTPUT);
  digitalWrite(GreenLedPin, LOW);
  pinMode(RedLedPin, OUTPUT);
  digitalWrite(RedLedPin, LOW);
  pinMode(mixingPin, OUTPUT);
  mixingstatus = LOW;
  dispensingstatus = LOW;
}

void loop() {
  digitalWrite(mixingmotor, mixingstatus);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
