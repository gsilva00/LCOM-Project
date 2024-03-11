#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KEYBOARD_IRQ 1 /**< @brief KEYBOARD IRQ line */

#define OUT_BUF    0x60 /**< @brief OUT_BUF port register */
#define STAT_REG   0x64 /**< @brief STATUS register of KBC */
#define COMAND_REG 0X64 /*o mesmo enderesso que status reg pois podemos tambem escrever comandos*/
#define ARG_REG 0X60    /*escrever argumentos se escrita ou ler return value na leitura(out_buf)*/

#define DELAY_US    20000

#endif
