/*

  Incidencemetre

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

//#define OLED
#define _MPU9250

/*
  extract from U8g2lib Example Overview:
*/
//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//#ifdef OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//#endif

#ifdef _MPU9250
MPU9250 mpu;
#endif

void setup(void) {
#ifdef _MPU9250
	Serial.begin(115200);

	/* U8g2 Project: SSD1306 Test Board */
#if 0
	pinMode(10, OUTPUT);
	pinMode(9, OUTPUT);
	digitalWrite(10, 0);
	digitalWrite(9, 0);
#endif
#endif
//#ifdef OLED
	u8g2.begin();
	u8g2.setBusClock(400000);
	//u8g2.setFlipMode(0);
//#endif
#ifdef _MPU9250
	Wire.begin();
	delay(2000);
	if (!mpu.setup(0x68)) {
		while (1) {
			Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
			delay(5000);
		}
	}
#endif
}

#ifdef _MPU9250
void print_roll_pitch_yaw() {
	Serial.print("Yaw, Pitch, Roll: ");
	Serial.print(mpu.getYaw(), 2);
	Serial.print(", ");
	Serial.print(mpu.getPitch(), 2);
	Serial.print(", ");
	Serial.println(mpu.getRoll(), 2);
}
#endif

int count = 0;

void write_something() {
//#ifdef OLED
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Incidencemetre");	// write something to the internal memory

  // u8g2.sendBuffer();					// transfer internal memory to the display
  // String mystr = String(count);
  // u8g2.drawStr(0,20,mystr.c_str());	// write something to the internal memory
  // count++;
  // u8g2.sendBuffer();					// transfer internal memory to the display

  String mystr;
  mystr = "Yaw = " + String (mpu.getYaw());
  u8g2.drawStr(0,30,mystr.c_str());	// write something to the internal memory
  mystr = "Pitch = " + String (mpu.getPitch());
  u8g2.drawStr(0,40,mystr.c_str());	// write something to the internal memory
  mystr = "Roll = " + String (mpu.getRoll());
  u8g2.drawStr(0,50,mystr.c_str());	// write something to the internal memory

  u8g2.sendBuffer();					// transfer internal memory to the display
//#endif
}

void loop(void) {
  // delay between each page
#ifdef _MPU9250
  if (mpu.update()) {
	  static uint32_t prev_ms = millis();
	  if (millis() > prev_ms + 100) {
		  // print_roll_pitch_yaw();
		  prev_ms = millis();
		  write_something();
	  }
  }
#endif
}
