#include <Wire.h>
#include <DHT.h>

#define INTERRUPT_PIN 2 // use pin 2 on Arduino Uno & most boards
#define LED_PIN A1      // (Arduino is 13, Teensy is 11, Teensy++ is 6)
#define SLAVE_ADDRESS 0x04
#define DHTPIN 7
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

bool blinkState = false;
unsigned long timer = 0;
volatile bool isPulled = false;
char cdata[20];
float hum, temp;
char hum_c[7], temp_c[7];

void sendMessage()
{
  isPulled = true;
  delay(10);

  // on request send message
  Wire.write(cdata);
  Serial.print(F("Data is: "));
  Serial.println(cdata);

  isPulled = false;
  delay(10);
}

void dht_setup()
{
  dht.begin();
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(SLAVE_ADDRESS);
  // Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

  Serial.println(F("Begin setup."));
  dht_setup();
  Serial.println(F("Done dht setting."));

  Wire.onRequest(sendMessage); // call sendMessage() on request by master
  pinMode(LED_PIN, OUTPUT);    // setup LED pin for output
}

void loop()
{
  if (!isPulled && (millis() - timer > 2000))
  {
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    dtostrf(hum, 2, 2, hum_c);
    dtostrf(temp, 2, 2, temp_c);
    snprintf(cdata, 20, "|h=%s|t=%s|", hum_c, temp_c);

    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState); // turn the LED off by making the voltage LOW
    timer = millis();
  }
}