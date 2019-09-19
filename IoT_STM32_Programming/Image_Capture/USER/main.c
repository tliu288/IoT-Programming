#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "piclib.h"
#include "ov7725.h"
#include "key.h"
#include "exti.h"
#include "led.h"
#include "imageprocess.h"
#include "string.h"
#include "nbiot.h"
#include "esp8266wifi.h"

extern u8 ov_sta;	// Defined in exti.c
extern u16 USART2_RX_STA;// Defined in usart.c
extern u8 USART2_RX_BUF[USART_REC_LEN];//Defined in usart.c

#define  OV7725_WINDOW_WIDTH		320 // <=320
#define  OV7725_WINDOW_HEIGHT		240 // <=240

FRESULT res_sd;// File operation result
FRESULT res_bmp;
FIL fnew; // File
UINT fnum; // How many bytes read in the file

FRESULT res;
FIL fsrc;
uint16_t a;
UINT byteread;
uint8_t buffer[512];
UINT fileSize;
uint16_t successBit;


int indexsize;
int t;

u16 x1 = 0;
u16 y1 = 0;
u16 x2 = 320;
u16 y2 = 240	;// Window size

//u16 x1 = 120;
//u16 y1 = 80;
//u16 x2 = 200;
//u16 y2 = 160;

// Picture result
u16  rec_result = 0;

// Update screen display
void camera_refresh(void)
{
	u16 i,j;
 	u16 color;
	BITMAPINFO bmp;
	u16 cut_width = 320, cut_height = 240;
	
	// Press KEY1 to capture the image
	if(ov_sta && KEY_Scan(1) == S1)
	{
		LCD_Scan_Dir(U2D_L2R);		// Direction
		LCD_WriteRAM_Prepare();   // Write to RAM
		
		// Open file, if file doesn't exit, create.
		res_sd = f_open(&fnew, "0:test1.bmp", FA_OPEN_ALWAYS | FA_WRITE);
		
		// File opens successfully
		if(res_sd == FR_OK)
		{
			// Fill bmp header
			bmp.bmfHeader.bfType = 0x4D42;				// Set as BMP. BMP Header starts with 4D42 
			bmp.bmfHeader.bfOffBits=sizeof(bmp.bmfHeader) + sizeof(bmp.bmiHeader) + sizeof(bmp.RGB_MASK);						//How many bytes in the header
			bmp.bmfHeader.bfSize= bmp.bmfHeader.bfOffBits + cut_width * cut_height * 2;	// FIle size 
			bmp.bmfHeader.bfReserved1 = 0x0000;		// Reserved space
			bmp.bmfHeader.bfReserved2 = 0x0000;
			
			// Fill bmp info header
			bmp.bmiHeader.biSize=sizeof(bmp.bmiHeader);  				    // How many bytes in BMP info header
			bmp.bmiHeader.biWidth=cut_width;  														// Width
			bmp.bmiHeader.biHeight=cut_height;  			    // Heights
			bmp.bmiHeader.biPlanes=1;  				    // Poriority
			bmp.bmiHeader.biBitCount=16;          // How many bits in a pixel, here is 16 for RGB picture
			bmp.bmiHeader.biCompression=3;  	    // RGB565, 3 bytes
			bmp.bmiHeader.biSizeImage=cut_width * cut_height * 2; // Image size
			bmp.bmiHeader.biXPelsPerMeter=0;			// Horizontal resolution
			bmp.bmiHeader.biYPelsPerMeter=0; 			// Vertical resolution
			bmp.bmiHeader.biClrImportant=0;   	  // Importance of R,G,B. 0 means R,G,B are equivalently important
			bmp.bmiHeader.biClrUsed=0;  			   
			
			//RGB565 Type code
			bmp.RGB_MASK[0] = 0X00F800;
			bmp.RGB_MASK[1] = 0X0007E0;
			bmp.RGB_MASK[2] = 0X00001F;
			
			//printfBmpFileInfo(bmp.bmfHeader);
			//printfBmpInfo(bmp.bmiHeader);
			
			// Write headers into bmp file
			res_sd = f_write(&fnew, &bmp, sizeof(bmp), &fnum);
			
			OV7725_RRST=0;				// enable reset read pointer
			OV7725_RCK_L;
			OV7725_RCK_H;
			OV7725_RCK_L;
			OV7725_RRST=1;				// read pointer ends
			OV7725_RCK_H; 
			
			for(i=0;i<240;i++)
			{
				for(j=0;j<320;j++)
				{
					OV7725_RCK_L;
					color=GPIOC->IDR&0XFF;	// read a byte
					OV7725_RCK_H; 
					color<<=8;  // shift
					OV7725_RCK_L;
					color|=GPIOC->IDR&0XFF;	//read a byte
					OV7725_RCK_H; 
					
					if((i > y1 && i < y2) && (j > x1 && j < x2))
					{
						f_write(&fnew, &color, sizeof(color), &fnum);
						//Usart_SendString(USART2,color);
						
					  //printf((char*)color);
						//printf(" ");
						LCD->LCD_RAM=color;
					}
					else
					{
						color = 0xFFFF;
						f_write(&fnew, &color, sizeof(color), &fnum);
						
						LCD->LCD_RAM=color;
					}
				}
			}
		}
		// Close the file
		f_close(&fnew);
		delay_ms(3000);
		ov_sta=0;					// Reset flag, start next capture
		LCD_Scan_Dir(DFT_SCAN_DIR);
	}
	
	if(ov_sta)
	{
		LCD_Scan_Dir(U2D_L2R);	 
		LCD_WriteRAM_Prepare(); 
		
		OV7725_RRST=0;				
		OV7725_RCK_L;
		OV7725_RCK_H;
		OV7725_RCK_L;
		OV7725_RRST=1;			
		OV7725_RCK_H; 
		
		for(i=0;i<240;i++)
		{
			for(j=0;j<320;j++)
			{
				if((i == y1 && j > x1 && j < x2) || (i == y2 && j > x1 && j < x2) || (j == x1 && i > y1 && i < y2) || (j == x2 && i > y1 && i < y2))
				{
					OV7725_RCK_L;
					OV7725_RCK_H; 
					OV7725_RCK_L;
					OV7725_RCK_H; 
					LCD->LCD_RAM=0xF800; 
				}
				else
				{
					OV7725_RCK_L;
					color=GPIOC->IDR&0XFF;	// Read first 8 bits
					OV7725_RCK_H; 
					color<<=8;  						// Shifting bits
					OV7725_RCK_L;
					color|=GPIOC->IDR&0XFF;	// Read last 8 bits
					OV7725_RCK_H; 
					LCD->LCD_RAM=color; 
				}
			}
		}
		ov_sta=0;					// Set flag to 0 and start next capture
		LCD_Scan_Dir(DFT_SCAN_DIR);	// Reset to default direction
	}
}

int main(void)
{
	u8 lightmode=0,saturation=2,brightness=2,contrast=2,effect=0;
	u8 i = 0, j = 0, k  = 0;
	int r1[5];// result
	u8 flag = 0;
	
	delay_init();	    	// Delay methods initializtion 
	uart_init(230400);	 	// usart 1 baud rate to 230400
	usart2_init(9600);		// usart 2 baud rate to 9600
	LCD_Init();					// LCD screen initialization
	KEY_Init();					// Key initialization
 	mem_init(SRAMIN);		// Memory pool initialization
 	exfuns_init();			// external memory request for FATFS
  f_mount(0,fs[0]); 	// SD card initialization
	piclib_init();			// Picture writing initialization

	while(OV7725_Init() != 0);				// Camera initialization. If failed, try again
	
	POINT_COLOR = RED;
	LCD_ShowString(30,30,200,16,16,"System is Initiating...");
  OV7725_Light_Mode(lightmode);
	OV7725_Color_Saturation(saturation);
	OV7725_Brightness(brightness);
	OV7725_Contrast(contrast);
	OV7725_Special_Effects(effect);
	
	//	
	OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,0);//VGA Picture mode
	// Enable output
	OV7725_CS=0;
	
	//WIFI MODULE INITIALIZATION
	//esp8266_init();
	
	
	EXTI8_Init();				// Enable interrupt for camera
	delay_ms(8000);
	LCD_Clear(BLACK);	// Clear screen
	while(1)
	{
		k = 0;
		camera_refresh();// Update the screen display
		
		// Press KEY2 to display the image
		if(KEY_Scan(1) == S2)
		{
			LCD_Clear(BLACK);
			ai_load_picfile((u8*)"0:test1.bmp",0,0,lcddev.width,lcddev.height,1);// Display the image
			delay_ms(5000);
			LCD_Clear(BLACK);// Clear the screen
			continue;
		}
		
		//按下S3图片处理
		if(KEY_Scan(1) == S3)
		{
			LCD_Clear(WHITE);
			// Send picture
			res = f_open(&fsrc, "0:test1.bmp", FA_READ);
		
			if(res == FR_OK)
			{
				fileSize = fsrc.fsize;
				//printf("File size:");
				//printf("%d\r\n",fileSize);
				byteread=1;
				a=0;
				//printf("res_open:%x \n",res);
				//printf("File content : ");
				//printf("\r\n");
				
			LCD_ShowString(30,30,200,16,16,"SENDING DATA...");
				
				while(1)
				{
					for(a=0; a<512; a++)
					{
						buffer[a]=0;
					}
					res = f_read(&fsrc,buffer, sizeof(buffer),&byteread);
          for(t=0;t<512;t++) 
							{ 
						    //Usart_SendByte(USART2,buffer[t]);
								//USART_SendData(USART2,buffer[t]);
							  printf("%c",buffer[t]);
							}
					//delay_ms(200);
							
					//Usart_SendString(USART2,buffer);
					if(res || byteread==0)
					{
						break;
					}
				}

//				for(indexsize=0;indexsize<3;indexsize++){
//					for(a=0; a<80; a++)
//					{
//						buffer[a]=0;
//					}
//					res = f_read(&fsrc,buffer, sizeof(buffer),&byteread);
//							for(t=0;t<80;t++) 
//							{ 
//								printf("%c",buffer[t]); 
//							}
//							
//				}
			}
			f_close(&fsrc);
			delay_ms(2000);
			successBit = 0;
			USART_SendData(USART2,successBit);
			LCD_Clear(BLACK);
			}
		}

		
	}
