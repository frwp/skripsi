
// Program for sending data from arduino nano and sensor to raspberry pi
// Part of skripsi by Rian Wardana
// SPI version

/**
 * Quick reference
 * SPCR = SPI Control Register, consists of 8 bit value, each controlling SPI functionality
 * |  ------------------------- SPCR ---------------------------- |
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

#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <SPI.h>

// define constants
#define LED_PIN PIN_A1
#define DHTPIN PIN7
#define DHTTYPE DHT22
#define SLAVE_ADDRESS 0x04
#define SS PIN_SPI_SS
#define MOSI PIN_SPI_MOSI
#define MISO PIN_SPI_MISO
#define SCLK PIN_SPI_SCK
#define MAX_LENGTH 42

// init DHT22 and MPU6050 object here
DHT dht(DHTPIN, DHTTYPE);
MPU6050 mpu(Wire);

// define vars
String globalData;
char cdata[MAX_LENGTH];
unsigned long timer = 0;
bool blinkState = false;
volatile byte pos;
volatile bool processed;

void mpu_setup()
{
  // Initialize device
  byte status = mpu.begin();
  Serial.println(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) { }

  Serial.println(F("Calculating offsets, do not move MPU6050..."));

  // wait for ready
  delay(1000);

  mpu.calcOffsets(false, true);

  Serial.println(F("Done MPU setting."));
}

void dht_setup()
{
  dht.begin();
  Serial.println(F("Done dht setting."));
}

String getData()
{
  float hum = dht.readHumidity(); 
  float temp = dht.readTemperature();

  float acc[3] = {
    mpu.getAccX(),
    mpu.getAccY(),
    mpu.getAccZ()
  };
  String data;
  data = "|h=" + String(hum) + "|t=" + String(temp) + "|x=" + String(acc[0]) + ",y=" + String(acc[1]) + ",z=" + String(acc[2]) + "|";
  // data = "|h=" + String(hum) + "|t=" + String(temp);
  return data;
}

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  Serial.println("Begin setup");
  dht_setup();
  mpu_setup();

  // these results in SPCR = 0b01000000
  SPCR |= (1 << SPE);
  SPI.attachInterrupt();

  // set pin mode in slave mode
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCLK, INPUT);
  pinMode(SS, INPUT);

  pinMode(LED_PIN, OUTPUT); // setup LED pin for output

  pos = 0;
  processed = false;
  Serial.println(F("Starting program."));  
}

// SPI interrupt routine
// called every SPI transfer cycle.
ISR (SPI_STC_vect)
{
  byte b = SPDR;

  // First start byte received from master. 
  // This indicates the begining of SPI transaction.
  if (b == 0x10)
  {
    pos = 0;
    processed = true;
  }

  if (pos < MAX_LENGTH)
  {
    SPDR = cdata[pos++];
  }
  else
  {
    processed = false;
  }
}

void loop()
{
  // pull data from sensor when not processing SPI call
  if (!processed)
  {
    mpu.update();

    if (millis() - timer > 2000) // update led every 2 sec
    {
      globalData = getData();
      globalData.toCharArray(cdata, globalData.length() + 1);

      // blinks to indicate program is running
      blinkState = !blinkState;
      digitalWrite(LED_PIN, blinkState);
      Serial.println(globalData);
      timer = millis();
    }
  }
}
