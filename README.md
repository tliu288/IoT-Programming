# IoT-Programming
STM Board programming with the camera and Wi-Fi module

# IoT Unit
------------------------------
## Hardware Component Breakdown

| Component     | Type           | Name  |
| ------------- |:-------------:| -----:|
| Main Board    | MCU        | STM32F103C8T6 |
| Camera Module | Camera      |  OV7725 |
| Wi-Fi Module  | Wireless Communication   | ESP8266 |
| Memory Storage  | External Memory   | SD Card |
| Power Supply  | Power Supply   | 5V-12V Power Supply |

**Note: If voltage goes beyond 12V, MCU unit will be burned unless a step-down transducer is used.**

## IoT Programming
- Porgramming on STM32F103C8T6 to utilize the Wi-Fi module and the Camera Module
- The most important file is [Main file](./IoT_STM32_Programming/Image_Capture/USER/main.c)
  * Line 224 enables WiFi initialization and makes use of it. If WiFi is not used, comment this line.
  * Uncomment Line 279 if Wi-Fi is used.

## Credit
- Credit to **Opendv** for assistance on component initialization [Opendv](http://www.openedv.com)
- Credit to **ELM-Chan** for FatFs File system in Microcontroller programming [FatFs](http://elm-chan.org/fsw/ff/00index_e.html)
