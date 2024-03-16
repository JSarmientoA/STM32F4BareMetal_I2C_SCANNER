#include <stdint.h>
#include <stdio.h> // Para poder usar printf()
#include "stm32f4xx.h"
#include "systick.h"
#include "I2c.h"
#include "uart.h"
/*Este prgrama esanea un dispositivo i2c para encontrar su direccion*/
int main(void)
{
	/**Iniciliza la comunicacion uart*/
	uart2_rx_tx_init();
	/**Iniciliza la comunicacion i2c*/
	I2c_int();
	int vandera = 0;

	printf("Escaneando Dispositivos ...\n\r");
	for(int Address = 0;Address<128;Address++){
		/*Condicion de star para el i2c*/
		I2c_Start();
		/*Envíe la direccion al registro de datos*/
		I2C1->DR = Address;
		/*Valida si la dirección enviada coincide con la del dispositivo*/
		//printf("Ultimo valor de Address 0x%X\n\r",Address);
		if((I2C1->SR1 & (1U<<1))){
			printf("Dispositivo Encontrado 0x%X\n\r",(Address<<0)-1);
			Address=129;
			vandera = 1;
			/*Lea los registros SR1 y SR2*/
			uint8_t tmp = I2C1->SR1 | I2C1->SR2;
		}
		/*Condicion de parada para el i2c*/
		I2c_Stop();
	}

	if(vandera==0){
		printf("No se encontraron dispositivos ...\n\r");
	}

	while (1){

	  }
}
