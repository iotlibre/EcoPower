/*!
 *  @file 		MAX11609.cpp 
 *  @version	1.0
 * 
 * MAX11609.cpp library for Prometeo project
 * Created by Alejandro July 27 2017
 * GNU GPL
*/

#include "MAX11609.h"
#include <Arduino.h>
#include <Wire.h>


/* Function:	Sets up the MAX11609 with a default setup and selected Vref
 * Parameters:	vRef: vref source selection
 * Return:		nothing
 */
void MAX11609::begin(uint8_t vRef)
{
	begin(vRef, 2048);
}

/* Function:	Sets up the MAX11609 with a default setup and selected Vref
 * Parameters:	vRef: vref source selection
 *				voltage: voltage value (in millivolts) used by the ADC
 * Return:		nothing
 */
void MAX11609::begin(uint8_t vRef, int16_t voltage)
{
	// Inits I2C bus
    Wire.begin();
    Wire.setClock(800000);

    // Basic setup Vref, internal clock and unipolar voltage reference
    // It also generates a reset in config register
    setupADC((vRef << SEL_0_BIT) & 0xF0); 
    
    // Stores the voltage reference
    voltage_ref = voltage;

}

/* Function: 	Writes the setup byte
 * Parameters:	data: the setup byte to write
 * Return:		nothing
 */
void MAX11609::setupADC(uint8_t data)
{
	setup_reg = data | 0x80; // Bit 7 must be '1' for setup register
    Wire.beginTransmission(MAX11609_ADDR);
    Wire.write(setup_reg); 
    Wire.endTransmission();
}

/* Function: 	Writes the configuration byte
 * Parameters:	data: the configuration byte to write  
 * Return:		nothing
 */
void MAX11609::configuration(uint8_t data)
{
    Wire.beginTransmission(MAX11609_ADDR);
    Wire.write(data & (~0x80));  // Bit 7 must be '0' for config register
    Wire.endTransmission();

}

/* Function: 	Reads one channel
 * Parameters:	channel: The channel to convert or read
 *				mode: '0' for differential mode, '1' for single-ended
 * Return:		uint16_t The conversion result or 32767 if there's an 
 *         error.
 */
int16_t MAX11609::read(bool mode, uint8_t channel)
{
	return read( mode, channel, 1);
}

/* Function: 	Reads one channel
 * Parameters:	channel: The channel to convert or read
 *				mode: '0' for differential mode, '1' for single-ended
 *				raw: '0' for raw measurement, '1' for processed voltage
 * Return:		int16_t The conversion result or 32767 if there's an error.
 */
int16_t MAX11609::read(bool mode, uint8_t channel, bool raw)
{

    int16_t result = 0x0000;
    
    // setup UNIPOLAR or BIPOLAR reference
    if (mode == SINGLE_ENDED_MODE)
    {
		setup_reg &= 0xFB;
	}
	else
	{
		setup_reg |= 0x04;
	}	
	setupADC(setup_reg);
	
	// Config channel and mode
	uint8_t configurationByte = (ONLY_SELECTED << SCAN_0_BIT) | (channel << CHANNEL_0_BIT) | mode;
	configuration(configurationByte);
	
	// Read 2 bytes
    Wire.requestFrom(MAX11609_ADDR, 2);

    if(Wire.available() == 2) // the conversion consists of two bytes per channel
    {
        result = (Wire.read() & 0x0003) << 8; 	// MSB is returned first. [7-2] are high.  
        result |= Wire.read() & 0x00ff; 		// read LSB   
        
        // Differential mode works with bipolar reference, conversion required
        if ((mode == DIFFERENTIAL_MODE) && ((result & 0x0200) != 0))
        {
			result |= 0xFC00;
		}
        
        // Convert to millivolts if necesary
        if (raw == 1)
        {
			result = float(voltage_ref / 1024) * result;
		}

        return result;
    }
    else
    {
        return 32767; // ERROR
    }

}

/* Function: 	Scans all channels conversion into a buffer.
 * Parameters:	mode: '0' for differential mode, '1' for single-ended
 *				buffer: an array where the channel read values are put.
 * Return:		nothing
 */
void MAX11609::scan(bool mode, int16_t *buffer)
{
	scan(mode, buffer, 1);
}

/* Function: 	Scans all channels conversion into a buffer.
 * Parameters:	mode: '0' for differential mode, '1' for single-ended
 *				buffer: an array where the channel read values are put.
 *				raw: '0' for raw measurement, '1' for processed voltage
 * Return:		nothing
 */
void MAX11609::scan(bool mode, int16_t *buffer, bool raw)
{
	uint8_t bytes_requested = 16;

	// setup UNIPOLAR or BIPOLAR reference
    if (mode == SINGLE_ENDED_MODE)
    {
		setup_reg &= 0xFB;
	}
	else
	{
		setup_reg |= 0x04;
		bytes_requested = 8; // In diffrential mode there are only 4 "channels"
	}	
	setupADC(setup_reg);
	
	// Config channel and mode
    uint8_t configurationByte = (CH_0_TO_SELECTED << SCAN_0_BIT) | (CH_7 << CHANNEL_0_BIT) | mode;
    configuration(configurationByte);

	// Read bytes_requested
    Wire.requestFrom((uint8_t)MAX11609_ADDR, bytes_requested);

    if(Wire.available() == bytes_requested)
    {
        for(uint8_t i = 0; i < (bytes_requested / 2); i++) // read all channels
        {
            *(buffer+i) = (Wire.read() & 0x03) << 8;	// MSB is returned first. [7-2] are high.
            *(buffer+i) |= Wire.read() & 0x00FF; 		// read LSB        
            
            // Differential mode works with bipolar reference, conversion required
			if ((mode == DIFFERENTIAL_MODE) && ((buffer[i] & 0x0200) != 0))
			{
				buffer[i] |= 0xFC00;
			}
			
			// Convert to millivolts if necesary
			if (raw == 1)
			{
				buffer[i] = float(voltage_ref / 1024) * buffer[i];
			}
                   
        }
    }
}

/* Function: 	Sets up the MAX11609 to work with fast reads and scans
 * Parameters:	mode: '0' for differential mode, '1' for single-ended
 *				channel: the channel to convert or read
 * Return:		nothing
 */
void MAX11609::fastConfig(bool mode, uint8_t channel)
{
	// setup UNIPOLAR or BIPOLAR reference
	if (mode == SINGLE_ENDED_MODE)
    {
		setup_reg &= 0xFB;
	}
	else
	{
		setup_reg |= 0x04;
	}	
	setupADC(setup_reg);
	
	// Config channel and mode
	if (channel == ALL_CHANNELS)
	{
		//scan config
		configuration((CH_0_TO_SELECTED << SCAN_0_BIT) | (CH_7 << CHANNEL_0_BIT) | mode);
	}
	else
	{
		configuration((ONLY_SELECTED << SCAN_0_BIT) | (channel << CHANNEL_0_BIT) | mode);
	}
}

/* Function: 	Reads the channel selected with fastConfig
 * Parameters:	mode: '0' for differential mode, '1' for single-ended
 * Return:		int16_t The conversion result or 32767 if there's an error.
 */
int16_t MAX11609::fastRead(bool mode)
{
	return fastRead(mode, 1);
}

/* Function: 	Reads the channel selected with fastConfig
 * Parameters:	mode: '0' for differential mode, '1' for single-ended
 *				raw: '0' for raw measurement, '1' for processed voltage
 * Return:		int16_t The conversion result or 32767 if there's an error.
 */
int16_t MAX11609::fastRead(bool mode, bool raw)
{	
    int16_t result = 0x0000;
   
    // Read 2 bytes
	Wire.requestFrom(MAX11609_ADDR, 2);

    if(Wire.available() == 2) // the conversion consists of two bytes per channel
    {
        result = (Wire.read() & 0x0003) << 8;	// MSB is returned first. [7-2] are high.  
        result |= Wire.read() & 0x00ff;			// read LSB   
        
        // Differential mode works with bipolar reference, conversion required
        if ((mode == DIFFERENTIAL_MODE) && ((result & 0x0200) != 0))
        {
			result |= 0xFC00;
		}
        
        // Convert to millivolts if necesary
        if (raw == 1)
        {
			result = float(voltage_ref / 1023) * result;
		}

        return result;
    }
    else
    {
        return 32767; // ERROR
    }
}

/* Function: 	Scans all channels conversion into a buffer.
 * Parameters:	mode: '0' for differential mode, '1' for single-ended
 *				buffer: an array where the channel read values are put.
 * Return:		nothing
 */
void MAX11609::fastScan(bool mode, int16_t *buffer)
{
	fastScan(mode, buffer, 1);
}

/* Function: 	Scans all channels conversion into a buffer.
 * Parameters:	mode: '0' for differential mode, '1' for single-ended
 *				buffer: an array where the channel read values are put.
 *				raw: '0' for raw measurement, '1' for processed voltage
 * Return:		nothing
 */
void MAX11609::fastScan(bool mode, int16_t *buffer, bool raw)
{

	uint8_t bytes_requested = 16;

	if (mode == DIFFERENTIAL_MODE)
	{
		bytes_requested = 8;
	}
	
	// Read bytes_requested
    Wire.requestFrom((uint8_t)MAX11609_ADDR, bytes_requested); 

    if(Wire.available() == bytes_requested)
    {
        for(uint8_t i = 0; i < (bytes_requested / 2); i++) // read all channels [AIN0-AIN7]
        {

            *(buffer+i) = (Wire.read() & 0x03) << 8;	// MSB is returned first. [7-2] are high.
            *(buffer+i) |= Wire.read() & 0x00FF;		// read LSB  
                  
            // Differential mode works with bipolar reference, conversion required
			if ((mode == DIFFERENTIAL_MODE) && ((buffer[i] & 0x0200) != 0))
			{
				buffer[i] |= 0xFC00;
			}
			
			// Convert to millivolts if necesary
			if (raw == 1)
			{
				buffer[i] = float(voltage_ref / 1024) * buffer[i];
            }     
        }
    }
}

MAX11609 MAX = MAX11609();


