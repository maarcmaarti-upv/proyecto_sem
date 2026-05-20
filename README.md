# proyecto_sem
Códigos coche control remoto con velocímetro calculado usando PCNT.

Hay dos carpetas, una para el coche y otra para el mando. Ya que para el proyecto son necesarias 2 esp32s3.

En el main del mando envía vía mqtt el valor del pot. Además luego recibe por mqtt las rpm y las imprime en un display con módulo i2c.

En el main del coche recibe vía mqtt el valor del pot del mando y alimenta a el motor dependiendo del valor.
Además envía por mqtt las rpm con un contador de pulsos detectando la presencia y ausencia de un imán con un sensor hall, detectando flancos de subida y bajada.

Sensor hall conectado a pin 4
Potenciómetro conectado a pin 4 
Motores conectados a pin 10
SDA conectado a pin 8(pantalla)
SCL conectado a pin 9(pantalla)
Todo esto conectado de su debida forma a 3.3V y GND.
