#include <Wire.h>
#include <BH1750FVI.h>
BH1750FVI LightSensor;

#include "TLC59108.h"
#define HW_RESET_PIN 0
#define I2C_ADDR TLC59108::I2C_ADDR::BASE
TLC59108 leds(I2C_ADDR + 0);

// настройки I2C для платы MGB-I2C63EN
#define I2C_HUB_ADDR 0x70
#define EN_MASK 0x08
#define DEF_CHANNEL 0x00
#define MAX_CHANNEL 0x08

/*
  I2C порт 0x08 - выводы GP21 (SDA), GP22 (SCL)
  I2C порт 0x07 - выводы GP16 (SDA), GP17 (SCL)
  I2C порт 0x06 - выводы GP4 (SDA), GP13 (SCL)
  I2C порт 0x05 - выводы GP14 (SDA), GP15 (SCL)
  I2C порт 0x04 - выводы GP5 (SDA), GP23 (SCL)
  I2C порт 0x03 - выводы GP18 (SDA), GP19 (SCL)
*/

void setup() {
  Wire.begin();
  Serial.begin(115200);

  pinMode(13, OUTPUT);

  setBusChannel(0x07);
  LightSensor.begin();
  LightSensor.setMode(Continuously_High_Resolution_Mode);

  setBusChannel(0x08);
  leds.init(HW_RESET_PIN);
  leds.setLedOutputMode(TLC59108::LED_MODE::PWM_IND);
  leds.setAllBrightness(byte(0));
}

void loop() {
  bool left, right;
  tumbler(left, right);
}

void tumbler(bool automatic_mode, bool manual_mode) {
  automatic_mode = digitalRead(15);
  manual_mode = digitalRead(14);
  Serial.println(String(automatic_mode));
  Serial.println(String(manual_mode));
  delay(100);
  
  // LEFT pos tumbler
  while (automatic_mode == LOW) {
    automatic_mode = digitalRead(15);
    delay(10);
    if (automatic_mode == LOW) {
      Serial.println("automatic_mode ON " + String(automatic_mode));
      atomatic();
    } else if (automatic_mode == HIGH) {
      setBusChannel(0x08);
      leds.setAllBrightness(byte(0));
      delay(500);
      break;
    }
  }

  // RIGHT pos tumbler
  while (manual_mode == LOW) {
    manual_mode = digitalRead(14);
    delay(10);
    if (manual_mode == LOW) {
      Serial.println("manual_mode ON " + String(manual_mode));
      setBusChannel(0x08);
      leds.setBrightness(0, 0xff);
      leds.setBrightness(6, 0xff);

      leds.setBrightness(3, 0xff); // красный
      leds.setBrightness(2, 0xff); // зеленый
      leds.setBrightness(5, 0xff); // синий
    } else {
      setBusChannel(0x08);
      leds.setAllBrightness(byte(0));
      delay(500);
      break;
    }
  }
}

void atomatic() {
  setBusChannel(0x07);
  float lux = LightSensor.getAmbientLight();
  Serial.println("Light sensivity: " + String(lux, 1) + " Люкс");
  delay(500);
  if (lux < 300) {
    setBusChannel(0x08);
    leds.setBrightness(0, 0xff);
    leds.setBrightness(6, 0xff);

    leds.setBrightness(3, 0xff); // красный
    leds.setBrightness(2, 0xff); // зеленый
    leds.setBrightness(5, 0xff); // синий

  } else {
    setBusChannel(0x08);
    leds.setBrightness(0, 0x00);
    leds.setBrightness(6, 0x00);

    leds.setBrightness(3, 0x00);
    leds.setBrightness(2, 0x00);
    leds.setBrightness(5, 0x00);
  }
}

bool setBusChannel(uint8_t i2c_channel) {
  if (i2c_channel >= MAX_CHANNEL) {
    return false;
  } else {
    Wire.beginTransmission(I2C_HUB_ADDR);
    Wire.write(i2c_channel | EN_MASK);
    Wire.endTransmission();
    return true;
  }
}