// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the final project of the course, Project 3 - Corona (Final project)

// Defining pins, variables and more.



/////////////////////////////////////// WiFi access codes. ///////////////////////////////////////
const char* ssid = "Bogfinkevej 8 st.th."; // HINT: create a hotspot with your phone..
const char* password = "emmaogchristian";



/////////////////////////////////////// Definerer information til mqtt serveren ///////////////////////////////////////
const char *mqtt_server  = "hairdresser.cloudmqtt.com"; //navn på mqtt-server. Find navnet på cloudmqtt-hjemmesiden
const int mqtt_port  = 15743 ; // Definerer porten
const char *mqtt_user  = "Freddy"; // Definerer mqtt-brugeren
const char *mqtt_pass  = "Freddy" ; // Definerer koden til mqtt-brugeren



/////////////////////////////////////// For setting time and date ///////////////////////////////////////
const long utcOffsetInSeconds = 7200; //For CEST, UTC +2 +2.00: 1*60*60
char daysOfTheWeek[7][12] = {"Sunday" , "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; //Week days

/////////////////////////////////////// Variabler der bliver brugt igennem koden ///////////////////////////////////////
String payload; // Definerer variablen 'payload' i det globale scope (payload er navnet på besked-variablen)


// values needed to read weight sensors
char c;
int e1;
int e2;
String d;
String d1;
String d2;

String Machinenr ="1";

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
static int mixing = 0;
static int mixingStatus = 0;




int Flame = LOW;



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
/////////////////////////////////////// Pin Setup ///////////////////////////////////////
//FLAME SENSOR
const int flamePin = D8;
//LED'er
int ledRED = D6;
int ledGREEN = D7;

/////////////////////////////////////// Millis Setup ///////////////////////////////////////
unsigned long time_now = 0;
unsigned long dispense_time = 0;
unsigned long update_time = 0;


/////////////////////////////////////// WiFi og OLED definationer ///////////////////////////////////////

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, D1, D2, U8X8_PIN_NONE);
