#ifndef LCOM_I8254_COPIA
#define LCOM_I8254_COPIA

#define TIMER_IRQ_LINE 0

#define frequencia_do_timer 1193181

#define TIMER_0_PORT 0x40
#define TIMER_1_PORT 0x41
#define TIMER_2_PORT 0x42
#define CONTROL_REG  0x43

#define READ_BACK_CMD 0XC0 //0x1100 0000
#define NOT_COUNT  0X20     //0x0010 0000
#define NOT_STATUS 0X10     //0x0001 0000

#define INIT (BIT(4) | BIT(5))
#define MODE (BIT(3) | BIT(2) | BIT(1))

#endif
