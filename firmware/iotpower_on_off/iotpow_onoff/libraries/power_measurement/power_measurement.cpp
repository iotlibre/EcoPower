/*!
 *  @file 		power_measurement.cpp 
 *  @version	1.0
 * 
 * Power_measurement.cpp library for Prometeo project
 * Created by Alejandro July 27 2017
 * Based on calcVI function of Emon library from openenergymonitor
 * GNU GPL
*/

#include "power_measurement.h"
#include <MAX11609.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


/* Function: 	Sets the calibration values of voltage and current sensors
 * Parameters:	_VCAL: calibration value for voltage sensor
 *				_PHASECAL: voltage phase adjustment. 1 for no adjustment
 *				_I1: calibration value for current sensor on socket 1
 *				_I2: calibration value for current sensor on socket 2
 *				_I3: calibration value for current sensor on socket 3
 * Return: 		nothing
 */
void Power_measurement::config( float _VCAL, float _PHASECAL, float _I1, float _I2, float _I3)
{
	VCAL = _VCAL;
	PHASECAL1 = _PHASECAL;
	PHASECAL2 = _PHASECAL;
	PHASECAL3 = _PHASECAL;
	I1 = _I1;
	I2 = _I2;
	I3 = _I3;
  
    MAX.begin(REF_EXTERNAL, 3300);
}

/* Function: 	Sets the calibration values of voltage and current sensors
 * Parameters:	_VCAL: calibration value for voltage sensor
 *				_PHASECAL1: voltage phase 1 adjustment. 1 for no adjustment
 *				_PHASECAL2: voltage phase 2 adjustment. 1 for no adjustment
 *				_PHASECAL3: voltage phase 3 adjustment. 1 for no adjustment
 *				_I1: calibration value for current sensor on socket 1
 *				_I2: calibration value for current sensor on socket 2
 *				_I3: calibration value for current sensor on socket 3
 * Return: 		nothing
 */
void Power_measurement::config( float _VCAL, float _PHASECAL1, float _PHASECAL2, float _PHASECAL3, float _I1, float _I2, float _I3)
{
	VCAL = _VCAL;
	PHASECAL1 = _PHASECAL1;
	PHASECAL2 = _PHASECAL2;
	PHASECAL3 = _PHASECAL3;
	I1 = _I1;
	I2 = _I2;
	I3 = _I3;
  
    MAX.begin(REF_EXTERNAL, 3300);
}


/* Function: 	Calculates realPower, apparentPower, powerFactor, Vrms and Irms 
 * 				for the 3 sockets. Based on the same name function from emonlib 
 * 				library
 * Parameters:	crossings: number of half wavelengths to measure
 *				timeout: timeout value in milliseconds
 * Return: 		-1 if no zero-crossing detected, 0 if all ok
 */
int8_t Power_measurement::calcVI(unsigned int crossings, unsigned int timeout)
{

	int16_t lectura[4];
	float lectura_f[4];
	unsigned int crossCount = 0; 		//Used to measure number of times threshold is crossed.
	unsigned int numberOfSamples = 0;	//This is now incremented
	
	int16_t startV; 					//Instantaneous voltage at start of sample window.
	boolean lastVCross, checkVCross;	//Used to measure number of times threshold is crossed.

	double lastphaseShiftedV[3];		//Filtered_ is the raw analog value minus the DC offset
	double filteredI;


	float phaseShiftedV[3];				//Holds the calibrated phase shifted voltage.

	float sqV,sumV,sqI[3],sumI[3],instP[3],sumP[3];	//sq = squared, sum = Sum, inst = instantaneous
	
	uint32_t tempo;
	
	
	//Reset accumulators
	sumV = 0;
	sumI[0] = 0;
	sumI[1] = 0;
	sumI[2] = 0;
	sumP[0] = 0;
	sumP[1] = 0;
	sumP[2] = 0;
	
	//--------------------------------------------------------------------------
	// 1) Waits for the waveform to be close to 'zero' part in sin curve.
	//--------------------------------------------------------------------------
	
	boolean st = false; 						//an indicator to exit the while loop
	MAX.fastConfig(DIFFERENTIAL_MODE, CH_0_1); 	// Configs the MAX11609 to read the voltage sensor channel
	unsigned long start = millis();   

	while(st==false)                                   //the while loop...
	{
		startV = MAX.fastRead(DIFFERENTIAL_MODE);      //using the voltage waveform
		if ((startV < (20)) && (startV > (-20)))
		{
			st = true;  //check its within range
		}
		if ((millis()-start)>timeout)
		{
			return -1;
		}
	}
	
        
	//--------------------------------------------------------------------------
	// 2) Main measurement loop
	//--------------------------------------------------------------------------
	
	MAX.fastConfig(DIFFERENTIAL_MODE, ALL_CHANNELS); // Configs the MAX11609 to read all channels
  
	lastphaseShiftedV[0] = lectura[0]; 				//Used for delay/phase compensation
	lastphaseShiftedV[1] = lectura[0]; 				//Used for delay/phase compensation
	lastphaseShiftedV[2] = lectura[0]; 				//Used for delay/phase compensation
	start = millis();
	while ((crossCount < crossings) && ((millis()-start)<timeout))
	{
		tempo = micros();
		numberOfSamples++;							//Count number of times looped.

		//-----------------------------------------------------------------------------
		// A) Read in raw voltage and current samples
		//-----------------------------------------------------------------------------
		MAX.fastScan(DIFFERENTIAL_MODE, lectura);	// ~290-292us to read 4 differential channels and ~10-12us to perform all maths
		lectura_f[0] = lectura[0];
		lectura_f[0] *= VCAL;
		lectura_f[1] = lectura[1];
		lectura_f[1] *= I1;
		lectura_f[2] = lectura[2];
		lectura_f[2] *= I2;
		lectura_f[3] = lectura[3];
		lectura_f[3] *= I3;

		//-----------------------------------------------------------------------------
		// C) Root-mean-square method voltage
		//-----------------------------------------------------------------------------
		sqV= lectura_f[0] * lectura_f[0];			//1) square voltage values
		sumV += sqV;								//2) sum

		//-----------------------------------------------------------------------------
		// D) Root-mean-square method current
		//-----------------------------------------------------------------------------
		sqI[0] = lectura_f[1] * lectura_f[1];		//1) square current values
		sqI[1] = lectura_f[2] * lectura_f[2];		//1) square current values
		sqI[2] = lectura_f[3] * lectura_f[3];		//1) square current values
		sumI[0] += sqI[0];							//2) sum
		sumI[1] += sqI[1];							//2) sum
		sumI[2] += sqI[2];							//2) sum

		//-----------------------------------------------------------------------------
		// E) Phase calibration
		//-----------------------------------------------------------------------------
		phaseShiftedV[0] = phaseShiftedV[0] + PHASECAL1 * (lectura[0] - phaseShiftedV[2]);
		phaseShiftedV[1] = phaseShiftedV[1] + PHASECAL2 * (phaseShiftedV[0] - phaseShiftedV[1]);
		phaseShiftedV[2] = phaseShiftedV[2] + PHASECAL3 * (phaseShiftedV[1] - phaseShiftedV[2]);

		//-----------------------------------------------------------------------------
		// F) Instantaneous power calc
		//-----------------------------------------------------------------------------
		instP[0] = phaseShiftedV[0] * lectura_f[1];	//Instantaneous Power
		instP[1] = phaseShiftedV[1] * lectura_f[2];	//Instantaneous Power
		instP[2] = phaseShiftedV[2] * lectura_f[3];	//Instantaneous Power
		sumP[0] += instP[0];						//Sum
		sumP[1] += instP[1];						//Sum
		sumP[2] += instP[2];						//Sum

		//-----------------------------------------------------------------------------
		// G) Find the number of times the voltage has crossed the initial voltage
		//    - every 2 crosses we will have sampled 1 wavelength
		//    - so this method allows us to sample an integer number of half wavelengths which increases accuracy
		//-----------------------------------------------------------------------------
		lastVCross = checkVCross;
		if (lectura[0] > startV)
		{
			 checkVCross = true;
		}
		else
		{
			checkVCross = false;
		}
		if (numberOfSamples==1)
		{
			lastVCross = checkVCross;
		}

		if (lastVCross != checkVCross)
		{
			crossCount++;
		}
		
		
	}

	
	//--------------------------------------------------------------------------
	// 3) Post loop calculations
	//--------------------------------------------------------------------------
	//Calculation of the root of the mean of the voltage and current squared (rms)

	Vrms = sqrt(sumV / numberOfSamples);

	Irms[0] = sqrt(sumI[0] / numberOfSamples);
	Irms[1] = sqrt(sumI[1] / numberOfSamples);
	Irms[2] = sqrt(sumI[2] / numberOfSamples);

	//Calculation power values
	realPower[0] = sumP[0] / numberOfSamples;
	apparentPower[0] = Vrms * Irms[0];
	powerFactor[0] = realPower[0] / apparentPower[0];
	if (powerFactor[0] > 1)
	{
		powerFactor[0] = 1;
	}
	
	realPower[1] = sumP[1] / numberOfSamples;
	apparentPower[1] = Vrms * Irms[1];
	powerFactor[1] = realPower[1] / apparentPower[1];
	if (powerFactor[1] > 1)
	{
		powerFactor[1] = 1;
	}
	realPower[2] = sumP[2] / numberOfSamples;
	apparentPower[2] = Vrms * Irms[2];
	powerFactor[2] = realPower[2] / apparentPower[2];
	if (powerFactor[2] > 1)
	{
		powerFactor[2] = 1;
	}

	return 0;
	
}

/* Function: 	Prints data from one or all sockets
 * Parameters:	socket: data to print from selected socket
 * Return:		nothing
 */
void Power_measurement::serialprint(uint8_t socket)
{
	if( (socket & SOCKET1) != 0)
	{
		Serial.print("SOCKET 1 ==> Vrms: ");
		Serial.print(Vrms);
		Serial.print(" V || Irms: ");
		Serial.print(Irms[0]);
		Serial.print(" A || P: ");
		Serial.print(realPower[0]);
		Serial.print(" W || S: ");
		Serial.print(apparentPower[0]);
		Serial.print(" VA || PF: ");
		Serial.print(powerFactor[0]);
		Serial.println("");
	}	
	
	if( (socket & SOCKET2) != 0)
	{
		Serial.print("SOCKET 2 ==> Vrms: ");
		Serial.print(Vrms);
		Serial.print(" V || Irms: ");
		Serial.print(Irms[1]);
		Serial.print(" A || P: ");
		Serial.print(realPower[1]);
		Serial.print(" W || S: ");
		Serial.print(apparentPower[1]);
		Serial.print(" VA || PF: ");
		Serial.print(powerFactor[1]);
		Serial.println("");
	}	
	
	if( (socket & SOCKET3) != 0)
	{
		Serial.print("SOCKET 3 ==> Vrms: ");
		Serial.print(Vrms);
		Serial.print(" V || Irms: ");
		Serial.print(Irms[2]);
		Serial.print(" A || P: ");
		Serial.print(realPower[2]);
		Serial.print(" W || S: ");
		Serial.print(apparentPower[2]);
		Serial.print(" VA || PF: ");
		Serial.print(powerFactor[2]);
		Serial.println("");
	}
	delay(100);
}
