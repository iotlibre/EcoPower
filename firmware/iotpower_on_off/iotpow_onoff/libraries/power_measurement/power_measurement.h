/*!
 *  @file 		power_measurement.h 
 *  @version	1.0
 * 
 * Power_measurement.h library for Prometeo project
 * Created by Alejandro July 27 2017
 * Based on Emon library from openenergymonitor
 * GNU GPL
*/

#ifndef power_measurement_h
#define power_measurement_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <MAX11609.h>

// Socket defines for serialprint function
#define SOCKET1 1
#define SOCKET2 2
#define SOCKET3 4
#define SOCKETALL 7

class Power_measurement
{

	private:

		//Calibration coefficients
		//These need to be set in order to obtain accurate results
		float VCAL;
		float I1;
		float I2;
		float I3;
		
	public:
		float PHASECAL1;
		float PHASECAL2;
		float PHASECAL3;
		
		//! This function sets the calibration values of voltage and current sensors
		/*!
		\param float _VCAL: calibration value for voltage sensor
		\param float _PHASECAL: voltage phase adjustment. 1 for no adjustment
		\param float _I1: calibration value for current sensor on socket 1
		\param float _I2: calibration value for current sensor on socket 2
		\param float _I3: calibration value for current sensor on socket 3
		\return		nothing
		*/
		void config( float _VCAL, float _PHASECAL, float _I1, float _I2, float _I3);
		
		//! This function sets the calibration values of voltage and current sensors
		/*!
		\param float _VCAL: calibration value for voltage sensor
		\param float _PHASECAL1: voltage phase 1 adjustment. 1 for no adjustment
		\param float _PHASECAL2: voltage phase 2 adjustment. 1 for no adjustment
		\param float _PHASECAL3: voltage phase 3 adjustment. 1 for no adjustment
		\param float _I1: calibration value for current sensor on socket 1
		\param float _I2: calibration value for current sensor on socket 2
		\param float _I3: calibration value for current sensor on socket 3
		\return		nothing
		*/
		void config( float _VCAL, float _PHASECAL1, float _PHASECAL2, float _PHASECAL3, float _I1, float _I2, float _I3);

		
		//! This function calculates realPower, apparentPower, powerFactor, Vrms and Irms for the 3 sockets. Based on the same name function from emonlib library
		/*!
		\param unsigned int crossings: number of half wavelengths to measure
		\param unsigned int timeout: timeout value in milliseconds
		\return	-1 if no zero-crossing detected, 0 if all ok
		*/
		int8_t calcVI(unsigned int crossings, unsigned int timeout);
		
		
		//! This function prints data from the selected sockets
		/*!
		\param uint8_t socket: sockets selected to show their data
		\return		nothing
		*/
		void serialprint(uint8_t socket);

		//Useful value variables
		float realPower[3]; 		// P (W)
		float apparentPower[3];		// S (VA)
		float powerFactor[3];
		float Vrms;					// (V)
		float Irms[3];				// (A)


};

#endif
