/*
 * Project msmg
 * Description: Mangnwon Community Based Dust Sensor
 * Author: Hojun Song
 * Date: 22nd Aug 2017
 */

SYSTEM_THREAD(ENABLED)

#include "msmg.h"
#include "SparkIntervalTimer.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME280.h"
#include "Adafruit_TSL2591.h"
#include "MSMG_HPMA115.h"

IntervalTimer Timer;
Adafruit_BME280 bme; // I2C
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
MSMG_HPMA115 dust_sensor(&Serial1);

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));
//STARTUP(WiFi.selectAntenna(ANT_INTERNAL));

void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution); Serial.println(F(" lux"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}

void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC);
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}

void setup() {
  pinMode(test_pin, INPUT);
  attachInterrupt(test_pin, sendInfo, RISING);

  pinMode(red_led_pin, OUTPUT);
  pinMode(green_led_pin, OUTPUT);
  pinMode(blue_led_pin, OUTPUT);

  led_ok = false;
  led_on = false;

  send_info_ok = false;


  dust_sensor.begin(9600);

  Serial.begin(115200);
  Serial.println(F("BME280 test"));

  dust_sensor.stopMeasurement();
  delay(200);
  dust_sensor.startMeasurement();
  delay(100);
  dust_sensor.enableAutoSend();

  // if (!bme.begin(0x76)) {
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  if (tsl.begin())
  {
    Serial.println(F("Found a TSL2591 sensor"));
  }
  else
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Configure the sensor */
  configureSensor();

  // run timer every update period
  // Timer.begin(update, update_period * 2, hmSec, TIMER6);
  tone(buzzer_pin, 4000,100);
}

void advancedRead(void)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("IR: ")); Serial.print(ir);  Serial.print(F("  "));
  Serial.print(F("Full: ")); Serial.print(full); Serial.print(F("  "));
  Serial.print(F("Visible: ")); Serial.print(full - ir); Serial.print(F("  "));
  Serial.print(F("Lux: ")); Serial.println(tsl.calculateLux(full, ir));
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  // Serial.print("Temperature = ");
  // Serial.print(bme.readTemperature());
  // Serial.println(" *C");
  //
  // Serial.print("Pressure = ");
  //
  // Serial.print(bme.readPressure() / 100.0F);
  // Serial.println(" hPa");
  //
  // Serial.print("Approx. Altitude = ");
  // Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  // Serial.println(" m");
  //
  // Serial.print("Humidity = ");
  // Serial.print(bme.readHumidity());
  // Serial.println(" %");
  //
  // Serial.println();
  //
  // advancedRead();

  // delay(2000);

  dust_sensor.readAutoData();

  if(dust_sensor.dataReady())
  {
    Serial.write(dust_sensor.getData(),32);
  }

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
