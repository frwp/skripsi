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
#include <Wire.h>
#include <MPU6050.h>
#include <SPI.h>
#include <stdlib.h>

// define constants
#define LED_PIN PIN_A1
#define DHTPIN PIN7
#define DHTTYPE DHT22
#define SLAVE_ADDRESS 0x04
#define SS PIN_SPI_SS
#define MOSI PIN_SPI_MOSI
#define MISO PIN_SPI_MISO
#define SCLK PIN_SPI_SCK
#define MAX_LENGTH 50

// init DHT22 and MPU6050 object here
DHT dht(DHTPIN, DHTTYPE);
MPU6050 mpu;

// define vars
char cdata[MAX_LENGTH];
unsigned long timer = 0;
bool blinkState = false;
volatile byte pos;
volatile bool processed;
float hum, temp;
char hum_c[7], temp_c[7], acc0_c[7], acc1_c[7], acc2_c[7];

void mpu_setup()
{
  // Initialize device
  Serial.println(F("Initialize MPU6050."));

  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println(F("Could not find a valid MPU6050 sensor, check wiring"));
    delay(500);
  }

  checkSettings();
}

void checkSettings()
{
  Serial.println();

  Serial.print(" * Sleep Mode:            ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Clock Source:          ");
  switch (mpu.getClockSource())
  {
  case MPU6050_CLOCK_KEEP_RESET:
    Serial.println("Stops the clock and keeps the timing generator in reset");
    break;
  case MPU6050_CLOCK_EXTERNAL_19MHZ:
    Serial.println("PLL with external 19.2MHz reference");
    break;
  case MPU6050_CLOCK_EXTERNAL_32KHZ:
    Serial.println("PLL with external 32.768kHz reference");
    break;
  case MPU6050_CLOCK_PLL_ZGYRO:
    Serial.println("PLL with Z axis gyroscope reference");
    break;
  case MPU6050_CLOCK_PLL_YGYRO:
    Serial.println("PLL with Y axis gyroscope reference");
    break;
  case MPU6050_CLOCK_PLL_XGYRO:
    Serial.println("PLL with X axis gyroscope reference");
    break;
  case MPU6050_CLOCK_INTERNAL_8MHZ:
    Serial.println("Internal 8MHz oscillator");
    break;
  }

  Serial.print(" * Accelerometer:         ");
  switch (mpu.getRange())
  {
  case MPU6050_RANGE_16G:
    Serial.println("+/- 16 g");
    break;
  case MPU6050_RANGE_8G:
    Serial.println("+/- 8 g");
    break;
  case MPU6050_RANGE_4G:
    Serial.println("+/- 4 g");
    break;
  case MPU6050_RANGE_2G:
    Serial.println("+/- 2 g");
    break;
  }

  Serial.print(" * Accelerometer offsets: ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());

  Serial.println();
}

void dht_setup()
{
  dht.begin();
}

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  Serial.println("Begin setup");
  dht_setup();
  Serial.println(F("Done dht setting."));
  mpu_setup();
  Serial.println(F("Done MPU setting."));

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
ISR(SPI_STC_vect)
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
  if (!processed && (millis() - timer > 2000)) // update led every sec
  {
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    Vector normAccel = mpu.readNormalizeAccel();

    dtostrf(hum, 2, 2, hum_c);
    dtostrf(temp, 2, 2, temp_c);
    dtostrf(normAccel.XAxis, 2, 2, acc0_c);
    dtostrf(normAccel.YAxis, 2, 2, acc1_c);
    dtostrf(normAccel.ZAxis, 2, 2, acc2_c);
    // sprintf(cdata, "|h=%s|t=%s|x=%s,y=%s,z=%s|", chum, ctemp, cacc0, cacc1, cacc2);
    snprintf(cdata, 50, "|h=%s|t=%s|x=%s,y=%s,z=%s|", hum_c, temp_c, acc0_c, acc1_c, acc2_c);
    Serial.println(cdata);

    // blinks to indicate program is running
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    timer = millis();
  }
}
