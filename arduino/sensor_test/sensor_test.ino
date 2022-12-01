// Program for sending data from arduino nano and sensor to raspberry pi
// Part of skripsi by Rian Wardana
// I2C version

#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

#define LED_PIN A7
#define DHTPIN 7
#define DHTTYPE DHT22
#define TCAADDR 0x70
// #define MPUSELECT 2

DHT dht(DHTPIN, DHTTYPE);

bool blinkState = false;
unsigned long timer = 0;
bool isPulled = false;
String globalData;

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

    Serial.println(F("Done."));
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
  return data;
}

void tcaselect(uint8_t num)
{
    if (num > 7) return;

    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << num);
    Wire.endTransmission();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Begin setup");
    dht_setup();
    tcaselect(0);
    mpu_setup();
    
    pinMode(LED_PIN, OUTPUT); // setup LED pin for output
}

void loop()
{
    tcaselect(0);
    mpu.update();

    if (millis() - timer > 2000) // update led every 2 sec
    {
        globalData = getData();
        Serial.println(globalData);
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState); // turn the LED off by making the voltage LOW
        timer = millis();
    }
}
