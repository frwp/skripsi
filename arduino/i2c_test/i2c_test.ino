// Program for sending data from arduino nano and sensor to raspberry pi
// Part of skripsi by Rian Wardana
// I2C version

#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin(4);                  // Address #0x04
  Wire.onRequest(sendMessage);    // call sendMessage() on request by master
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
}

void sendMessage() {
  // on request send message
  Wire.write("test ");      // send 5 byte message to master
}