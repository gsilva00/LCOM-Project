#ifndef _LCOM_
#define _LCOM_

#define STATUS_REG 0X64
#define CONTROL_REG 0X64
#define OUT_BUF 0X60
#define IN_BUF 0x60

//irqline
#define IRQLINE_KBD 1

//STATUS BITS
#define BIT(n) (1 << (n))
#define DELAY_US 20000
#define PARITY BIT(7)
#define TIMEOUT BIT(6)
#define AUX BIT(5)
#define IBF BIT(1)
#define OBF BIT(0)


#define BREAK BIT(7)

#endif
