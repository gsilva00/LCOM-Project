#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>

#include "keyboard.h"

uint32_t count = 0;
uint8_t bit_no = 0;
uint8_t bit_no_timer = 0;
uint8_t counter = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  //dar print de make code e break code do KBC
  //verifcar se dá erro na entrega
  //deve terminar quando lê 0x81 ou seja o break code do ESC
  //trabalhar com 2 bytes scancodes. 1º bite é 0xE0

  //temos que dar subscribe ao kbc paraa sabermos e dá interrupt para depois ler o OUT_BUF
  //para prevenir que General interrupt Handler do Minix intervenha, vamos no setpolicy colocar IRQ_REENABLE|IRQ_EXCLUSIVE

  if( keyboard_subscribe_int(&bit_no) ){
    return 1;
  }

  kbc_ih();

  kbd_print_no_sysinb(count);
 
  if (keyboard_unsubscribe_int()){
    return 1;
  }

  return 0;
}

int(kbd_test_poll)() {
  //Read the scan codes by polling o register status até que obf = 1 e aux = 0
  uint8_t out_buf_text = 0;

  while (true){
    if(verify_error_out_buf_by_status_and_see_availabity()){
      printf("Not now...");
      continue;
    }else{
      if(keyboard_read_out_buf(&out_buf_text)){
        continue;
      }
      printf("texto: %x", out_buf_text);
      bool make = out_buf_text & 0x80;
      uint8_t size = 1;
            
      if(kbd_print_scancode(!make,size,&out_buf_text)){
        printf("uh oh");
        continue;
      }

      if(out_buf_text == 0x81){
        break;
      }
    }
  }
  
  kbd_print_no_sysinb(count);
  
  if(enable_interrupt()){
    printf("nao consegui dar enable interrupt :(");
    return 1;
  }

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  if( keyboard_subscribe_int(&bit_no) ){
    return 1;
  }

  if(timer_subscribe_int(&bit_no_timer)){
    return 1;
  }

  kbc_timed_ih(n);

  kbd_print_no_sysinb(count);
 
  if (keyboard_unsubscribe_int()){
    return 1;
  }

  if(timer_unsubscribe_int()){
    return 1;
  }

  return 0;
}
