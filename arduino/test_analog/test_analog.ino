#include <DHT.h>
#include <Wire.h>
#include <stdlib.h>

#define LED_PIN A1
#define DHTPIN 7
#define DHTTYPE DHT22
#define SLAVE_ADDRESS 0x04
#define MAX_LENGTH 32
#define xInput A2
#define yInput A3
#define zInput A6

DHT dht(DHTPIN, DHTTYPE);

const int sampleSize = 10;
const int RawMin = 0;
const int RawMax = 1023;

char cdata[MAX_LENGTH];
unsigned long timer = 0;
bool blinkState = false;
volatile bool isPulled = false;
float hum, temp;
char hum_c[7], temp_c[7], acc0_c[7], acc1_c[7], acc2_c[7];
int xOffset = 0, yOffset = 0, zOffset = 0;

void dht_setup()
{
    dht.begin();
}

int readAxis(int axisPin)
{
    long reading = 0;
    analogRead(axisPin);
    delay(1);
    for (int i = 0; i < sampleSize; i++)
    {
        reading += analogRead(axisPin);
    }
    return reading / sampleSize;
}

float readX()
{
    int xRaw = readAxis(xInput) + xOffset;
    long xScaled = map(xRaw, RawMin, RawMax, -3000, 3000);
    return xScaled / 1000.0;
}
float readY()
{
    int yRaw = readAxis(yInput) + yOffset;
    long yScaled = map(yRaw, RawMin, RawMax, -3000, 3000);
    return yScaled / 1000.0;
}
float readZ()
{
    int zRaw = readAxis(zInput) + zOffset;
    long zScaled = map(zRaw, RawMin, RawMax, -3000, 3000);
    return zScaled / 1000.0;
}

void calibrate_gy61()
{
    int zeroG = 512; // analog input is 0-1023, 512 is the median.
    long xVal = 0;
    long yVal = 0;
    long zVal = 0;
    int counter = 0;
    while (counter < 100)
    {
        xVal += readAxis(xInput);
        yVal += readAxis(yInput);
        zVal += readAxis(zInput);
        counter++;
    }
    xOffset = zeroG - xVal / 100;
    yOffset = zeroG - yVal / 100;
    zOffset = zeroG - zVal / 100;
    Serial.println(F("Offset is: "));
    Serial.print(F("xOffset:\t"));
    Serial.println(xOffset);
    Serial.print(F("yOffset:\t"));
    Serial.println(yOffset);
    Serial.print(F("zOffset:\t"));
    Serial.println(zOffset);
}

void sendMessage()
{
    isPulled = true;
    delay(10);

    for (int i = 0; i < MAX_LENGTH; i++)
    {
        Wire.write(cdata[i]);
    }
    // Wire.write(cdata, MAX_LENGTH);
    Serial.println(cdata);

    delay(10);
    isPulled = false;
}

void setup()
{
    Serial.begin(115200);

    Wire.begin(SLAVE_ADDRESS);

    Serial.println(F("Begin setup"));
    dht_setup();
    Serial.println(F("Done dht setting."));
    Serial.println(F("Calibrating gy61"));
    calibrate_gy61();
    delay(1000);
    Serial.println(F("Done calibrating."));

    Wire.onRequest(sendMessage);
    // pinMode(SCL, INPUT);
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    if (!isPulled && (millis() - timer > 500)) // update led every sec
    {
        hum = dht.readHumidity();
        temp = dht.readTemperature();

        float xAccel = readX();
        float yAccel = readY();
        float zAccel = readZ();

        dtostrf(hum, 2, 1, hum_c);
        dtostrf(temp, 2, 1, temp_c);
        dtostrf(xAccel, 2, 2, acc0_c);
        dtostrf(yAccel, 2, 2, acc1_c);
        dtostrf(zAccel, 2, 2, acc2_c);
        // Data is humidity | temperature | acceleration [x, y, z]
        snprintf(cdata, MAX_LENGTH, "%s|%s|[%s,%s,%s]", hum_c, temp_c, acc0_c, acc1_c, acc2_c);
        // Serial.println(cdata);

        // blinks to indicate program is running
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
        timer = millis();
    }
}