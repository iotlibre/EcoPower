/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#ifndef power_sensor_h
#define power_sensor_h

// *******************************************************
// ******** CONFIGURACION SENSORES DE POTENCIA    ********
// *******************************************************

// 97.6  66.5  51.3(39.0 ohm)  20.0 (100.0 ohm)

#define NUMBER_OF_PWR_SENSORS 4
#define TRANSFORMATOR_CONSTANT 24

#define MAME_PWR_1 "s1"
#define ENTER_1 A0
#define CURRENT_CONST_1 TRANSFORMATOR_CONSTANT

#define MAME_PWR_2 "s2"
#define ENTER_2 A1
#define CURRENT_CONST_2 TRANSFORMATOR_CONSTANT

#define MAME_PWR_3 "s3"
#define ENTER_3 A2
#define CURRENT_CONST_3 TRANSFORMATOR_CONSTANT

#define MAME_PWR_4 "s4"
#define ENTER_4 A3
#define CURRENT_CONST_4 TRANSFORMATOR_CONSTANT

#define MAME_PWR_5 "p5"
#define ENTER_5 A4
#define CURRENT_CONST_5 TRANSFORMATOR_CONSTANT

#define MAME_PWR_6 "p6"
#define ENTER_6 A5
#define CURRENT_CONST_6 TRANSFORMATOR_CONSTANT

#define MAME_PWR_7 "v1"
#define ENTER_7 A6
#define CURRENT_CONST_7 180.2


// Falta la medida del voltage

// *******************************************************

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;

/*
// variable declaration
// long 2,147,483,647 (with sing)
// 240 x 100 x 60 seg x 5 medias
// 7,200,000
*/

typedef struct {                                                      // RFM RF payload datastructure
  String name_input;
  int pin_input;
  float constant_input;
  long power_sum;
  long power_measurements;
} analog_input;

analog_input p1={MAME_PWR_1,ENTER_1,CURRENT_CONST_1,0,0};
analog_input p2={MAME_PWR_2,ENTER_2,CURRENT_CONST_2,0,0};
analog_input p3={MAME_PWR_3,ENTER_3,CURRENT_CONST_3,0,0};
analog_input p4={MAME_PWR_4,ENTER_4,CURRENT_CONST_4,0,0};
analog_input p5={MAME_PWR_5,ENTER_5,CURRENT_CONST_5,0,0};
analog_input p6={MAME_PWR_6,ENTER_6,CURRENT_CONST_6,0,0};
analog_input v1={MAME_PWR_7,ENTER_7,CURRENT_CONST_7,0,0};


analog_input analog_inputs[]={p1,p2,p3,p4,p5,p6,v1};


// Function Prototypes
void configure_output();
void calculate_power();
void buildPowerMessage(uint8_t);

/* 
 *  configurar las salidas auxiliares para medir tiempos
 *  en los que un relé esta activo
*/
void configure_output(){ 
  pinMode(analog_inputs[4].pin_input, OUTPUT); 
  digitalWrite(analog_inputs[4].pin_input, LOW);
  pinMode(analog_inputs[5].pin_input, OUTPUT); 
  digitalWrite(analog_inputs[5].pin_input, LOW);
}

void calculate_power(){

  // uint32_t millis_init =millis();
 
  emon1.current(analog_inputs[6].pin_input,analog_inputs[6].constant_input); // calibration.
  double Vrms = emon1.calcIrms(141);  // Calculate Irms only, (119 20ms for algoritm )
  // Serial.println(millis()-millis_init);
  if(Vrms < 40) Vrms = 1;  // para evitar numeros muy elevados
  analog_inputs[6].power_sum+= Vrms; 
  analog_inputs[6].power_measurements++ ;    

  

  
  // millis_init =millis();
  for (uint8_t i=0; i < NUMBER_OF_PWR_SENSORS; i++){     
    emon1.current(analog_inputs[i].pin_input,analog_inputs[i].constant_input); // calibration.
    double Irms = emon1.calcIrms(141);  // Calculate Irms only, (119 20ms for algoritm )
    analog_inputs[i].power_sum+=(Irms*Vrms); 
    analog_inputs[i].power_measurements++ ;    
  }
  // Serial.println(millis()-millis_init);
}

void buildPowerMessage(uint8_t output){
  uint32_t millis_init =millis();
  for (uint8_t i=0; i < NUMBER_OF_PWR_SENSORS; i++){ 
    long power_result = analog_inputs[i].power_sum/analog_inputs[i].power_measurements;     
    if (output==0){
      if(DEBUG){
        Serial.print ("name_input: ");          Serial.println(analog_inputs[i].name_input);       
        Serial.print ("power_sum: ");           Serial.println (analog_inputs[i].power_sum); 
        Serial.print ("power_measurements: ");  Serial.println (analog_inputs[i].power_measurements);
        Serial.println (analog_inputs[i].name_input+ ":" + power_result); 
        Serial.flush();     
      }
      wifiSerialInit.println (analog_inputs[i].name_input+ ":" + power_result);
      analog_inputs[i].power_sum = 0;
      analog_inputs[i].power_measurements = 0;
      
    }
  }
  if(DEBUG){Serial.println(millis()-millis_init);}
}

#endif
