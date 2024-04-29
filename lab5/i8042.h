#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <lcom/lcf.h>

#define KBD_IRQ 1
#define OUT_BUF BIT(0)
#define IN_BUF BIT(1)
#define MS_DATA BIT(5)
#define TM_ERR BIT(6)
#define PAR_ERR BIT(7)
#define BREAKCODE BIT(7)

#define STATUS_REG 0x64
#define OUTBUF_REG 0x60
#define INBUF_ARG_REG 0x60
#define INBUF_CMD_REG 0x64

#define DELAY_US 20000
#define BREAKCODE_ESC 0x81

#define READ_CMD_BYTE 0x20
#define WRITE_CMD_BYTE 0x60

#endif
