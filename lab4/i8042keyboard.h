#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KEYBOARD_IRQ 1 
#define OUT_BUF    0x60 
#define STAT_REG   0x64
#define COMAND_REG 0X64 
#define ARG_REG 0X60    

#define DELAY_US    20000

#define TENTATIVAS 10 

#define PARITY_ERROR BIT(7)
#define TIMEOUT_ERROR BIT(6) 
#define AUX BIT(5) 
#define IBF BIT(1)
#define OBF BIT(0)

#define BREAKCODE BIT(7) //Para ver breakcode ex: 0x01 ou 0x81

#define READ_COMANDBYTE 0x20 
#define WRITE_COMANDBYTE 0x60



#endif
