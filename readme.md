# EcoPower

<img src="https://github.com/iotlibre/EcoPower/blob/master/docs/ecopower_equipo.png" align="right">
Ecopower está pensado para tomar las medidas de consumo eléctrico de una vivienda o de un edificio con consumos inferiores a 30 KW. El sistema permite diferenciar varias zonas.

EcoPower se puede instalar en el cuadro eléctrico principal o en cualquier otra parte que nos permita tomar datos del consumo mediante pinzas (transformadores de intensidad).

En el un edificio podemos tener tantos dispositivos como sean necesarios y el servidor los integrará en la misma instalación sin necesidad de configuraciones adicionales o cambios en el Firmware.

La comunicación se hace mediante Wifi por lo que no es necesario el cableado de datos al equipo solamente es preciso que esté dentro de la cobertura de la red WIFI del edificio.

Para su uso no son necesarios conocimientos de informática aunque todos los desarrollos y el hardware están hechos bajo licencias libres lo que permite la modificación y mejora de las funcionalidades.

## Listado con las principales características
Las principales características de EcoPower son:
- 6 entradas para sensores de corriente con salida estandar de 50 mA
- 1 entrada de medida de voltage 200 - 440 Voltios
- Precisión en las medida de energía: ± 1,2%
- Comunicación a Internet por WIFI local 
- Configurable vía Web
- Estandard: IEEE 802.11 b/g/n
- Alimentación a 220 voltios de corriente alterna. Rango: 85 ~ 264VAC 
- Montaje en carril din
- La PCB integra un Arduino nano con el ESP8266 12E
- Compatible con el [servidor de Open Energy Monitor](https://emoncms.org/)

## Puesta a punto
La puesta a punto de EcoPower consta de dos partes:
- La configuración en la instalación
- El firmware de Arduino

### La configuración en la instalación
En este punto se definen los parámetros del servidor de destino y la wifi local a la que va a estar conectado el dispositivo. Con este fin, la primera vez que se ponga en servicio el ESP y siempre que no encuentre la WIFI configurada, el ESP 8266 12E creará su propio punto de acceso, su propia red WIFI . Conectandose a cualquier dirección a través de este punto de acceso nos aparecerá la página de configuración del ESP. Tengase en cuenta que una  vez configurado el ESP y conectado a una red WIFI el router le asignará una única dirección IP a la que será necesario acceder para cambiar la configuración.

En caso de que se quiera actualizar el firmware del ESP a la última versión, este se puede encontrar en: [EcoPower-emonESP ](https://github.com/iotlibre/20180328_EmonESP)

### El firmware de Arduino
El firmware que que esta cargado por defecto en el arduino nano funciona correctamente para tranformadores de intensidad de 100A/50mA sin embargo, si quiere cambiar la configuración puede encontrar la última actualización del firmware en este repositorio:[EcoPower-iotpow_serial ](https://github.com/iotlibre/EcoPower/tree/master/firmware/iotpower_serial/iotpow_serial)

## Source code
Copyright 2020 Endef Engineering SL.

Tanto el hardware como el software de este repositorio han sido desarrollados por la empresa: [Endef Engineering Sl](https://endef.com)

## Licencia
* Los diseños de hardware estan disponibles bajo licencia "Creative Commons Attribution-ShareAlike 3.0 Unported License.".
* El firmware se publica bajo la licencia GNU GPL V3 La documentación está sujeta a la Licencia "GNU Free Documentation"
* Los diseños de hardware siguen los términos de la Declaración de principios 1.0 de OSHW (Open-source hardware).

## Disclaimer
Todo el código que se encuentra en este repositorio está bajo la licencia: "GNU V3 General Public License" por lo que las condiciones de su uso son las incluidas en esta licencia

NUESTROS PRODUCTOS Y JUEGOS DE MONTAJE PUEDEN SER UTILIZADOS POR USUARIOS EXPERIMENTADOS Y CAPACITADOS BAJO SU PROPIO RIESGO. EN LA MEDIDA MÁXIMA QUE LO PERMITA LA LEY APLICABLE, POR EL PRESENTE RENUNCIAMOS CUALQUIER RESPONSABILIDAD, RIESGO Y DAÑOS DERIVADOS DE LA MUERTE O LESIONES PERSONALES RESULTANTES DEL MONTAJE O FUNCIONAMIENTO DE NUESTROS PRODUCTOS.

Su seguridad es su propia responsabilidad, incluido el uso adecuado del equipo y el equipo de seguridad, y determinar si tiene la habilidad y la experiencia adecuadas. Endef Engineering se exime de toda responsabilidad por cualquier daño, lesión o gasto resultante. Es su responsabilidad asegurarse de que sus actividades cumplan con las leyes aplicables, incluidos los derechos de autor. Puede haber actualizaciones o correcciones importantes. Todo uso de las instrucciones, kits, proyectos y sugerencias dadas por Endef Ingeniería es bajo su propio riesgo. La tecnología (hardware, firmware y software) cambia constantemente, es posible que la documentación (incluidas las instrucciones y la guía de compilación) no esté completa o sea correcta.
