#include "stm32f4xx.h"
#include "I2c.h"
#include "systick.h"

/*Pinout
 * PB8 ---> SCL--> D15
 * PB9 ---> SDA--> D14
 * */

void I2c_int(void){
	/*Habilitar el reloj para el I2C*/
	RCC->APB1ENR |= (1U<<21);

	/*Habilitar el reloj para el GPIOB*/
	RCC->AHB1ENR|= (1U<<1);

	/*Estableser PB8 y PB9 como funcion alternativa*/
	GPIOB->MODER &=~ (1U<<16);
	GPIOB->MODER |=  (1U<<17);
	GPIOB->MODER &=~ (1U<<18);
	GPIOB->MODER |=  (1U<<19);

	/*Estableser PB8 y PB9 en open drain*/
	GPIOB->OTYPER |= (1U<<8);
	GPIOB->OTYPER |= (1U<<9);

	/*Configuracion de PB9 y PB8 en alta valocidad*/
	GPIOB->OSPEEDR |= (1U<<16);
	GPIOB->OSPEEDR |= (1U<<17);
	GPIOB->OSPEEDR |= (1U<<18);
	GPIOB->OSPEEDR |= (1U<<19);

	/*Habilite las resistencias Pull up  para PB9 y PB8*/
	GPIOB->PUPDR |=  (1U<<16);
	GPIOB->PUPDR &=~ (1U<<17);
	GPIOB->PUPDR |=  (1U<<18);
	GPIOB->PUPDR &=~ (1U<<19);

	/*establecer como función alternativa PB8 y PB9*/
	//PB8
	GPIOB->AFR[1] &=~ (1U<<0);
	GPIOB->AFR[1] &=~ (1U<<1);
	GPIOB->AFR[1] |=  (1U<<2);
	GPIOB->AFR[1] &=~ (1U<<3);
	//PB9
	GPIOB->AFR[1] &=~ (1U<<4);
	GPIOB->AFR[1] &=~ (1U<<5);
	GPIOB->AFR[1] |=  (1U<<6);
	GPIOB->AFR[1] &=~ (1U<<7);

	/*I2C en estado de reset*/
	I2C1->CR1 |= (1U<<15);

	/*Salida del reinicio I2C*/
	I2C1->CR1 &= ~(1U<<15);

	/*Estableser la frecuencia del bus donde esta conectado el I2C. Esto es:
	 * Bits 5:0 FREQ[5:0]: Frecuencia de reloj periférico, esto es:
	 * Los bits FREQ deben configurarse con el valor de frecuencia de reloj
	 * APB (periférico I2C conectado a APB). El periférico utiliza el campo
	 * FREQ para generar tiempos de configuración y retención de datos que
	 * cumplan con las especificaciones I2C. La frecuencia mínima permitida
	 * es de 2 MHz, la frecuencia máxima está limitada por la frecuencia máxima
	 * APB (45 MHz) y no puede exceder los 50 MHz (límite máximo intrínseco del
	 * periférico).
	 * */
	I2C1->CR2 |= (1U<<4);// 16MHZ por defecto todo se sta trabajabdo a 16 MHZ

	/*Configuracion del reloj para que el I2C tranaje a 100KHZ*/
	/*Calculos para obtener la frecuencia del I2C en modo estandar 100KHZ*/
	/*El bit DUTY (15) del registro RCC esta por default en 0 debido a que no se va autilizar el modo e alta velocidad
	 *Las fromulas para el calculo de la velocidad son:*/
	//Thigh = CCR * TPCLK1, donde Thigh  = tr(SCL) + tw(SCLH).
	//Tlow = CCR * TPCLK1,  donde Tlow = tf(SCL) + tw(SCLL)
	/*Se puede usar cualquiera de las dos
	 * Los valores de tr(SCL), tw(SCLH), tf(SCL) y tw(SCLL) se encuentran en la hoja de datos del micro*/

	/*
	 * Al despejar RCC y reemplazar los valores de Thigh  = tr(SCL) + tw(SCLH), queda
	 *
	         tr(SCL)(ns) + tw(SCLH)(ns)
	  CCR =	---------------------------
		              TPCLK1
	                 1
      Con TPCLK1 = -----
				   16MHZ (Frecuencia del bus a la que esta conectada el prifefrico)

	         1000ns 4000ns
	  CCR = ---------------- = 80
	            62.5ns
	*/

	//Segun la hoja de datos tw(SCLH) = 4.0 μs y tr(SCL) = 1000 μs. Tabla 61

	/*Setear el I2C en modo estandar, esto es la configuracion del reloj*/
	I2C1->CCR = 80<<0;

	/*Setee el tiempo de riso TRISE*/

	/*
	         tr(SCL)           1000ns
	TRISE = ---------- + 1 = ---------- + 1 = 17
	         TPCLK1            62.5ns
	*/

	I2C1->TRISE = 17;//Esto es 17

	/*Habiltar el I2C*/
	I2C1->CR1 |= (1U<<0);
}

void I2c_Start(void){
	/*Abilita el reconocomiento (Acknowledge)*/
	I2C1->CR1 |= (1U<<10);

	/*Generando una condicion de imicio para el I2C*/
	I2C1->CR1 |= (1U<<8);

	/*Esperar a que se establezca la bandera SB*/
	/*si el bit de inicio no está sentado,
	 *nos quedaremos atascados aquí, cuando se establezca SR1_SB, saldrá de este bucle aquí.*/
	while(!(I2C1->SR1 & (1U<<0)));
}

void I2c_Write(uint8_t data){
	/*Esperar hasta que el registro de datos esté vacío validando la vandera TXE*/
	while(!(I2C1->SR1 & (1U<<7)));

	/*Envío de bytes*/
	I2C1->DR = data;//<<1;

	/*Esperamos a que se termine la trnasferencia de bytes*/
	while(!(I2C1->SR1 & (1U<<2)));
}

void I2c_Address(uint8_t Address){
	/*Envíe la direccion al registro de datos*/
	I2C1->DR = Address;

 	while(!(I2C1->SR1 & (1U<<1)));

	/*Lea los registros SR1 y SR2*/
	uint8_t tmp = I2C1->SR1 | I2C1->SR2;
}

void I2c_Stop(void){
	I2C1->CR1 |= (1U<<9);
}

/*Si se desa escribir mas de un byte de datos a la vez*/
void I2c_WriteMultiByte(uint8_t *data, uint8_t n){// n = número de bytes que se quieren enviar
	while(!(I2C1->SR1 & (1U<<7)));
	while(n){
		while(!(I2C1->SR1 & (1U<<7)));
		I2C1->DR = (volatile uint32_t)*data++;
		n--;
	}
	/*Una vez enviado el ultimo byte de datos espere epere a que se reestablesca BTF*/
	while(!(I2C1->SR1 & (1U<<2)));
}


void I2c_Read (uint8_t Address, uint8_t *buffer, uint8_t size)
{
/**** STEPS FOLLOWED  ************
1. If only 1 BYTE needs to be Read
	a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
	b) the Acknowledge disable is made during EV6 (before ADDR flag is cleared) and the STOP condition generation is made after EV6
	c) Wait for the RXNE (Receive Buffer not Empty) bit to set
	d) Read the data from the DR

2. If Multiple BYTES needs to be read
  a) Write the slave Address, and wait for the ADDR bit (bit 1 in SR1) to be set
	b) Clear the ADDR bit by reading the SR1 and SR2 Registers
	c) Wait for the RXNE (Receive buffer not empty) bit to set
	d) Read the data from the DR
	e) Generate the Acknowlegment by settint the ACK (bit 10 in SR1)
	f) To generate the nonacknowledge pulse after the last received data byte, the ACK bit must be cleared just after reading the
		 second last data byte (after second last RxNE event)
	g) In order to generate the Stop/Restart condition, software must set the STOP/START bit
	   after reading the second last data byte (after the second last RxNE event)
*/

	int remaining = size;

/**** STEP 1 ****/
	if (size == 1)
	{
		/**** STEP 1-a ****/
		I2C1->DR = Address;  //  send the address
		while (!(I2C1->SR1 & (1<<1)));  // wait for ADDR bit to set

		/**** STEP 1-b ****/
		I2C1->CR1 &= ~(1<<10);  // clear the ACK bit
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit.... EV6 condition
		I2C1->CR1 |= (1<<9);  // Stop I2C

		/**** STEP 1-c ****/
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set

		/**** STEP 1-d ****/
		buffer[size-remaining] = I2C1->DR;  // Read the data from the DATA REGISTER

	}

/**** STEP 2 ****/
	else
	{
		/**** STEP 2-a ****/
		I2C1->DR = Address;  //  send the address
		while (!(I2C1->SR1 & (1<<1)));  // wait for ADDR bit to set

		/**** STEP 2-b ****/
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit

		while (remaining>2)
		{
			/**** STEP 2-c ****/
			while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set

			/**** STEP 2-d ****/
			buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer

			/**** STEP 2-e ****/
			I2C1->CR1 |= 1<<10;  // Set the ACK bit to Acknowledge the data received

			remaining--;
		}

		// Read the SECOND LAST BYTE
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;

		/**** STEP 2-f ****/
		I2C1->CR1 &= ~(1<<10);  // clear the ACK bit

		/**** STEP 2-g ****/
		I2C1->CR1 |= (1<<9);  // Stop I2C

		remaining--;

		// Read the Last BYTE
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer
	}

}















