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

/*

Sketchbook location
/home/onsar/git/onsar/ecopower/firmware/iotpower_serial/iotpow_serial


*/



#define DEBUG 0

#define BPS 115200 //Velocidad de las comunicaciones

#ifdef DEBUG == 1
  int freeRam(){
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  }
#endif

/*
#include <SoftwareSerial.h>
#define RXPIN 6
#define TXPIN 5
SoftwareSerial wifiSerialInit (RXPIN,TXPIN);
*/

/* D12 MISO  -> e_rx (D5)
 * D11 MOSI  -> e_tx (D6)
 */




#include <SoftwareSerial.h>
#define RXPIN 12    //11
#define TXPIN 11    //12
SoftwareSerial wifiSerialInit (RXPIN,TXPIN);


#include "power_sensor.h"

uint32_t t_last_tx=0;
uint32_t t_last_measurement=0;

void setup(void){     
  Serial.begin(BPS);
  wifiSerialInit.begin(BPS);
}

void loop(void){

  uint32_t current_time= millis();

  // en prevision de que el contador de tiempo de la vuelta
  if (current_time < t_last_tx) t_last_tx=0; 
  if (current_time - t_last_tx > 59000){  
    t_last_tx = current_time;
    if(DEBUG){
      Serial.print(F("******Print LCD - sgs: "));
      Serial.println(millis() / 1000);
      Serial.print("freeRam: "); Serial.println(freeRam());   
      Serial.flush();    
    }
    buildPowerMessage(0);
  }
  
  // en prevision de que el contador de tiempo de la vuelta
  if (current_time < t_last_measurement) t_last_measurement=0;
  // Medida de potencia cada 200ms
  if (current_time - t_last_measurement > 200){     
    t_last_measurement = current_time;
    calculate_power();
    }
  }
  
