const int motor1 = 11; //LED connected to digital pin 10
const int motor2 = 10;
const int motor3 = 9;


void setup()
{
  Serial.begin(9600);
  pinMode(motor1, OUTPUT); //sets the digital pin as output
  pinMode(motor2, OUTPUT); //sets the digital pin as output
  pinMode(motor3, OUTPUT); //sets the digital pin as output
}

void loop()
{
  digitalWrite(motor1, HIGH); //turns the LED on
  delay(5000);
  digitalWrite(motor1, LOW);
  
  digitalWrite(motor2, HIGH); //turns the LED on
  delay(5000);
  digitalWrite(motor2, LOW);
  
  digitalWrite(motor3, HIGH); //turns the LED on
  delay(5000);
  digitalWrite(motor3, LOW);

  digitalWrite(motor1, HIGH); //turns the LED on
  digitalWrite(motor2, HIGH); //turns the LED on
  digitalWrite(motor3, HIGH); //turns the LED on
  delay(3000);


  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(motor3, LOW);

  
  delay(5000);
}
