#include "esp8266wifi.h"
#include "stm32f10x.h"
#include "usart.h"	 
#include "crc16.h"
#include "string.h"
#include "lcd.h"

extern u16 USART2_RX_STA;//在usart.c中定义
extern u8 USART2_RX_BUF[USART_REC_LEN];//在usart.c中定义
u8 recData;
int j = 0;
int i = 0;
//IMEI
#define MODE_CMD "AT+CWMODE=1\r\n"

#define RST_CMD "AT+RST\r\n"       // Reset the wifi module
#define CONNECTAP_CMD "AT+CWJAP=\"leon\",\"135246789\"\r\n"     // Connet to WIFI/AP
#define CONNECTIONTYPE_CMD "AT+CIPMUX=0\r\n"   // Set single connection type
#define TCPCONNECT_CMD "AT+CIPSTART=\"TCP\",\"54.166.192.107\",10012\r\n" // Connect to the TCP server
#define SENDTYPE_CMD "AT+CIPMODE=1\r\n"  // 透传
#define SEND_CMD "AT+CIPSEND\r\n"  //Send message

void esp8266_init(void)
{
	atdelay();
	//Usart_SendString(USART2, "AT+RST\r\n");
	
  atdelay();
	Usart_SendString(USART2,CONNECTAP_CMD);

	// AT + CIPMUX = 0
	atdelay();
	Usart_SendString(USART2,CONNECTIONTYPE_CMD);
	LCD_ShowString(30,50,200,16,16,"AT+SWJAP");
	// AT + CIPSTART="TCP","IP",8081
	atdelay();
	atdelay();
	Usart_SendString(USART2,TCPCONNECT_CMD);
	LCD_ShowString(30,70,200,16,16,"AT+CIPSTART");
	// AT + CIPMODE = 1
	atdelay();
	atdelay();
	atdelay();
	Usart_SendString(USART2,SENDTYPE_CMD);
	LCD_ShowString(30,90,200,16,16,"AT+CIPMODE");
	
  // AT + CIPSEND
	atdelay();
	atdelay();
	Usart_SendString(USART2,SEND_CMD);
	LCD_ShowString(30,110,200,16,16,"AT+CIPSEND");
	atdelay();
	atdelay();
//	LCD_ShowString(30,130,200,16,16,"SENDING DATA...");
//	while(i<25)
//	{
//		atdelay();
//		Usart_SendString(USART2,"SEND SUCCESSFUL");
//		i++;
//	}
  //Usart_SendString(USART2,"SUCCESSFUL CONNECTION");
	LCD_ShowString(60,210,200,16,16, (u8*)"WIFI CONNECTED...");
	
	atdelay();
	
}

void atdelay(void)
{
	delay_ms(10000);
	delay_ms(10000);
	delay_ms(10000);
}