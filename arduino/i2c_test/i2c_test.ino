#include "Wire.h"


#define INTERRUPT_PIN 2 // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13      // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;


void sendMessage()
{
  // on request send message
  Wire.write("test "); // send 5 byte message to master
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(4);
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

  pinMode(INTERRUPT_PIN, INPUT);

  Wire.onRequest(sendMessage);  // call sendMessage() on request by master
  pinMode(LED_BUILTIN, OUTPUT); // setup LED pin for output
}

void loop()
{
  blinkState = !blinkState;
  digitalWrite(LED_BUILTIN, blinkState); // turn the LED off by making the voltage LOW
  delay(2000);
}