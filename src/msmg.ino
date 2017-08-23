/*
 * Project msmg
 * Description: Mangnwon Community Based Dust Sensor
 * Author: Hojun Song
 * Date: 22nd Aug 2017
 */
SYSTEM_THREAD(ENABLED)

#include "msmg.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
#include "Adafruit_TSL2591.h"
#include "MSMG_HPMA115.h"

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
//STARTUP(WiFi.selectAntenna(ANT_INTERNAL));

Adafruit_BME280 bme; // I2C
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
MSMG_HPMA115 dust_sensor(&Serial1);

// every 1000 ms
Timer timer(1000, timerCallback);

void setup() {

  pinMode(test_pin, INPUT);
  attachInterrupt(test_pin, sendInfo, RISING);

  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);

  led_ok = false;
  led_on = false;

  send_info_ok = false;
  publish_data_ok = false;

  // debug serial
  Serial.begin(115200);



  dust_sensor.begin(9600);

  dust_sensor.stopMeasurement();
  delay(10);
  dust_sensor.startMeasurement();
  delay(10);
  dust_sensor.enableAutoSend();


  bme.begin();

  tsl.begin();

  // TSL2591 Sensor Configure
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)


  tone(buzzer_pin, 4000,100);
  timer.start();
}

void loop() {

  dust_sensor.readAutoData();

  if(dust_sensor.dataReady())
  {
    dust_sensor.getData();
    //Serial.write(dust_sensor.getData(),32);
  }


  if(led_ok)
  {

    led_ok = false;
  }

  if(send_info_ok)
  {
    Particle.publish("mangwon", "RSSI: " + String(WiFi.RSSI()), PRIVATE);
    Particle.publish("mangwon", "Local IP: " + String(WiFi.localIP()), PRIVATE);
    send_info_ok = false;
  }

  if(publish_data_ok)
  {
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;

    String pm2_5Str = String(dust_sensor.getLastPM2_5Data());
    String pm10Str = String(dust_sensor.getLastPM10Data());
    String tempStr = String(bme.readTemperature()); // C
    String pressureStr = String(bme.readPressure() / 100.0F); // hPa
    String humidStr = String(bme.readHumidity()); // %
    String luxStr = String(tsl.calculateLux(full, ir)); // lux

    String allDataStr ="";
    allDataStr = "PM2.5: " + pm2_5Str + "," + "PM10: " + pm10Str + "," + "TEMP: " + tempStr + "," + "PRESS: " + pressureStr +"," + "HUMID: " + humidStr +","+ "LUX: " + luxStr;

    Particle.publish("mangwon", allDataStr, PRIVATE);

    publish_data_ok = false;
  }
}

void timerCallback() {
  publish_data_ok = true;
}

void ledControl()
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
}

void sendInfo()
{
  send_info_ok = true;

}
