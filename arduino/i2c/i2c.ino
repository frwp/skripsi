// Program for sending data from arduino nano and sensor to raspberry pi
// Part of skripsi by Rian Wardana
// I2C version

#include <DHT.h>
#include <Wire.h>
#include <MPU6050.h>
#include <stdlib.h>

#define LED_PIN A1
#define DHTPIN 7
#define DHTTYPE DHT22
#define SLAVE_ADDRESS 0x04
#define MAX_LENGTH 50

DHT dht(DHTPIN, DHTTYPE);
MPU6050 mpu;

char cdata[MAX_LENGTH];
unsigned long timer = 0;
bool blinkState = false;
volatile bool isPulled = false;
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

void sendMessage()
{
  isPulled = true;
  delay(10);
  // pinMode(SCL, INPUT);

  // on request send message
  Wire.write("TEST ");
  for (int i = 0; i < MAX_LENGTH; i++)
  {
    Wire.write(cdata[i]); // send message to master
  }
  Serial.print(F("Data is: "));
  Serial.println(cdata);

  isPulled = false;
  delay(10);
  // pinMode(SCL, OUTPUT);
}

void setup()
{
  Serial.begin(115200);

  Wire.begin(SLAVE_ADDRESS);
  // Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  Serial.println("Begin setup");
  dht_setup();
  Serial.println(F("Done dht setting."));
  mpu_setup();
  Serial.println(F("Done MPU setting."));

  Wire.onRequest(sendMessage); // call sendMessage() on request by master
  pinMode(LED_PIN, OUTPUT);    // setup LED pin for output
}

void loop()
{
  if (!isPulled && (millis() - timer > 2000)) // update led every sec
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
    snprintf(cdata, MAX_LENGTH, "|h=%s|t=%s|x=%s,y=%s,z=%s|", hum_c, temp_c, acc0_c, acc1_c, acc2_c);
    // Serial.println(cdata);

    // blinks to indicate program is running
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    timer = millis();
  }
}
