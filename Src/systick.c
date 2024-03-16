#include "stm32f4xx.h"

#define	SYSTICK_LOAD_VAL		16000
#define CTRL_ENABLE				(1U<<0)
#define TCRL_CLKSRC				(1U<<2)
#define CTRL_COUNTFLAG			(1U<<16)


void SystickDelayMS(int delay){
	/*************Configuracion del SysTick**************/

	/*Ingrese el número de MiliSegundos*/
	SysTick->LOAD = SYSTICK_LOAD_VAL;

	/*Borrar el valor actual del registro SysTick*/
	SysTick->VAL = 0;

	/*Hbilite el SysTick y seleecione la fuente de rloj interna*/
	SysTick->CTRL = CTRL_ENABLE |TCRL_CLKSRC;

	for(int i; i<delay; i++){
		/*Espere hasta que se establesca COUNTFLAG*/
		while((SysTick->CTRL & CTRL_COUNTFLAG)==0);
	}
	/*Inhabilita el registro SysTick*/
	SysTick->CTRL = 0;
}
