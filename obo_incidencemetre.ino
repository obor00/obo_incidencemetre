/*

  Incidencemetre

  libraries:
	MPU9250 by hideakitai https://github.com/hideakitai/MPU9250
	U8G2 by Oliver https://github.com/olikraus/u8g2
*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "MPU9250.h"
#include "eeprom_utils.h"
#include <MovingAverage.h>

/*
  extract from U8g2lib Example Overview:
*/
//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//#ifdef OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//#endif

MPU9250 mpu;
MovingAverage<float> xAvg(10);
MovingAverage<float> yAvg(10);
MovingAverage<float> zAvg(10);

void setup(void) {

	//Serial.begin(115200);

	// Oled setting
	u8g2.begin();
	u8g2.setBusClock(400000);
	//u8g2.setFlipMode(0);

	// MPU9250 setting
	Wire.begin();
	delay(2000);
	if (!mpu.setup(0x68)) {
		while (1) {
			Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
			delay(5000);
		}
	}
    loadCalibration();
	mpu.selectFilter(QuatFilterSel::MADGWICK);
	mpu.setFilterIterations(10);
}


void print_roll_pitch_yaw() {
	Serial.print("Yaw, Pitch, Roll: ");
	Serial.print(mpu.getYaw(), 2);
	Serial.print(", ");
	Serial.print(mpu.getPitch(), 2);
	Serial.print(", ");
	Serial.println(mpu.getRoll(), 2);
}

int count = 0;

void write_something() {
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Incidencemetre");	// write something to the internal memory

  xAvg.push(mpu.getEulerX());
  yAvg.push(mpu.getEulerY());
  zAvg.push(mpu.getEulerZ());

  String mystr;
  mystr = "X = " + String (xAvg.get());
  u8g2.drawStr(0,30,mystr.c_str());	// write something to the internal memory
  mystr = "Y = " + String (yAvg.get());
  u8g2.drawStr(0,40,mystr.c_str());	// write something to the internal memory
  mystr = "Z = " + String (zAvg.get());
  u8g2.drawStr(0,50,mystr.c_str());	// write something to the internal memory

  u8g2.sendBuffer();					// transfer internal memory to the display
}

void loop(void) {
  if (mpu.update()) {
	  static uint32_t prev_ms = millis();
	  if (millis() > prev_ms + 10) {
		  // print_roll_pitch_yaw();
		  prev_ms = millis();
		  write_something();
	  }
  }
}
