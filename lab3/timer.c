#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "aux_timer.h"
#include "i8254.h"

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  // Timer is [0,2]
  if (timer > 2) return 1;
  // Frequency needs to be between minimum frequency (see notes below) and maximum frequency.
  if (freq < 19 || freq > TIMER_FREQ) {
    printf("Error in %s: Input frequency is out of bounds. Make sure it is in the interval [%d Hz, %d Hz]\n", __func__, 19, TIMER_FREQ);
    return 1;
  }

  // Variables: 
  // ctrlWord - store timer status; 
  // timerAddr - store timer counter's address;
  uint8_t status, timerAddr = TIMER_0 + timer;

  // Read timer configuration/status
  if (timer_get_conf(timer, &status)) return 1;

  // Prepare control word without changing 4 LSbits of the status
  // - Ignore 4 MSbits of the status byte. 
  // - Bits 5,4 will become 'LSB followed by MSB'. 
  // - Don't know more about bit 6. 
  // - Don't need bit 7 (current value of timer's out line). 
  uint8_t ctrlWord = status & 0x0f;

  // Setup the 4 MSbits:
  // - Which timer it is:
  if (timer == 0) ctrlWord |= TIMER_SEL0;
  else if (timer == 1) ctrlWord |= TIMER_SEL1;
  else ctrlWord |= TIMER_SEL2;
  // - Initialization mode (LSB followed by MSB): 
  ctrlWord |= TIMER_LSB_MSB;

  // Write control word to configure the timer:
  if (sys_outb(TIMER_CTRL, ctrlWord)) return 1;


  // Get number of clock cycles (see notes below):
  uint16_t numClocks = (uint16_t) (TIMER_FREQ/freq);

  // Split 2 bytes into LSB and MSB:
  uint8_t lsb = 0x00, msb = 0x00;
  if (util_get_LSB(numClocks, &lsb)) return 1;
  if (util_get_MSB(numClocks, &msb)) return 1;

  // Write both bytes to the timer reg (lsb first)
  if (sys_outb(timerAddr, lsb)) return 1;
  if (sys_outb(timerAddr, msb)) return 1;

  return 0;
}

// Arbitrary value -> it will be the mask bit before being timer's hook_id (see notes on lab2.c)
static int timer_hookId = TIMER0_IRQ;
// See notes below
static int timer_intCounter = 0;

int(timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  
  *bit_no = timer_hookId;

  // Check notes below on the arguments of this function
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hookId)) return 1;

  return 0;
}

int(timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer_hookId)) return 1;
  return 0;
}

void(timer_int_handler)() {
  timer_intCounter++;
}

int get_timer_intCounter() {
  return timer_intCounter;
}
void set_timer_intCounter(int newTime) {
  timer_intCounter = newTime;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL || timer > 2) return 1;

  uint8_t rb_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, rb_cmd)) return 1;
  if (util_sys_inb(TIMER_0 + timer, st)) return 1;

  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  if (timer > 2) return 1;

  union timer_status_field_val toDisplay;
  switch (field) {
    case tsf_all:
      toDisplay.byte = st;
      break;
    case tsf_initial: // initialization mode (3 modes, bits 5,4)
      switch ((st & (BIT(5) | BIT(4))) >> 4) {
        case 1:
          toDisplay.in_mode = LSB_only;
          break;
        case 2:
          toDisplay.in_mode = MSB_only;
          break;
        case 3:
          toDisplay.in_mode = MSB_after_LSB;
          break;
        default:
          toDisplay.in_mode = INVAL_val;
          break;
      }
      break;
    case tsf_mode: // operating mode (6 modes, bits 3,2,1)
      switch ((st & (BIT(3) | BIT(2) | BIT(1))) >> 1) {
        case 0:
          toDisplay.count_mode = 0;
          break;
        case 1:
          toDisplay.count_mode = 1;
          break;
        case 2: case 6: // See notes below
          toDisplay.count_mode = 2;
          break;
        case 3: case 7:
          toDisplay.count_mode = 3;
          break;
        case 4:
          toDisplay.count_mode = 4;
          break;
        case 5:
          toDisplay.count_mode = 5;
          break;
      }
      break;
    case tsf_base:
      toDisplay.bcd = st & BIT(0);
      break;
  }

  if (timer_print_config(timer, field, toDisplay)) return 1;

  return 0;
}

/* NOTES:
<About timer_set_frequency()>
Notes on the numClocks variable:
- Corresponds to the number of clock cycles - also called count (each timer's internal count) - for the timer to achieve the desired frequency, freq. 
- This is because, every clock cycle, the timer internal counter decrements. And having the counter's initial value be equal to numClocks, makes it reach 0 and reset to the initial value, exactly 'freq' number of times. 
- It has to do with MINIX's timer being in Mode 3 (Square Wave Generator). "An initial count of N results in a square wave with a period of N CLK cycles" (https://web.fe.up.pt/~pfs/aulas/lcom2010/labs/lab3/intel-82c54-timer.pdf)

Note on the minimum allowed frequency:
- The result of TIMER_FREQ/freq can be at most UINT16_MAX (65532), because 16-bit is the size of the timer's internal counter. So freq (uint32_t) can be, at a minimum, 18,207623756, therefore the lowest integer it can be is 19. 
- If it was 18, the timer's internal counter would become 66287,888888889 - which even the integer part (66287) is impossible to represent in 16-bit.


<About timer_display_conf()>
Note on operating modes/counting modes 2 and 3: 
- The 3rd bit (MSbit) of operating modes 2 and 3 are 'don't care' bits - they can be either 0 or 1. According to https://web.fe.up.pt/~pfs/aulas/lcom2122/labs/lab2/lab2_03.html table 1's caption, case 6 (0b110) and 7 (0b111) are used to ensure compatibility with future Intel products. - THIS IS A MISTAKE, confirmed by professor Pedro Souto
- While https://web.fe.up.pt/~pfs/aulas/lcom2010/labs/lab3/intel-82c54-timer.pdf figure 7's caption says the opposite: case 2 (0b010) and 3 (0b011) are the ones that are actually for compatibility with future Intel products. - THIS IS CORRECT


<About timer_subscribe_int()>
Note on sys_irqsetpolicy(int irq_line, int policy, int *timer_hookId)'s arguments;
- irq_line is the irq_line of the device I want (timer 0 - https://web.fe.up.pt/~pfs/aulas/lcom2122/labs/lab2/lab2_04.html - table 5)
- policy has 2 options:
  - IRQ_REENABLE: The kernel automatically re-enables the interrupt after notification. (For timer interrupts, a common choice is IRQ_REENABLE to ensure continuous triggering of timer events without manual re-enabling. - by Google Gemini)
  - IRQ_DISABLE: The kernel disables the interrupt after notification, and you'll need to manually re-enable it using sys_irqenable.
- timer_hookId is used both for:
  - input to the call: value that will be used in interrupt notifications; 
  - output from the call (returned by the call): must be used in the other MINIX 3 kernel calls to specify the interrupt notification to operate on.

<About timer_int_handler(), get_int_counter()>
- timer_int_handler() increments the int_counter variable to store the number of interrupts the timer has sent (60 Hz - 60 per second).
- get_int_counter() serves as a getter for the static global int_counter variable. It's use is to be called in other files.
*/
