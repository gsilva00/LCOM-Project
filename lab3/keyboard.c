#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"
#include "keyboard.h"

int keyboardhookid = 1;

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL){
    return 1;
  }
  *bit_no = BIT(keyboardhookid);
  if( sys_irqsetpolicy( KEYBOARD_IRQ ,IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboardhookid ) ) {
    return 1;
  }
  return 0;
}

int (keyboard_unsubscribe_int)() {
  if( sys_irqrmpolicy(&keyboardhookid) ){
    return 1;
  }
  return 0;
}

int (keyboard_read_out_buf)( uint8_t *scancode){
  if(scancode == NULL){
    return 1;
  }
  if(util_sys_inb(OUT_BUF, scancode)){  //exceto o primeiro byte que é 0xE0
    return 1;
  }
  return 0;
}

int (verify_error_out_buf_by_status)(){
  uint8_t status = 0;
  if(util_sys_inb(STAT_REG,&status) ){
    return 1;
  }
  //printf("status: %x ",status);
  uint8_t obf = status & 0x01;
  if(obf){

    //0x1111 1111  0x1100 0000  0x1000 0000  0x80 0x0100 0000 0x40
    uint8_t erro_time_out = (status & 0x40) >> 6;
    uint8_t erro_parity = (status & 0x80) >> 7;
    if(erro_parity || erro_time_out){
      return 1;
    }

    return 0;
  }

  return 1;
}



void (kbc_ih)(){
  uint8_t out_buf_text = 0;

  int ipc_status,r;
  message msg;

  while( 1 ) { /* You may want to use a different condition */
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */                
          if (msg.m_notify.interrupts & bit_no) { /* subscribed interrupt */
            //verificar se houve erro na entrega
            if(verify_error_out_buf_by_status()){
              printf("uhh");
              //discard out_buf
              continue;
            }

            if(keyboard_read_out_buf(&out_buf_text)){  /* process it */
              continue;
            }
            bool make = out_buf_text & 0x80;
            uint8_t size = 1;
            
            if(kbd_print_scancode(!make,size,&out_buf_text)){
              printf("uh oh");
              continue;
            }

            if(out_buf_text == 0x81){
              return;
            }

          }
          break;
        default:
          break; /* no other notifications expected: do nothing */    
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }
}






int (kbc_sys_outb)(uint8_t comando, bool com_arg ,uint8_t * argumento) {
  uint8_t status = 0;
  if(util_sys_inb(STAT_REG,&status) ){
    return 1;
  }
  uint8_t ibf = status & 0x02;

  if(ibf){
    printf("ibf encontra-se ocupado para escrever");
    return 1;
  }

  uint8_t erro_time_out = (status & 0x40) >> 6;
  uint8_t erro_parity = (status & 0x80) >> 7;
  if(erro_parity || erro_time_out){
    printf("erro!");
    return 1;
  }

  if(sys_outb(COMAND_REG,comando) ){
    printf("nao consegui enviar comando de leitura");
    return 1;
  }
  tickdelay(micros_to_ticks(DELAY_US));

  if(com_arg){
    if(argumento == NULL){
      printf("Argumento passado com insucesso");
      return 1;
    }
    if(sys_outb(ARG_REG,*argumento)){
      printf("não conseguiu enviar argumento necessário paraa escrita");
      return 1;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  
  return 0;
}

int (verify_error_out_buf_by_status_and_see_availabity)(){
  uint8_t status = 0;
  if(util_sys_inb(STAT_REG,&status) ){
    printf("nao leu status");
    return 1;
  }
  //printf("status: %x ",status);
  uint8_t obf = status & 0x01;
  uint8_t aux = status & 0x20; //0x0010 0000

  printf("\n obf: %x , aux: %x",obf,aux);
  if(obf){
    if(!aux){
    //0x1111 1111  0x1100 0000  0x1000 0000  0x80 0x0100 0000 0x40
    uint8_t erro_time_out = (status & 0x40) >> 6;
    uint8_t erro_parity = (status & 0x80) >> 7;
    if(erro_parity || erro_time_out){
      return 1;
    }

    return 0;
    }
  }

  return 1;
}

int (enable_interrupt)(){
//enable interrupt OBF lendo o command byte (escrever comando 0x20 no porto 0x64 e ler comando em 0x60) Depois trocar 0 bit para 1 e enviar comando 0x60 em 0x64 e colocar o comando trocado em 0x60
  uint8_t command_byte = 0;

  /*if(sys_outb(COMAND_REG,0x20)){
    printf("nao consegui enviar comando de leitura");
    return 1;
  }

  if(util_sys_inb(OUT_BUF, &command_byte)){
    printf("não conseguiu ler command byte em out_buf");
    return 1;
  }

  uint8_t novo_comando = command_byte | 0x01;

  if(sys_outb(COMAND_REG,0x60)){
    printf("não conseguiu escrever comando para escrever enable de teclado");
    return 1;
  }

  if(sys_outb(ARG_REG, &novo_comando)){
    printf("não conseguiu enviar argumento necessário paraa escrita enable");
    return 1;
  }*/
  uint8_t novo_comando;
  if(kbc_sys_outb(0x20,0, &novo_comando)){
    return 1;
  }

  printf("conseguiu escrever");

  if(util_sys_inb(OUT_BUF, &command_byte)){
    printf("\nnão conseguiu ler command byte em out_buf");
    return 1;
  }

  printf("command byte: %x",command_byte);

  novo_comando = command_byte | 0x01;

  printf("\nnovo comando %x",novo_comando);

  if(kbc_sys_outb(0x60,1,&novo_comando)){
    return 1;
  }

  printf("argumento com sucesso");

  return 0;
}


void (kbc_timed_ih)(uint8_t n){
uint8_t out_buf_text = 0;
uint8_t time = n;

  int ipc_status,r;
  message msg;

  while(time > 0) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */                
          if (msg.m_notify.interrupts & bit_no_timer) { /* subscribed interrupt */
            timer_int_handler(); //apenas soma o counter++;
            if(counter == 60){
              counter = 0;
              printf("\ntempo %d",time);
              time--;
              }
          }
          
          if (msg.m_notify.interrupts & bit_no) { /* subscribed interrupt */
            //verificar se houve erro na entrega
            if(verify_error_out_buf_by_status()){
              //printf("uhh");
              //discard out_buf
              continue;
            }

            time = n;
            counter = 0;

            if(keyboard_read_out_buf(&out_buf_text)){  /* process it */
              continue;
            }
            bool make = out_buf_text & 0x80;
            uint8_t size = 1;
            
            if(kbd_print_scancode(!make,size,&out_buf_text)){
              printf("uh oh");
              continue;
            }

            if(out_buf_text == 0x81){
              return;
            }

          }
          break;
        default:
          break; /* no other notifications expected: do nothing */    
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }
}
