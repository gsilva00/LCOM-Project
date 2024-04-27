#ifndef _LCI8042M_
#define _LCI8042M_

#include <lcom/lcf.h>

#define MOUSE_IRQ 12 

#define DELAY_US_MOUSE 25000

#define MOUSE_WRITE 0xD4 

#define ACK 0xFA 
#define NACK 0xFE
#define ERROR 0xFC 

#define Y_OVER BIT(7) 
#define X_OVER BIT(6) 
#define DELTAY_MSB BIT(5) 
#define DELTAX_MSB BIT(4)
#define PS2_FIRSTID BIT(3) 
#define MIDDLE_BUTTON BIT(2) 
#define RIGHT_BUTTON BIT(1) 
#define LEFT_BUTTON BIT(0) 

#define DATA_DIS 0xF5 
#define DATA_ENABLE 0xF4

#endif
