#include <SPI.h>
#define SS PIN_SPI_SS
#define MOSI PIN_SPI_MOSI
#define MISO PIN_SPI_MISO
#define SCLK PIN_SPI_SCK
#define LED PIN_A1

/**
 * Quick reference
 * SPCR = SPI Control Register, consists of 8 bit value, each controlling SPI functionality
 * |  ---                       SPCR                          --- |
 * | SPIE | SPE   | DORD  | MSTR  | CPOL  | CPHA  | SPR1  | SPR0  |
 * |  7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
 * 
 * SPIE = enables SPI interrupt when 1
 * SPE  = enables SPE when 1
 * DORD = Sends data least Significant Bit First when 1, most Significant Bit first when 0
 * MSTR = Sets the Arduino in controller mode when 1, peripheral mode when 0
 * CPOL = Sets the data clock to be idle when high if set to 1, idle when low if set to 0
 * CPHA = Samples data on the falling edge of the data clock when 1, rising edge when 0
 * SPR1 and SPR0 - Sets the SPI speed, 00 is fastest (4MHz) 11 is slowest (250KHz)
*/
String data;
char cdata[10];
volatile byte pos;
volatile bool processed;
bool blinkState = false;

void setup() {
  // setup serial communication for debug
  Serial.begin(115200);

  // unlike in master mode when we could just call
  // SPI.beginTransaction() ,
  // set SPI to slave mode by enabling SPE bit
  SPCR |= (1 << SPE);
  SPI.attachInterrupt();

  // code above resulting in SPCR = 01000000
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCLK, INPUT);
  pinMode(SS, INPUT);
  pinMode(LED, OUTPUT);

  pos = 0;
  processed = false;
  Serial.println(F("Start."));
}

// SPI interrupt routine
// called every SPI transfer cycle.
ISR (SPI_STC_vect) {
  byte b = SPDR;
  if (b == 0x10) {
    pos = 0;
    processed = true;
  }
  
  if (pos < 8)
  {
    SPDR = cdata[pos++];
  }
  else
  {
    processed = false;
  }
  // SPDR = b + 2;
}

void loop() {
  data = "tester ";
  if (!processed)
    data.toCharArray(cdata, data.length() + 1);
  // Serial.println(cdata);
  
  delay(500);
  digitalWrite(LED, blinkState);
  blinkState = !blinkState;
}
