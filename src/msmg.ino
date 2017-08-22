/*
 * Project msmg
 * Description: Mangnwon Community Based Dust Sensor
 * Author: Hojun Song
 * Date: 22nd Aug 2017
 */

#include "msmg.h"
#include "SparkIntervalTimer.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"

IntervalTimer Timer;
Adafruit_BME280 bme; // I2C

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
//STARTUP(WiFi.selectAntenna(ANT_INTERNAL));

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  pinMode(test_pin, INPUT);
  attachInterrupt(test_pin, sendInfo, RISING);

  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);

  led_ok = false;
  led_on = false;

  send_info_ok = false;

  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  // if (!bme.begin(0x76)) {
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }


  // run timer every update period
  //Timer.begin(update, update_period * 2, hmSec, TIMER6);
  tone(buzzer_pin, 4000,100);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
  delay(2000);

  ledControl();

  if(send_info_ok)
  {
    Particle.publish("RSSI",String(WiFi.RSSI()));
    Particle.publish("Local IP",String(WiFi.localIP()));
    send_info_ok = false;
  }

}

void update() {
  led_ok = true;
}

void ledControl()
{

  if(led_ok)
  {
    if(led_on == false)
      led_on = true;
    else
      led_on = false;


    if(led_on)
    {
      digitalWrite(red_led_pin, HIGH);
      digitalWrite(green_led_pin, HIGH);
      digitalWrite(blue_led_pin, HIGH);
      tone(buzzer_pin, 4000,100);
    }
    else
    {
      digitalWrite(red_led_pin, LOW);
      digitalWrite(green_led_pin, LOW);
      digitalWrite(blue_led_pin, LOW);
    }

    led_ok = false;
  }
}

void sendInfo()
{
  send_info_ok = true;

}
