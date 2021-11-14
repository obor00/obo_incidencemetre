/*

	Incidencemetre
	Author: Obor 14/11/2021

	libraries:
	MPU9250 by hideakitai https://github.com/hideakitai/MPU9250
	U8G2 by Oliver https://github.com/olikraus/u8g2

	OLD model: SSH1106, 128*64
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

// setup SH1106_128X64
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

MPU9250 mpu;
MovingAverage<float> xAvg(10);
MovingAverage<float> yAvg(10);
MovingAverage<float> zAvg(10);

void setup(void) {

	//Serial.begin(115200);

	// --------------------
	// Oled setting
	u8g2.begin();
	u8g2.setBusClock(400000);
	//u8g2.setFlipMode(0);

	// --------------------
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

	// --------------------
	//  Oled start draw
	u8g2.clearBuffer();
	u8g2.setFont(u8g2_font_ncenB08_tr);
	u8g2.drawStr(0,10,"  Incidencemetre OBO");
	u8g2.drawFrame(0,0,u8g2.getDisplayWidth(),u8g2.getDisplayHeight() );
	u8g2.sendBuffer();

	u8g2.setFont(u8g2_font_10x20_mf);
	u8g2.setFontMode(0);
	u8g2.setDrawColor(1);
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

void write_angles() {

	xAvg.push(mpu.getEulerX());
	yAvg.push(mpu.getEulerY());
	zAvg.push(mpu.getEulerZ());

	String mystr;
	mystr = "X: " + String (xAvg.get(),1) + " ";
	u8g2.drawStr(30,30,mystr.c_str());
	mystr = "Y: " + String (yAvg.get(),1) + " ";
	u8g2.drawStr(30,52,mystr.c_str());
	//mystr = "Z = " + String (zAvg.get());
	//u8g2.drawStr(0,50,mystr.c_str());	// write something to the internal memory

	u8g2.sendBuffer();
}

void loop(void) {
	if (mpu.update()) {
		static uint32_t prev_ms = millis();
		if (millis() > prev_ms + 100) {
			// print_roll_pitch_yaw();
			prev_ms = millis();
			write_angles();
		}
	}
}

