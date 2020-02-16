// Slave


/*
 * Descubrimiento de esclavos
 * ===============================

 * estado = 0 inicio de la comunicacion       (sensores?)               B0   55   An  
 * estado = 1 confirmacion esclavo preparado  (final de comunicacion)   B8   An   55  
 * estado = 2 comprobacion, final de comunicacion                       B8   55   55
 * estado = 3 espera entre comuniaciones                                                            
 */


 /*
 * diagrama de estados del servidor
 * ===============================

 * estado = 0 inicio de la comunicacion       (sensores?)               B0   55   An  
 * estado = 1 confirmacion, cliente preparado (siguente registro)       B1   An   r1  
 * estado = 2 recepción de registros          (siguente registro)       B1   r1   r2
 *                                            (siguente registro)       B1   r2   r3
 *                                            (siguente registro)       B1   r3   rn
 *                                            (suma de registros)       B1   rn   suma
 * estado = 3 comprabar la trama recibida     (final de comunicacion)   B8   suma 55
 *                                                              
 * estado = 4 procesando y transmision de valores
 *            primer valor el de menor peso           registros_recibidos[0]
 *            segundo valor recibido el de mas peso   registros_recibidos[1]
 * Estado = 5 espera entre comuniaciones
 */
 
#define DEBUG 1

#define DATOS 0x06

#include <SPI.h>

uint8_t datos_matrix[] = {0xC1,0xC2,0xC3,0xC4,0xC5,0xC6};
uint8_t datos_pendientes;

uint32_t t_last_tx;

void setup (void)
{
  Serial.begin(9600);

  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= bit(SPE);

  // turn on interrupts
  SPCR |= bit(SPIE);

  SPDR = 0x00;
  datos_pendientes = 0;

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect){
  byte c = SPDR;
  
  if(c==0xB0){ // inicio  
    datos_pendientes = DATOS;
    SPDR = 0xA0 | DATOS;
    if(DEBUG) {
      Serial.print("bit recibido =  "); Serial.println(c,HEX);
      Serial.print("datos_pendientes =  "); Serial.println(datos_pendientes);
    }
  }

  if(c==0xB1){// transmision iniciada
    
    if(datos_pendientes > 0) {
      SPDR = datos_matrix[DATOS - datos_pendientes];  // 4-4=0, 4-3=1, 4-2=2, 4-1=3,
      datos_pendientes = datos_pendientes-1;
      if(DEBUG) {
        Serial.print("bit recibido =  "); Serial.println(c,HEX);
        Serial.print("datos_pendientes =  "); Serial.println(datos_pendientes);
      }
    }
    else{
      uint8_t registros_suma  = sumar_registros();
      SPDR = registros_suma;
      datos_pendientes = 0;
      if(DEBUG) {
        Serial.print("bit recibido =  "); Serial.println(c,HEX);
        Serial.print("registros_suma:  ");Serial.println(registros_suma,HEX);
      }
    }  
  }

  if(c==0xB8){ // reset   
    SPDR = 0x55;
    datos_pendientes = 0;
    if(DEBUG) {
      Serial.print("bit recibido =  "); Serial.println(c,HEX);
      Serial.println(" ***reset  ");
    }
  }

}  // end of interrupt service routine (ISR) SPI_STC_vect




void loop (void)
{
  /*
  uint32_t current_time= millis();
     
  if ((current_time - t_last_tx) > 10000)
    {      
      Serial.print(F("current_time - sgs: ")); Serial.println(millis() / 1000);
      t_last_tx = current_time;    
      datos_tx = DATOS;
      SPDR = 0xF0 | DATOS;
    }


*/
}  // end of loop

uint8_t sumar_registros(){
  uint8_t suma = 0;
  for(uint8_t n=0; n < DATOS; n++){
    suma = suma + datos_matrix[n];
  }
  Serial.print("suma: "); Serial.println(suma,HEX);
  return suma;
}


  
