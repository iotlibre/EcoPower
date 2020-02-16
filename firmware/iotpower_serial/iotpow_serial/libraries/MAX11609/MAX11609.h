/*!
 *  @file 		MAX11609.h 
 *  @version	1.0
 * 
 * MAX11609.h library for Prometeo project
 * Created by Alejandro July 27 2017
 * GNU GPL
*/


#ifndef MAX11609_H
#define MAX11609_H

#include <Arduino.h>

#define MAX11609_ADDR	0x33

// Setup register constants
#define REG_MODE_BIT		7
#define SEL_2_BIT			6
#define SEL_1_BIT			5
#define SEL_0_BIT			4
#define CLK_INPUT_BIT		3
#define UNI_BIP_BIT			2
#define RESET_CONFIG_BIT	1

// Configuration register constants
#define SCAN_1_BIT			6
#define SCAN_0_BIT			5
#define CHANNEL_3_BIT		4
#define CHANNEL_2_BIT		3
#define CHANNEL_1_BIT		2
#define CHANNEL_0_BIT		1
#define MEASURE_MODE_BIT	0

// Measure mode constants
#define DIFFERENTIAL_MODE	0
#define SINGLE_ENDED_MODE	1

// Channel selection constants
#define CH_0	0	// Single ended channels
#define CH_1	1
#define CH_2	2
#define CH_3	3
#define CH_4	4
#define CH_5	5
#define CH_6	6
#define CH_7	7

#define CH_0_1	0	// Differential channels
#define CH_1_0	1
#define CH_2_3	2
#define CH_3_2	3
#define CH_4_5	4
#define CH_5_4	5
#define CH_6_7	6
#define CH_7_6	7

#define ALL_CHANNELS	8

// Scanning configuration constants
#define CH_0_TO_SELECTED	0
#define SELECTED_X8			1
#define UPPER_QUARTILE		2
#define ONLY_SELECTED		3

//Reference voltage constants
#define REF_VDD			0x00
#define REF_EXTERNAL	0x02
#define REF_INTERNAL	0x07


class MAX11609
{
	private:
		
		uint8_t setup_reg;
		int16_t voltage_ref = 2048; // mV
		
	public:
	
		//**********************************************************************
		// STANDART MODE FUNCTIONS
		//**********************************************************************
		//! This function sets up the MAX11609 with a default setup and selected Vref
		/*!
		* \param vRef: vref source selection
		*/
		void begin(uint8_t vRef = REF_VDD);
		
		//! This function sets up the MAX11609 with a default setup and selected Vref
		/*!
		* \param vRef: vref source selection
		* \param voltage: voltage value (in millivolts) used by the ADC
		*/
		void begin(uint8_t vRef, int16_t voltage);
		
		//! This function writes the setup byte
		/*!
		 * \param data: the setup byte to write
		 */
		void setupADC(uint8_t data);
		
		//! This function writes the configuration byte
		/*!
		* \param data: the configuration byte to write  
		*/
		void configuration(uint8_t data);
		
		//! This function reads one channel
		/*!
		* \param channel: The channel to convert or read
		* \param mode: '0' for differential mode, '1' for single-ended
		* 
		* \return uint16_t The conversion result or 32767 if there's an 
		*         error.
		*/
		int16_t read(bool mode, uint8_t channel);
		
		//! This function reads one channel
		/*!
		 * \param channel: The channel to convert or read
		 * \param mode: '0' for differential mode, '1' for single-ended
		 * \param raw: '0' for raw measurement, '1' for processed voltage
		 * 
		 * \return int16_t The conversion result or 32767 if there's an 
		 *         error.
		 */
		int16_t read(bool mode, uint8_t channel, bool raw);
		
		//! This function scans all channels conversion into a buffer.
		/*!
		 * \param mode: '0' for differential mode, '1' for single-ended
		 * \param buffer: an array where the channel read values are put.
		 * \param raw: '0' for raw measurement, '1' for processed voltage
		 */
		void scan(bool mode, int16_t *buffer);
		
		//! This function scans all channels conversion into a buffer.
		/*!
		 * \param mode: '0' for differential mode, '1' for single-ended
		 * \param buffer: an array where the channel read values are put.
		 * \param raw: '0' for raw measurement, '1' for processed voltage
		 */
		void scan(bool mode, int16_t *buffer, bool raw);
		
		
		//**********************************************************************
		// FAST MODE FUNCTIONS
		//**********************************************************************
		//! This function sets up the MAX11609 to work with fast reads and scans
		/*!
		 * \param mode: '0' for differential mode, '1' for single-ended
		 * \param  channel: the channel to convert or read
		 */
		void fastConfig(bool mode, uint8_t channel);
		
		//! This function reads the channel selected with fastConfig
		/*!
		 * \param mode: '0' for differential mode, '1' for single-ended
		 *
		 * \return int16_t The conversion result or 32767 if there's an 
		 *         error.
		 */
		int16_t fastRead(bool mode);
		
		//! This function reads the channel selected with fastConfig
		/*!
		 * \param mode: '0' for differential mode, '1' for single-ended
		 * \param raw: '0' for raw measurement, '1' for processed voltage
		 *
		 * \return int16_t The conversion result or 32767 if there's an 
		 *         error.
		 */
		int16_t fastRead(bool mode, bool raw);
		
		//! This function scans all channels conversion into a buffer.
		/*!
		* \param mode: '0' for differential mode, '1' for single-ended
		* \param buffer: an array where the channel read values are put.
		*/
		void fastScan(bool mode, int16_t *buffer);
		
		//! This function scans all channels conversion into a buffer.
		/*!
		* \param mode: '0' for differential mode, '1' for single-ended
		* \param buffer: an array where the channel read values are put.
		*/
		void fastScan(bool mode, int16_t *buffer, bool raw);

};

extern MAX11609 MAX;

#endif



