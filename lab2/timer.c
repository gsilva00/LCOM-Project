#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  // Timer needs to be between 0 and 2, frequency needs to be between minimum frequency and maximum frequency
  if (timer > 2 || freq < 19 || freq > TIMER_FREQ) return 1;

  // Variables to: store timer status and then turn into control word, store timer's count register;
  uint8_t ctrlWord = 0x00, timerReg = TIMER_0 + timer;

  // Read timer configuration/status
  if (timer_get_conf(timer, &ctrlWord)) return 1;

  // Prepare control word without changing 4 LSbits
  // - Ignore 4 MSbits of the status byte. Bits 5,4 will become 'LSB followed by MSB'. Dont know more about bit 6. Don't need bit 7 (current value of timer's out line). 
  ctrlWord &= 0x0f;

  // - Setup the 4 MSbits:
  //   - Which timer it is:
  if (timer == 0) ctrlWord |= TIMER_SEL0;
  else if (timer == 1) ctrlWord |= TIMER_SEL1;
  else ctrlWord |= TIMER_SEL2;
  //   - Initialization mode (LSB followed by MSB): 
  ctrlWord |= TIMER_LSB_MSB;

  // Write control word to configure the timer:
  if (sys_outb(TIMER_CTRL, ctrlWord)) return 1;

  // Get new frequency for timer:
  uint16_t newFreq = (uint16_t) TIMER_FREQ/freq;

  // Split 16bits into LSB and MSB:
  uint8_t lsb = 0x00, msb = 0x00;
  if (util_get_LSB(newFreq, &lsb)) return 1;
  if (util_get_LSB(newFreq, &msb)) return 1;

  // Write both bytes to the timer reg (lsb first)
  if (sys_outb(timerReg, lsb)) return 1;
  if (sys_outb(timerReg, msb)) return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void(timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
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

  // Example:
  // 10001101 (encontro o bcd_bit)
  // 01000110 (shift 1 para a direita, ja nao preciso do bcd_bit)
  // (uso os 5 LSbits do count_bits)
  // 00000001 (shift 6 para a direta, ja nao preciso dos 5 bits do count_bits nem do bit 6 (null count))

  uint8_t bcd_bit = st & 0x01;
  uint8_t count_bits = ((uint8_t) (st >> 1)) & 0x1f;
  // uint8_t output_bit = ((uint8_t) (st >> 7) & 0x01);

  union timer_status_field_val toDisplay;
  switch (field) {
    case tsf_all:
      toDisplay.byte = st;
      break;
    case tsf_initial:
      switch (count_bits >> 3) {
        case 0b01:
          toDisplay.in_mode = LSB_only;
          break;
        case 0b10:
          toDisplay.in_mode = MSB_only;
          break;
        case 0b11:
          toDisplay.in_mode = MSB_after_LSB;
          break;
        default:
          toDisplay.in_mode = INVAL_val;
          break;
      }
      break;
    case tsf_mode: // operating mode (6 modes, bits 3,2,1)
      switch (count_bits & 0x07) {
        case 0b000:
          toDisplay.count_mode = 0;
          break;
        case 0b001:
          toDisplay.count_mode = 1;
          break;
        case 0b010: case 0b110:
          toDisplay.count_mode = 2;
          break;
        case 0b011: case 0b111:
          toDisplay.count_mode = 3;
          break;
        case 0b100:
          toDisplay.count_mode = 4;
          break;
        case 0b101:
          toDisplay.in_mode = 5;
          break;
      }
      break;
    case tsf_base:
      toDisplay.bcd = bcd_bit;
      break;
  }

  if (timer_print_config(timer, field, toDisplay)) return 1;

  return 0;
}
