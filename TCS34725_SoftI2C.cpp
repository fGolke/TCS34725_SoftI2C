/*
	License: GNU GPL 3.0
	Copyright (C) 2020 Felix Golke 
	For further information see TCS34725_SoftI2C.h
*/

//########################################################################################################################## TEMP
#define RG_MAX 0.80
#define BG_MAX 0.82
#define CG_MAX 2.7
#define CG_MIN 2.0



#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif
#include <stdlib.h>
#include <math.h>

#include "TCS34725_SoftI2C.h"

float powFlt(const float x, const float y)
{
  return (float)(pow((double)x, (double)y));
}

namespace TCS34725{
	
	void ColourSensor::writeReg (uint8_t reg, uint32_t value)
	{
	softWire.beginTransmission(TCS34725_ADDRESS);
	softWire.write(TCS34725_COMMAND_BIT | reg);
	softWire.write(value & 0xFF);
	softWire.endTransmission();
	}
	
	
	uint8_t ColourSensor::readReg(uint8_t reg)
	{
	softWire.beginTransmission(TCS34725_ADDRESS);
	softWire.write(TCS34725_COMMAND_BIT | reg);
	softWire.endTransmission();
	
	softWire.requestFrom(TCS34725_ADDRESS, 1);
	return softWire.read();
	}
	
	
	uint16_t ColourSensor::readRegWord(uint8_t reg)
	{
	uint16_t x; uint16_t t;
	
	softWire.beginTransmission(TCS34725_ADDRESS);
	softWire.write(TCS34725_COMMAND_BIT | reg);
	softWire.endTransmission();
	
	softWire.requestFrom(TCS34725_ADDRESS, 2);
	t = softWire.read();
	x = softWire.read();
	x <<= 8;
	x |= t;
	return x;
	}
	
	
	void ColourSensor::enable(void)
	{
	writeReg(TCS34725_ENABLE, TCS34725_ENABLE_PON);
	delay(3);
	writeReg(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
	}
	
	
	void ColourSensor::disable(void)
	{
	/* Turn the device off to save power */
	uint8_t reg = 0;
	reg = readReg(TCS34725_ENABLE);
	writeReg(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
	}
	
	
	
	ColourSensor::ColourSensor(uint8_t sclPin, uint8_t sdaPin, IntegrationTime_t it, Gain_t gain) : softWire(sdaPin, sclPin)
	{
	_tcs34725_initialised = false;
	_tcs34725_intTime = it;
	_tcs34725_gain = gain;
	}
	
	
	boolean ColourSensor::begin(void) 
	{
	softWire.begin();
	
	/* Make sure we're actually connected */
	uint8_t x = readReg(TCS34725_ID);
	if ((x != 0x44) && (x != 0x10))
	{
		return false;
	}
	_tcs34725_initialised = true;
	
	/* Set default integration time and gain */
	setIntegrationTime(_tcs34725_intTime);
	setGain(_tcs34725_gain);
	
	/* Note: by default, the device is in power down mode on bootup */
	enable();
	
	return true;
	}
	
	
	void ColourSensor::setIntegrationTime(TCS34725::IntegrationTime_t it)
	{
	if (!_tcs34725_initialised) begin();
	
	/* Update the timing register */
	writeReg(TCS34725_ATIME, it);
	
	/* Update value placeholders */
	_tcs34725_intTime = it;
	}
	
	
	void ColourSensor::setGain(TCS34725::Gain_t gain)
	{
	if (!_tcs34725_initialised) begin();
	
	/* Update the timing register */
	writeReg(TCS34725_CONTROL, gain);
	
	/* Update value placeholders */
	_tcs34725_gain = gain;
	}
	
	void ColourSensor::read(){
		if (!_tcs34725_initialised) begin();
	
		m_rawValues[0] = readRegWord(TCS34725_RDATAL);
		m_rawValues[1] = readRegWord(TCS34725_GDATAL);
		m_rawValues[2] = readRegWord(TCS34725_BDATAL);
		m_rawValues[3] = readRegWord(TCS34725_CDATAL);
	}
	
	uint16_t ColourSensor::get(uint8_t part){
		if(part <= PART_C)
			return ColourSensor::m_rawValues[part];
		switch(part){
			case REDNESS:
				return ColourSensor::m_colourConfidence[PART_R];
			case GREENNESS:
				return ColourSensor::m_colourConfidence[PART_G];
			default:
				return 0;
		}
	}
	
	void ColourSensor::determineColour(){
		int16_t red		= (int16_t)get(PART_R);
		int16_t green 	= (int16_t)get(PART_G);
		int16_t blue	= (int16_t)get(PART_B);
		int16_t clear 	= (int16_t)get(PART_C);
		
		int16_t max = max(red, max(green, blue));
		
		double rg = (double)red   / (double)green;
		double bg = (double)blue  / (double)green;
		double cg = (double)clear / (double)green;
		
		m_colourConfidence[PART_R] = 0;
		m_colourConfidence[PART_G] = 0;
		
		if(max == green && rg < RG_MAX && bg < BG_MAX && cg < CG_MAX && cg > CG_MIN)	
			m_colourConfidence[PART_G] = (uint8_t) (((RG_MAX - rg) + (BG_MAX - bg) + (CG_MAX - cg)) * 80.0);
		
		//if(max == green && rg < RG_MAX && bg < BG_MAX && cg < CG_MAX && cg > CG_MIN)	
		//	m_colourConfidence[PART_R] = (uint8) (((RG_MAX - rg) + (BG_MAX - bg) + (CG_MAX - cg)) * 80.0);
		
	}
	
	
	void ColourSensor::lock()
	{
		uint8_t r = readReg(TCS34725_ENABLE);
		r |= TCS34725_ENABLE_AIEN;
		writeReg(TCS34725_ENABLE, r);
	}
	
	void ColourSensor::unlock()
	{
		uint8_t r = readReg(TCS34725_ENABLE);
		r &= ~TCS34725_ENABLE_AIEN;
		writeReg(TCS34725_ENABLE, r);
	}
	
	
	void ColourSensor::clear(void) {
	softWire.beginTransmission(TCS34725_ADDRESS);
	softWire.write(TCS34725_COMMAND_BIT | 0x66);
	softWire.endTransmission();
	}
	
	
	void ColourSensor::setIntLimits(uint16_t low, uint16_t high) {
	writeReg(0x04, low & 0xFF);
	writeReg(0x05, low >> 8);
	writeReg(0x06, high & 0xFF);
	writeReg(0x07, high >> 8);
	}

}
