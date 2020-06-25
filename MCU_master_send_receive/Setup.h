// This code has been made for the course Mechatronics engineering design F20. 
// The code has been made by Group 8 for the second project of the course, Project 3 - IoT  

// Defining pins, variables and more. 



/////////////////////////////////////// WiFi access codes. ///////////////////////////////////////
const char* ssid = "NSA 1"; // HINT: create a hotspot with your phone..
const char* password = "pipper444";



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
int mixingstatus;
int dispensingstatus;
int fillweight;
/////////////////////////////////////// Pin Setup ///////////////////////////////////////


/////////////////////////////////////// Millis Setup ///////////////////////////////////////
int tenseconds = 10000;
unsigned long time_now = 0;


/////////////////////////////////////// WiFi og OLED definationer ///////////////////////////////////////

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
