#ifndef __ESP8266_H
#define __ESP8266_H	 
#include "sys.h"
#include "delay.h"		    
#include "usart.h"
#include "led.h"
#include "lcd.h"

typedef enum {
	INIT0,
}SIM_STATUS;

void esp8266_init(void);
void atdelay(void);
#endif