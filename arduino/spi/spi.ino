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
 * SPE  = (SPI Enable) enables SPI when 1
 * DORD = Sends data least Significant Bit First when 1, most Significant Bit first when 0
 * MSTR = Sets the Arduino in controller mode when 1, peripheral mode when 0
 * CPOL = Sets the data clock to be idle when high if set to 1, idle when low if set to 0
 * CPHA = Samples data on the falling edge of the data clock when 1, rising edge when 0
 * SPR1 and SPR0 - Sets the SPI speed, 00 is fastest (4MHz) 11 is slowest (250KHz)
 */

#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <stdlib.h>

// define constants
#define LED_PIN PIN_A1
#define DHTPIN PIN7
#define DHTTYPE DHT22
#define xInput PIN_A2
#define yInput PIN_A3
#define zInput PIN_A6
#define SS PIN_SPI_SS
#define MOSI PIN_SPI_MOSI
#define MISO PIN_SPI_MISO
#define SCLK PIN_SPI_SCK
#define MAX_LENGTH 32

const int sampleSize = 10;
const int RawMin = 0;
const int RawMax = 1023;

// init DHT22
DHT dht(DHTPIN, DHTTYPE);

// define vars
char cdata[MAX_LENGTH];
unsigned long timer = 0;
bool blinkState = false;
volatile byte pos;
volatile bool processed;
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
    int zeroG = 512; // analog input is 0-1023, 512 is the median value
    delay(1000);

    // calibrate the x,y,z axis
    // read 100 samples from each axis and average them out
    int xTotal = 0, yTotal = 0, zTotal = 0;
    for (int i = 0; i < 100; i++)
    {
        xTotal += readAxis(xInput);
        yTotal += readAxis(yInput);
        zTotal += readAxis(zInput);
    }
    Serial.print(F("X average: ")); Serial.println(xTotal / 100);
    xOffset = zeroG - abs(xTotal / 100);
    yOffset = zeroG - abs(yTotal / 100);
    zOffset = zeroG - abs(zTotal / 100);
    Serial.println(F("Offset is: "));
    Serial.print(F("xOffset:\t"));
    Serial.println(xOffset);
    Serial.print(F("yOffset:\t"));
    Serial.println(yOffset);
    Serial.print(F("zOffset:\t"));
    Serial.println(zOffset);
}

void setup()
{
    Serial.begin(115200);

    Wire.begin();
    Serial.println("Begin setup");
    dht_setup();
    Serial.println(F("Done dht setting."));

    Serial.println(F("Calibrating gy61..."));
    calibrate_gy61();
    delay(1000);
    Serial.println(F("Done calibrating gy61."));

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
    if (!processed && (millis() - timer > 500)) // loop every half sec
    {
        hum = dht.readHumidity();
        temp = dht.readTemperature();

        float xAccel = readX(); float yAccel = readY();  float zAccel = readZ();

        dtostrf(hum, 2, 1, hum_c);
        dtostrf(temp, 2, 1, temp_c);
        dtostrf(xAccel, 2, 2, acc0_c);
        dtostrf(yAccel, 2, 2, acc1_c);
        dtostrf(zAccel, 2, 2, acc2_c);
        // Data is humidity | temperature | acceleration [x, y, z]
        snprintf(cdata, MAX_LENGTH, "%s|%s|%s,%s,%s", hum_c, temp_c, acc0_c, acc1_c, acc2_c);
        // Serial.println(cdata);

        // blinks to indicate program is running
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
        timer = millis();
    }
}
