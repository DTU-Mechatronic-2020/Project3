
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

char c;
int e;
String d;
String Machinenr;


int ethanolrcp;
int waterrcp;
int glycerinrcp;
int brintoveriltercp;
int oilrcp;

int ethanolMass;
int waterMass;
int glycerinMass;
int brinoverilteMass;
int oilMass;

int mixingMass;
int mixingMassMin;

int recipe[5];
int distance;


//FLAME SENSOR
const int flamePin = D5;
int Flame = HIGH;

//LED'er

int ledRED = D6;
int ledGREEN = D7;


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

    int ethanolrcp = payload.toInt() + 1000; //Cryptation for I2C bus

    Serial.println(payload);
  }

  ///////////////////////// WATER CONFIGURATION /////////////////////////

  if (topic == "Water") {
    // Nulstiller payload, endhour og endmin variablen så forloopet ikke appender til en allerede eksisterende definationer
    payload = "";
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    int waterrcp = payload.toInt() + 2000; //Cryptation for I2C bus

    Serial.println(payload);
  }

  ///////////////////////// GLYCERIN CONFIGURATION /////////////////////////

  if (topic == "Glycerin") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    int glycerinrcp = payload.toInt() + 3000; //Cryptation for I2C bus

    Serial.print(payload);
  }

  ///////////////////////// H2O2 CONFIGURATION /////////////////////////

  if (topic == "Brintoverilte") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    int brintoveriltercp = payload.toInt() + 4000; //Cryptation for I2C bus

    Serial.print(payload);
  }

  ///////////////////////// Oil CONFIGURATION /////////////////////////

  if (topic == "Oil") {
    payload = ""; // Nulstil payload variablen så forloopet ikke appender til en allerede eksisterende payload
    for (int i = 0; i < length; i++) {
      payload += (char)byteArrayPayload[i];
    }

    int oilrcp = payload.toInt() + 5000; //Cryptation for I2C bus

    Serial.print(payload);
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
  }

  ///////////////////////// REFILL CONFIGURATION /////////////////////////

  if (topic == "Refill") {
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
    //client.publish("RecievedEndTime", String(payload).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  char ethanolrcpchar[3] = {ethanolrcp};
  char waterrcpchar[3] = {waterrcp};
  char glycerinrcpchar[3] = {glycerinrcp};
  char brintoveriltercpchar[3] = {brintoveriltercp};
  char oilrcpchar[3] = {oilrcp};



  int recipe[5] = {ethanolrcpchar[3], waterrcpchar[3], glycerinrcpchar[3], brintoveriltercpchar[3], oilrcpchar[3]};

}

///////// CALLBACK SLUT /////////



//Dummy inputmængde
float x = 3550;
char ix;

int len;



void setup() {
  Serial.begin(115200); /* begin serial for debug */
  Wire.begin(D3, D4); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  u8g2.begin();

  setup_wifi(); // Kører WiFi loopet og forbinder herved.
  client.setServer(mqtt_server, mqtt_port); // Forbinder til mqtt serveren (defineret længere oppe)

  client.setCallback(callback); // Ingangsætter den definerede callback funktion hver gang der er en ny besked på den subscribede "cmd"- topic
  Serial.println("MQTT_Server");


  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  u8g2.drawStr(0, 10, "Dispensing"); // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(5000);
  /*
    pinMode(GreenLedPin, OUTPUT);
    digitalWrite(GreenLedPin, LOW);
    pinMode(RedLedPin, OUTPUT);
    digitalWrite(RedLedPin, LOW);
    pinMode(mixingPin, OUTPUT);
  */

  bool mixingstatus = LOW;
  bool dispensingstatus = LOW;
  int mixingMassMin = 500;
  Serial.println("MixingMassMin");


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

  /////////////////// Read function //////////////////
  Wire.requestFrom(8, 3); /* request & read data of size 13 from slave */
  while (Wire.available()) {
    char c = Wire.read(); //Read charachters to c
    //Serial.print(c);

    ////// OLED control //////

    /*
      if (c == 0) {
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.drawStr(0, 10, "Dispensing"); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      delay(10);
      }

      if (c == 1) {
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.drawStr(0, 10, "FIRE guys, run!!"); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      delay(10);
      }

      if (c == 2) {

      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.drawStr(0, 10, ""); // write something to the internal memory
      u8g2.sendBuffer();          // transfer internal memory to the display
      delay(10);

      }
      else {
    */

    //// Read weightsensors ////

    d += c; //Collect characters to string
    int e = d.toInt(); //Convert received string to integer
    Serial.print(e);
    d = ""; //reset d
    Serial.println();
    delay(1000);
  }



  ///////////////// Interpretation of I2C weight inputs /////////////////

  //Weight 1
  if (2000 >= e && e > 1000) {
    e = e - 1000;
    int mixingMass = e;
    client.publish("Weight 1", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 2
  else if (3000 >= e && e > 2000) {
    e = e - 2000;
    int ethanolMass = e;
    client.publish("Weight 2", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 3
  else if (4000 >= e && e > 3000) {
    e = (e - 3000);
    int waterMass = e;
    client.publish("Weight 3", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 4
  else if (5000 >= e && e > 4000) {
    e = e - 4000;
    int glycerinMass = e;
    client.publish("Weight 4", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 5
  else if (5000 >= e && e > 4000) {
    e = e - 4000;
    int H2O2Mass = e;
    client.publish("Weight 5", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  //Weight 6
  else if (6000 >= e && e > 5000) {
    e = e - 5000;
    int oilMass = e;
    client.publish("Weight 6", String(e).c_str()); // Publish besked fra MCU til et valgt topic. Husk at subscribe til topic'et i NodeRed.
  }

  int e = 0;

  /////////////////// Write function /////////////////

  // dtostrf( value, samlede ant. tegn, ant. tegn efter komma, char)
  if (mixingstatus == HIGH && mixingMass < mixingMassMin) {
    for (int t = 0; t <= 4; t++) {
      //char irecipe [4];
      // char *message = &irecipe[0];

      //dtostrf(recipe[t], 4, 0, irecipe[t]);
      Wire.beginTransmission(8); /* begin with device address 8 */
      Wire.write(static_cast<byte>(recipe[t]));  /* sends string */
      Wire.endTransmission();  /* stop transmitting */
    }

  }


  //Flame detection
  Flame = digitalRead(flamePin);

  //If Flame is ON, turn ledRED and write "Fire" on OLED
  if (Flame == HIGH)
  {
    digitalWrite(ledRED, HIGH);
    digitalWrite(ledGREEN, LOW);

    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
    u8g2.drawStr(0, 10, "FIRE guys, run!!"); // write something to the internal memory
    u8g2.sendBuffer();          // transfer internal memory to the display
    delay(10);
  }

  //When dispensing
  else if (distance < 15)
  {
    digitalWrite(ledRED, LOW);
    digitalWrite(ledGREEN, HIGH);

    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
    u8g2.drawStr(0, 10, "Dispensing"); // write something to the internal memory
    u8g2.sendBuffer();          // transfer internal memory to the display
    delay(10);
  }

  // If there is no fire and no dispensing, write nothing on the OLED and turn OFF the LED's
  else
  {
    digitalWrite(ledRED, LOW);
    digitalWrite(ledGREEN, LOW);
    u8g2.clearBuffer();          // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
    u8g2.drawStr(0, 10, ""); // write something to the internal memory
    u8g2.sendBuffer();          // transfer internal memory to the display
    delay(10);

  }
}
