/* A Personal Space Detector Machine built with Arduino Uno.
 *  Hardware Required: Arduino Uno, Keypad, LCD i2c, Ultrasonic Sensor, Servo Motor, Breadboard, 2 LEDs, 18 wires
 *  @Author Marques Reimann
 */
#include <TimerOne.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define trigger 11
#define echo 12
/**********************************************************/
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
Servo servo;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B' },
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String str = " Key: _                    "; //the string to print on the LCD
char key;
int tim = 200; //the value of delay time
int RED = 13; //red light
int tooClose = 100, // the distance (in cm) that is "too close" to the personal space machine
      distance=0, // distance (in cm) that the Sensor is from an object
      m=0,
      x=0;
float duration=0;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27

/*********************************************************/
void setup()
{
  Serial.begin(115200);
  
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight
  servo.attach(10);
  
  pinMode(trigger,OUTPUT);
  pinMode(echo, INPUT);
  pinMode(RED, OUTPUT);

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(showMinute);
}
/*********************************************************/
void loop()
{
  handleKey();

  distance = getDistance(); // (in cm)
  Serial.println(distance); 

  if(distance > tooClose)
    digitalWrite(RED, LOW);
  else{
    digitalWrite(RED, HIGH);
    servo.write((servo.read()+15)%180);
  }

  displayMessage();
  delay(tim); 
  lcd.clear(); //Clears the LCD screen and positions the cursor in the upper - left corner.
}
/* A function for writing and reading an Ultrasonic Sensor pulse*/
static int getDistance(){
  digitalWrite(trigger,LOW); // Produce pulse at the trigger pin of the sensor device - Starts at LOW pulse
  delayMicroseconds(2);
  digitalWrite(trigger,HIGH); //Goes to HIGH pulse
  delayMicroseconds(10);
  digitalWrite(trigger,LOW); //Back to LOW pulse
  delayMicroseconds(2);
  duration = pulseIn(echo,HIGH); // duration has the round-trip time of transmitters ultrasound wave
  return (duration/2) * 0.0343; // one-way distance = half of round-trip * speed of sound
}
/* A function to get the key from keypad and put it into str */
static void handleKey(){
  key = keypad.getKey();
  if (key != NO_KEY) {
    tooClose= (int)key*10 - 480;
    Serial.println(tooClose);
  }
}
/* A function to display the necessary message to the LCD */
static void displayMessage(){
  lcd.setCursor(0, 0); // set the cursor to top left corner
  lcd.print("Too Close: " + (String)tooClose); // Print a message to the LCD.
  
  lcd.setCursor(0, 1); // set the cursor to bottom left corner
  lcd.print("Distance: " + (String)distance); // Print a message to the LCD.
}
/* a function to interrupt the program after 60 seconds */
void showMinute(){
  x++;
  if (x==10) {
      x=0;
      m++;
      Serial.print(m);
    }
}

