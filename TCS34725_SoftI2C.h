<<<<<<< HEAD
/*
Library to use TCS34725 colour sensors with SoftwareWire I2C instead of hardware I2C.
Copyright (C) 2020 Felix Golke
this library is based on the DFRobot_TCS34725 library (C)[DFRobot](http://www.dfrobot.com), 2016
it uses SoftwareWire by Testato https://www.arduinolibraries.info/libraries/software-wire

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#ifndef _TCS34725_SOFTWIRE_H_
#define _TCS34725_SOFTWARE_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <SoftwareWire.h>

#define TCS34725_ADDRESS          (0x29)

#define TCS34725_COMMAND_BIT      (0x80)

#define TCS34725_ENABLE           (0x00)
#define TCS34725_ENABLE_AIEN      (0x10)    ///< RGBC Interrupt Enable 
#define TCS34725_ENABLE_WEN       (0x08)    ///< Wait enable - Writing 1 activates the wait timer 
#define TCS34725_ENABLE_AEN       (0x02)    ///< RGBC Enable - Writing 1 actives the ADC, 0 disables it 
#define TCS34725_ENABLE_PON       (0x01)    ///< Power on - Writing 1 activates the internal oscillator, 0 disables it 
#define TCS34725_ATIME            (0x01)    ///< Integration time 
#define TCS34725_WTIME            (0x03)    ///< Wait time (if TCS34725_ENABLE_WEN is asserted) 
#define TCS34725_WTIME_2_4MS      (0xFF)    ///< WLONG0 = 2.4ms   WLONG1 = 0.029s 
#define TCS34725_WTIME_204MS      (0xAB)    ///< WLONG0 = 204ms   WLONG1 = 2.45s  
#define TCS34725_WTIME_614MS      (0x00)    ///< WLONG0 = 614ms   WLONG1 = 7.4s   
#define TCS34725_AILTL            (0x04)    ///< Clear channel lower interrupt threshold 
#define TCS34725_AILTH            (0x05)
#define TCS34725_AIHTL            (0x06)    ///< Clear channel upper interrupt threshold 
#define TCS34725_AIHTH            (0x07)
#define TCS34725_PERS             (0x0C)    ///< Persistence register - basic SW filtering mechanism for interrupts 
#define TCS34725_PERS_NONE        (0b0000)  ///< Every RGBC cycle generates an interrupt                                
#define TCS34725_PERS_1_CYCLE     (0b0001)  ///< 1 clean channel value outside threshold range generates an interrupt   
#define TCS34725_PERS_2_CYCLE     (0b0010)  ///< 2 clean channel values outside threshold range generates an interrupt  
#define TCS34725_PERS_3_CYCLE     (0b0011)  ///< 3 clean channel values outside threshold range generates an interrupt  
#define TCS34725_PERS_5_CYCLE     (0b0100)  ///< 5 clean channel values outside threshold range generates an interrupt  
#define TCS34725_PERS_10_CYCLE    (0b0101)  ///< 10 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_15_CYCLE    (0b0110)  ///< 15 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_20_CYCLE    (0b0111)  ///< 20 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_25_CYCLE    (0b1000)  ///< 25 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_30_CYCLE    (0b1001)  ///< 30 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_35_CYCLE    (0b1010)  ///< 35 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_40_CYCLE    (0b1011)  ///< 40 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_45_CYCLE    (0b1100)  ///< 45 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_50_CYCLE    (0b1101)  ///< 50 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_55_CYCLE    (0b1110)  ///< 55 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_60_CYCLE    (0b1111)  ///< 60 clean channel values outside threshold range generates an interrupt 
#define TCS34725_CONFIG           (0x0D)
#define TCS34725_CONFIG_WLONG     (0x02)    ///< Choose between short and long (12x) wait times via TCS34725_WTIME 
#define TCS34725_CONTROL          (0x0F)    ///< Set the gain level for the sensor 
#define TCS34725_ID               (0x12)    ///< 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727 
#define TCS34725_STATUS           (0x13)
#define TCS34725_STATUS_AINT      (0x10)    ///< RGBC Clean channel interrupt 
#define TCS34725_STATUS_AVALID    (0x01)    ///< Indicates that the RGBC channels have completed an integration cycle 
#define TCS34725_CDATAL           (0x14)    ///< Clear channel data 
#define TCS34725_CDATAH           (0x15)
#define TCS34725_RDATAL           (0x16)    ///< Red channel data 
#define TCS34725_RDATAH           (0x17)
#define TCS34725_GDATAL           (0x18)    ///< Green channel data 
#define TCS34725_GDATAH           (0x19)
#define TCS34725_BDATAL           (0x1A)    ///< Blue channel data 
#define TCS34725_BDATAH           (0x1B)

#define PART_R 0
#define PART_G 1
#define PART_B 2
#define PART_C 3

#define REDNESS   4
#define GREENNESS 5


namespace TCS34725{
typedef enum
{
  T_2_4MS  = 0xFF,   ///<  2.4ms - 1 cycle    - Max Count: 1024  
  T_24MS   = 0xF6,   ///<  24ms  - 10 cycles  - Max Count: 10240 
  T_50MS   = 0xEB,   ///<  50ms  - 20 cycles  - Max Count: 20480 
  T_101MS  = 0xD5,   ///<  101ms - 42 cycles  - Max Count: 43008 
  T_154MS  = 0xC0,   ///<  154ms - 64 cycles  - Max Count: 65535 
  T_700MS  = 0x00    ///<  700ms - 256 cycles - Max Count: 65535 
} 
IntegrationTime_t;	

typedef enum
{
  GAIN_1X                = 0x00,   ///<  No gain  
  GAIN_4X                = 0x01,   ///<  4x gain  
  GAIN_16X               = 0x02,   ///<  16x gain 
  GAIN_60X               = 0x03    ///<  60x gain 
}
Gain_t;

class ColourSensor {
 public:
		/*!
		*  @brief Constructor.
		*  @param uint8_t sclPin	pin for serial clock 
		*  @param uint8_t sdaPin	pin for I2C data
		*  @param tcs34725IntegrationTime_t  Sensor integration time
		*  @param tcs34725Gain_t gain Sensor gain
		*/
ColourSensor(uint8_t sclPin, uint8_t sdaPin, TCS34725::IntegrationTime_t = 0xF8, TCS34725::Gain_t = TCS34725::GAIN_1X);
		/*!
		*  @brief Initializes SoftwareWire and configures the sensor (call this function before doing anything else).
		*  @return  true success.
		*/
  boolean  begin(void);
  
		/*!
		*  @brief Sets the integration time for the TC34725.
		*  @param it  integration time.
		*/
  void     setIntegrationTime(TCS34725::IntegrationTime_t it);
		/*!
		*  @brief Adjusts the gain on the TCS34725 (adjusts the sensitivity to light)
		*  @param gain  gain time.
		*/
  void     setGain(TCS34725::Gain_t);
  
		/*!
		*	@brief reads the crgb values and stores them internally
		*
		*/
  void	   read(void);
 		/*!
		*	@brief returns the desired property of the last measurement
		*	@param part Desired part of the colour (PART_R/G/B/C) OR GREENNESS, REDNESS
		*   @return desired property of the colour
		*/
  uint16_t get(uint8_t part); 
   		/*!
		*	@brief determines the colour Confidence
		*/
  void determineColour(void);
  
  
		/*!
		*  @brief Writes a register and an 8 bit value over I2C
		*  @param reg register address .
		*  @param value  data.
		*/
  void     writeReg (uint8_t reg, uint32_t value);
		/*!
		*  @brief Reads an 8 bit value over I2C
		*  @param reg register address .
		*  @return I2C  data.
		*/
  uint8_t  readReg (uint8_t reg);
		/*!
		*  @brief Reads an 8 bit value over I2C
		*  @param reg register address .
		*  @return I2C  data.
		*/
  uint16_t readRegWord (uint8_t reg);
		/*!
		*  @brief Interrupts enabled
		*/
  void lock(void);
		/*!
		*  @brief Interrupts disabled
		*/
  void unlock(void);
		/*!
		*  @brief clear Interrupts
		*/
  void clear(void);
		/*!
		*  @brief set Int Limits
		*  @param l low .
		*  @param h high .
		*/
  void setIntLimits(uint16_t l, uint16_t h);
		/*!
		*  @brief Enables the device
		*/
  void     enable(void);

 private:
  SoftwareWire softWire;				///<Software Wire object
  boolean _tcs34725_initialised;			///<init flag
  TCS34725::Gain_t _tcs34725_gain;				///<gain time
  TCS34725::IntegrationTime_t _tcs34725_intTime; 	///<Integration time
  uint16_t m_rawValues[4]     = {0, 0, 0, 0};
  uint8_t m_colourConfidence[2] = {0, 0}; //Red, Green
  
  
		/*!
		*  Disables the device (putting it in lower power sleep mode)
		*/
		
  void     disable(void);
};
	
}





#endif
=======
/*
Library to use TCS34725 colour sensors with SoftwareWire I2C instead of hardware I2C.
Copyright (C) 2020 Felix Golke
this library is based on the DFRobot_TCS34725 library (C)[DFRobot](http://www.dfrobot.com), 2016
it uses SoftwareWire by Testato https://www.arduinolibraries.info/libraries/software-wire

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#ifndef _TCS34725_SOFTWIRE_H_
#define _TCS34725_SOFTWARE_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <SoftwareWire.h>

#define TCS34725_ADDRESS          (0x29)

#define TCS34725_COMMAND_BIT      (0x80)

#define TCS34725_ENABLE           (0x00)
#define TCS34725_ENABLE_AIEN      (0x10)    ///< RGBC Interrupt Enable 
#define TCS34725_ENABLE_WEN       (0x08)    ///< Wait enable - Writing 1 activates the wait timer 
#define TCS34725_ENABLE_AEN       (0x02)    ///< RGBC Enable - Writing 1 actives the ADC, 0 disables it 
#define TCS34725_ENABLE_PON       (0x01)    ///< Power on - Writing 1 activates the internal oscillator, 0 disables it 
#define TCS34725_ATIME            (0x01)    ///< Integration time 
#define TCS34725_WTIME            (0x03)    ///< Wait time (if TCS34725_ENABLE_WEN is asserted) 
#define TCS34725_WTIME_2_4MS      (0xFF)    ///< WLONG0 = 2.4ms   WLONG1 = 0.029s 
#define TCS34725_WTIME_204MS      (0xAB)    ///< WLONG0 = 204ms   WLONG1 = 2.45s  
#define TCS34725_WTIME_614MS      (0x00)    ///< WLONG0 = 614ms   WLONG1 = 7.4s   
#define TCS34725_AILTL            (0x04)    ///< Clear channel lower interrupt threshold 
#define TCS34725_AILTH            (0x05)
#define TCS34725_AIHTL            (0x06)    ///< Clear channel upper interrupt threshold 
#define TCS34725_AIHTH            (0x07)
#define TCS34725_PERS             (0x0C)    ///< Persistence register - basic SW filtering mechanism for interrupts 
#define TCS34725_PERS_NONE        (0b0000)  ///< Every RGBC cycle generates an interrupt                                
#define TCS34725_PERS_1_CYCLE     (0b0001)  ///< 1 clean channel value outside threshold range generates an interrupt   
#define TCS34725_PERS_2_CYCLE     (0b0010)  ///< 2 clean channel values outside threshold range generates an interrupt  
#define TCS34725_PERS_3_CYCLE     (0b0011)  ///< 3 clean channel values outside threshold range generates an interrupt  
#define TCS34725_PERS_5_CYCLE     (0b0100)  ///< 5 clean channel values outside threshold range generates an interrupt  
#define TCS34725_PERS_10_CYCLE    (0b0101)  ///< 10 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_15_CYCLE    (0b0110)  ///< 15 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_20_CYCLE    (0b0111)  ///< 20 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_25_CYCLE    (0b1000)  ///< 25 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_30_CYCLE    (0b1001)  ///< 30 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_35_CYCLE    (0b1010)  ///< 35 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_40_CYCLE    (0b1011)  ///< 40 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_45_CYCLE    (0b1100)  ///< 45 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_50_CYCLE    (0b1101)  ///< 50 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_55_CYCLE    (0b1110)  ///< 55 clean channel values outside threshold range generates an interrupt 
#define TCS34725_PERS_60_CYCLE    (0b1111)  ///< 60 clean channel values outside threshold range generates an interrupt 
#define TCS34725_CONFIG           (0x0D)
#define TCS34725_CONFIG_WLONG     (0x02)    ///< Choose between short and long (12x) wait times via TCS34725_WTIME 
#define TCS34725_CONTROL          (0x0F)    ///< Set the gain level for the sensor 
#define TCS34725_ID               (0x12)    ///< 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727 
#define TCS34725_STATUS           (0x13)
#define TCS34725_STATUS_AINT      (0x10)    ///< RGBC Clean channel interrupt 
#define TCS34725_STATUS_AVALID    (0x01)    ///< Indicates that the RGBC channels have completed an integration cycle 
#define TCS34725_CDATAL           (0x14)    ///< Clear channel data 
#define TCS34725_CDATAH           (0x15)
#define TCS34725_RDATAL           (0x16)    ///< Red channel data 
#define TCS34725_RDATAH           (0x17)
#define TCS34725_GDATAL           (0x18)    ///< Green channel data 
#define TCS34725_GDATAH           (0x19)
#define TCS34725_BDATAL           (0x1A)    ///< Blue channel data 
#define TCS34725_BDATAH           (0x1B)

#define PART_R 0
#define PART_G 1
#define PART_B 2
#define PART_C 3

#define REDNESS   4
#define GREENNESS 5


namespace TCS34725{
typedef enum
{
  T_2_4MS  = 0xFF,   ///<  2.4ms - 1 cycle    - Max Count: 1024  
  T_24MS   = 0xF6,   ///<  24ms  - 10 cycles  - Max Count: 10240 
  T_50MS   = 0xEB,   ///<  50ms  - 20 cycles  - Max Count: 20480 
  T_101MS  = 0xD5,   ///<  101ms - 42 cycles  - Max Count: 43008 
  T_154MS  = 0xC0,   ///<  154ms - 64 cycles  - Max Count: 65535 
  T_700MS  = 0x00    ///<  700ms - 256 cycles - Max Count: 65535 
} 
IntegrationTime_t;	

typedef enum
{
  GAIN_1X                = 0x00,   ///<  No gain  
  GAIN_4X                = 0x01,   ///<  4x gain  
  GAIN_16X               = 0x02,   ///<  16x gain 
  GAIN_60X               = 0x03    ///<  60x gain 
}
Gain_t;

class ColourSensor {
 public:
		/*!
		*  @brief Constructor.
		*  @param uint8_t sclPin	pin for serial clock 
		*  @param uint8_t sdaPin	pin for I2C data
		*  @param tcs34725IntegrationTime_t  Sensor integration time
		*  @param tcs34725Gain_t gain Sensor gain
		*/
ColourSensor(uint8_t sclPin, uint8_t sdaPin, TCS34725::IntegrationTime_t = 0xF8, TCS34725::Gain_t = TCS34725::GAIN_1X);
		/*!
		*  @brief Initializes SoftwareWire and configures the sensor (call this function before doing anything else).
		*  @return  true success.
		*/
  boolean  begin(void);
  
		/*!
		*  @brief Sets the integration time for the TC34725.
		*  @param it  integration time.
		*/
  void     setIntegrationTime(TCS34725::IntegrationTime_t it);
		/*!
		*  @brief Adjusts the gain on the TCS34725 (adjusts the sensitivity to light)
		*  @param gain  gain time.
		*/
  void     setGain(TCS34725::Gain_t);
  
		/*!
		*	@brief reads the crgb values and stores them internally
		*
		*/
  void	   read(void);
 		/*!
		*	@brief returns the desired property of the last measurement
		*	@param part Desired part of the colour (PART_R/G/B/C) OR GREENNESS, REDNESS
		*   @return desired property of the colour
		*/
  uint16_t get(uint8_t part); 
   		/*!
		*	@brief determines the colour Confidence
		*/
  void determineColour(void);
  
  
		/*!
		*  @brief Writes a register and an 8 bit value over I2C
		*  @param reg register address .
		*  @param value  data.
		*/
  void     writeReg (uint8_t reg, uint32_t value);
		/*!
		*  @brief Reads an 8 bit value over I2C
		*  @param reg register address .
		*  @return I2C  data.
		*/
  uint8_t  readReg (uint8_t reg);
		/*!
		*  @brief Reads an 8 bit value over I2C
		*  @param reg register address .
		*  @return I2C  data.
		*/
  uint16_t readRegWord (uint8_t reg);
		/*!
		*  @brief Interrupts enabled
		*/
  void lock(void);
		/*!
		*  @brief Interrupts disabled
		*/
  void unlock(void);
		/*!
		*  @brief clear Interrupts
		*/
  void clear(void);
		/*!
		*  @brief set Int Limits
		*  @param l low .
		*  @param h high .
		*/
  void setIntLimits(uint16_t l, uint16_t h);
		/*!
		*  @brief Enables the device
		*/
  void     enable(void);

 private:
  SoftwareWire softWire;				///<Software Wire object
  boolean _tcs34725_initialised;			///<init flag
  TCS34725::Gain_t _tcs34725_gain;				///<gain time
  TCS34725::IntegrationTime_t _tcs34725_intTime; 	///<Integration time
  uint16_t m_rawValues[4]     = {0, 0, 0, 0};
  uint8_t m_colourConfidence[2] = {0, 0}; //Red, Green
  
  
		/*!
		*  Disables the device (putting it in lower power sleep mode)
		*/
		
  void     disable(void);
};
	
}





#endif
>>>>>>> 0062cfb (Add basic functionality)
