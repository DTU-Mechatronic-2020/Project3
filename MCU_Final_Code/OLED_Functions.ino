// This code has been made for the course Mechatronics engineering design F20.
// The code has been made by Group 8 for the final project of the course, Project 3 - Corona (Final project)

// Function that controls and changes the OLED screen, as well as uses an weather API. This screen will be the default screen.
void OLEDStandard() {
  // Most of this code is reused from our project 2.
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http; //Declare an object of class HTTPClient

    // Får vejrdata fra en vejr-api. Her er vejret fra københavn.
    http.begin("http://api.openweathermap.org/data/2.5/forecast?q=Copenhagen,dk&appid=95fb8a0ca887dc68309f34e8d20c8ceb&units=metric&cnt=3"); //Specify request destination
    int httpCode = http.GET(); //Send the request

    // Hvis der er data modtaget, så kører koden.
    if (httpCode > 0) { //Check the returning code

      String payload = http.getString(); //Get the request response payload - the json object as a plain text

      //define a json object buffer
      DynamicJsonDocument jsonBuffer(1024);

      //try convert the plain text to an arduino - json object
      DeserializationError success = deserializeJson(jsonBuffer, payload);

      // if conversion not successful:
      // NOTE: there are two cases this can fail: if the json object is too big for the arduino
      // OR the communication protocol to the api is secure: e.g. 'https' instead of 'http'
      // There are solutions to both, but not relevant for this exercise..

      if (!success) {
        Serial.println("deserialization failed");
        delay(1000);
        return;
      }

      // the first forecast in the json object is at jsonBuffer['list'][0]
      String timestamp = jsonBuffer["list"][0]["dt_txt"];

      // The 'weather' in the first position in the list, is actually a list, with only one element...
      String desc = jsonBuffer["list"][0]["weather"][0]["description"];


      float temp = jsonBuffer["list"][0]["main"]["temp"]; //Tager temperaturen fra vejr APIen
      timeClient.update(); //Opdaterer tiden.
      u8g2.clearBuffer();          // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      u8g2.setCursor(0, 10);
      u8g2.printf("%02d", timeClient.getHours()); //Makes sure that there's always being printed with two dicimals.
      u8g2.print(":");
      u8g2.printf("%02d", timeClient.getMinutes()); //Makes sure that there's always being printed with two dicimals.
      u8g2.setCursor(70, 10);
      u8g2.print(daysOfTheWeek[timeClient.getDay()]);
      u8g2.setCursor(40, 20);
      u8g2.print(desc);
      u8g2.setCursor(0, 20);
      u8g2.print(String(temp));
      u8g2.print("C");
      u8g2.setCursor(0, 30);
      u8g2.print("Maskine nr: ");
      u8g2.print(Machinenr);
      u8g2.setCursor(0, 50);
      u8g2.print("Ready to dispense");
      u8g2.sendBuffer();          // transfer internal memory to the display
    }
    http.end(); //Close connection
  }
}


void dispensing () { //When the machine is dispensing to the user, the screen should change to "dispensing" and pause the script while the pump is dispensing.

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_crox5tb_tr);
  u8g2.setCursor(0, 40);
  u8g2.print("Dispensing...");
  u8g2.sendBuffer();
  digitalWrite(ledGREEN, HIGH);
  delay(3000);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_crox5tb_tr);
  u8g2.setCursor(0, 40);
  u8g2.print("Done!");
  u8g2.sendBuffer();
  digitalWrite(ledGREEN, LOW);
  delay(800);
  u8g2.clearBuffer();


}


void flamesensor () { //In a case of fire, the red LED should turn on and the OLED should display a warning message.
  digitalWrite(ledRED, HIGH);
  digitalWrite(ledGREEN, LOW);
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_crox5tb_tr); // choose a suitable font
  u8g2.drawStr(0, 30, "!!DANGER!!"); // write something to the internal memory
  u8g2.drawStr(0, 50, "!!!FIRE!!!"); // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display



}


void mixingScreen() { // When the mixingprocess is undertaking, the OLED should communicate this to the users.
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_crox5tb_tr);
  u8g2.setCursor(0, 30);
  u8g2.print("Mixing...");
  u8g2.setCursor(0, 50);
  u8g2.print("Please wait");
  u8g2.sendBuffer();

}
