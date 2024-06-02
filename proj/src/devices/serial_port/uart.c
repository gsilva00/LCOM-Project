#include "uart.h"
#include "uart_macros.h"

#include "../kbc/keyboard.h"


static int uart_hookId = COM1_IRQ;
static queue_t *receive_q;
static uint8_t minix_IER;

int configure_uart() {
  uint8_t og_config;
  if (util_sys_inb(COM1_PORT + UART_IER, &og_config)) {
    printf("Error while reading initial IER state!\n");
    return 1;
  }

  // Store MINIX IER configuration for resetting later
  minix_IER = og_config;

  // Ignore interrupts configured by MINIX (4 LSbits)
  uint8_t new_config = og_config & 0xF0;

  // Set interrupts when there's data for reading
  new_config |= DATA_RDY_INT;
  if (sys_outb(COM1_PORT + UART_IER, new_config)) {
    printf("Error while enabling necessary ints!\n");
    return 1;
  }

  receive_q = new_queue(Q_INIT_SZ);
  return 0;
}

int reset_uart() {
  // Revert back what was changed
  if (sys_outb(COM1_PORT + UART_IER, minix_IER)) {
    printf("Error while reverting IER change!\n");
    return 1;
  }
  delete_queue(receive_q);

  return 0;
}

int uart_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {printf("Pointer to store bit_no points to NULL!\n"); return 1;}
  *bit_no = uart_hookId;

  // Need for IRQ_EXCLUSIVE - Minix has default IH for serial port
  return sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &uart_hookId);
}

int uart_unsubscribe_int() {
  return sys_irqrmpolicy(&uart_hookId);
}

void uart_ih() {
  printf("There's interrupt\n");
  // Store value from interrupt identification register
  uint8_t iir;
  // Store the status
  uint8_t st;
  if (util_sys_inb(COM1_PORT + UART_IIR, &iir)) {
    printf("Error while getting IIR info!\n");
    return;
  }
  if (util_sys_inb(COM1_PORT + UART_LSR, &st)) {
    printf("Error on initial status read!\n");
    return;
  }

  if (iir & IIR_NONE) { // There's no interrupt pending
    printf("No UART interrupt\n");
    return;
  }

  // If there's an interrupt
  // Only relevant information is first 4 bits
  if ((iir & 0x0F) == IIR_DATA_RDY) {
    while (st & LSR_DATA) {
      if (receive_char(st)) {
        printf("Error while reading char from uart!\n");
        return;
      }

      if (util_sys_inb(COM1_PORT + UART_LSR, &st)) { // Update the status
        printf("Error while reading status!\n");
        return;
      }
    }
  }
}


int receive_char(uint8_t st) {
  uint8_t chr;

  // Read the character from the buffer to clear it
  // (even if it has errors - checked later)
  if (util_sys_inb(COM1_PORT + UART_RBR, &chr)) {
    printf("Error while reading data buffer!\n");
    return 1;
  }

  if (st & (LSR_ERRFRAME | LSR_ERRPAR | LSR_ERROVER)) {
    printf("Error in received character!\n");
    return 1;
  }

  printf("The character received is: 0x%08x\n", chr);

  if (put_queue(receive_q, chr)) {
    printf("Error while pushing character to queue\n");
    return 1;
  }

  return 0;
}

int send_char(uint8_t chr) {
  printf("The character being sent is: 0x%08x\n", chr);

  if (sys_outb(COM1_PORT + UART_THR, chr)) {
    printf("Error while writing to THR!\n");
    return 1;
  }

  return 0;
}


queue_t* get_receive_q() {
  return receive_q;
}
