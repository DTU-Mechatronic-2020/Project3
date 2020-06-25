// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the final project of the course, Project 3 - Corona (Final project)
#include <Arduino.h>
#include <Wire.h>
#include "HX711.h"

//1 KG weight, alcohol-weight
#define calibration_factor1 -1940 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define zero_factor1 -1098839 //This large value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT1 3 // pin3
#define CLK1 2 // pin2

HX711 scale1;

//3 KG weight, mixing chamber
#define calibration_factor2 -1015 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define zero_factor2 -598509 //This large value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT2 5 // pin5
#define CLK2 4 // pin4

HX711 scale2;

//SONAR
const int trigPin = A0;
const int echoPin = A1;
float duration, distance;

//
int ethanolPump = 13;
int waterPump = 12;
int glycerinPump = 11;
int brintoveriltePump = 10;
int oilPump = 9;
int dispensePump = 8;
int mixingMotor = 7;

int t = -1;

//Weight sensor input. This float sends the weight sensor value from this Arduino-code to NodeMCU
float w = 430;
char iw1[3];
char iw2[3];

static int w1;
static int w2;

String send_w1s;
String send_w2s;

char send_w1[5];
char send_w2[5];

char* send_w[2] = {send_w1, send_w2};

//Initialisations in order to read ingredient-refill from NodeMCU.
String d = "";
char c;
int e;

//Initialisations for recipe received over I2C
static int rcp;
static int ethanolrcp;
static int waterrcp;
static int glycerinrcp;
static int brintoveriltercp;
static int oilrcp;

//  static int recipe[5] = {ethanolrcp, waterrcp, glycerinrcp, brintoveriltercp, oilrcp};

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
char *ingredients[5] = {"Ethanol: ", "Wather: ", "Glycerin: ", "Hydrogen peroxide: ", "Oil: "};
int pumps[5] = {ethanolPump, waterPump, glycerinPump, brintoveriltePump, oilPump};
// Initialisation of mixing status
bool mixingStatus = false;

//////////////// Initialisation done ///////////////////

void setup() {
  Wire.begin(8);                /* join i2c bus with address 8 */
  Serial.begin(115200);           /* start serial for debug */



  //PUMPER
  pinMode(ethanolPump, OUTPUT);
  pinMode(waterPump, OUTPUT);
  pinMode(glycerinPump, OUTPUT);
  pinMode(brintoveriltePump, OUTPUT);
  pinMode(oilPump, OUTPUT);
  pinMode(dispensePump, OUTPUT);
  pinMode(mixingMotor, OUTPUT);

  //Sonar
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Weight sensor 1
  scale1.begin(DOUT1, CLK1);
  scale1.set_scale(calibration_factor1); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale1.set_offset(zero_factor1); //Zero out the scale using a previously known zero_factor

  //Weight sensor 2
  scale2.begin(DOUT2, CLK2);
  scale2.set_scale(calibration_factor2); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale2.set_offset(zero_factor2); //Zero out the scale using a previously known zero_factor

}
////////// setup done //////////

///////// Receive event //////////
void receiveEvent(int howMany) {
  while (0 < Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    d += c;     /* appends characters to string */
  }

  rcp = d.toInt();
  d = "";
  
  if (2000 > rcp && rcp >= 1000) {

    ethanolrcp = rcp - 1000;
    ethanolrcpS = String(ethanolrcp);
    ethanolrcpS.toCharArray(pumpe1, 5);
  }

  else if (3000 > rcp && rcp >= 2000) {
    waterrcp = rcp - 2000;
    waterrcpS = String(waterrcp);
    waterrcpS.toCharArray(pumpe2, 5);
    //Serial.print("Vand er: ");
    //Serial.println(waterrcp);
  }

  else if (4000 > rcp && rcp >= 3000) {
    glycerinrcp = rcp - 3000;
    glycerinrcpS = String(glycerinrcp);
    glycerinrcpS.toCharArray(pumpe3, 5);
  }

  else if (5000 > rcp && rcp >= 4000) {
    brintoveriltercp = rcp - 4000;

    brintoveriltercpS = String(brintoveriltercp);
    brintoveriltercpS.toCharArray(pumpe4, 5);
  }

  else if (6000 > rcp && rcp >= 5000) {
    oilrcp = rcp - 5000;
    oilrcpS = String(oilrcp);
    oilrcpS.toCharArray(pumpe5, 5);

    mixingStatus = true;
  }
}

///////// Request event //////////
void requestEvent() {
  int count = 0;
  if (distance < 15) {
    //  Serial.print("DISPENSE");
    // Serial.println();
    char dispense[5] = {"3000"};
    Wire.write(dispense);  //send string on request

  } else {
    send_w1s = String(w1);
    send_w1s.toCharArray(send_w1, 5);

    send_w2s = String(w2);
    send_w2s.toCharArray(send_w2, 5);

    Wire.write(send_w1);
    delay(100);
    Serial.print("vægt 1: ");
    Serial.println(send_w1);

    Wire.write(send_w2);
    delay(100);
    Serial.print("vægt 2: ");
    Serial.println(send_w2);
  }
}

void loop() {
///////// SONAR MEASUREMENT //////////
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * .0343) / 2;
  delay(100);



///////// SERIAL PRINT RECIPE /////////
  Serial.println();
  Serial.print("Recipe is: ");
  Serial.println();

  for (int t = 0; t < 5; t++) {
    Serial.print(ingredients[t]);
    Serial.println(recipe[t]);
  }

////////// PUMP CONTROL ////////////

  digitalWrite(ethanolPump, LOW);
  digitalWrite(waterPump, LOW);
  digitalWrite(glycerinPump, LOW);
  digitalWrite(brintoveriltePump, LOW);
  digitalWrite(oilPump, LOW);
  digitalWrite(dispensePump, LOW);
  digitalWrite(mixingMotor, LOW);

  if (mixingStatus == true) {
    float weightStart = scale2.get_units();
    float weightNow = scale2.get_units();
    int i = 0;

    while (true) {
      digitalWrite(pumps[i], HIGH);
      if (scale2.get_units() - weightStart > String(recipe[i]).toInt()) {
        digitalWrite(pumps[i], LOW);
        weightStart = scale2.get_units();
        i++;
        if (i == 4) {
          digitalWrite (mixingMotor, HIGH);
          delay(5000);
          digitalWrite(mixingMotor, LOW);
          break;
        }
      }
    }
    mixingStatus = false;
  }

  //////// DISPENSING FUNCTION //////////

  if (distance < 10) {
    digitalWrite(dispensePump, HIGH);
    delay(15000);
    digitalWrite(dispensePump, LOW);
    delay(1000);
  }

///////// Send weight input to MCU ///////////
  w1 = scale1.get_units(1);
  w1 = w1 + 1000;
  w2 = scale2.get_units(1);
  w2 = w2 + 2000;

  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
}
