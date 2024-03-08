#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  //escrever o readback comand para para ler as configuraçoes do timer. so assim é que podemos mudar
  // RB command : 1110 0010 em 0x43
  //ler o que recebemos ,status, em 0x40
  uint8_t status = 0;
  if(timer_get_conf(timer, &status)){
    return 1;
  }
  //analisar a donfiguração
  //chamartimer_print_config
  if( timer_display_conf(timer,status,field) ){
    return 1;
  }

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  //mudar a frequencia a que o timer 0 gera interrupts escrevendo control word em 0x43 antes de acessar os timers
  // em que a control word não pode mudar s ultimos 4 bits(mode 3 e bcd/-binary counting)
  //ler antes de tudo o status do timer 0? LSB antes de MSB
  //colocar divisor no timer0
  if(timer_set_frequency(timer,freq)){
    return 1;
  }

  return 0;
}


//          NOTA EU NAO SEI PORQUE ELE VAI TAO RAPIDO
int(timer_test_int)(uint8_t time) {
  //nao chama elapsed_time() com timer_int_handler() eles são separados
  //criar interrupt handler em timer.c
  //ciar para esconder detalhes do i8254 de outro codigo -> retorna numero bit de msg.m_notify.interrupts quando ocorre interrupt do timer0
  //_______________________//
    int ipc_status, r;  //o r é apenas para saber se o driver enviou uma mensagem(colocando a mensagem em msg) para o processo(0 = não há erros, 1 = erro)
    message msg;
    uint8_t bit_no = 0;
    //incrementar global counter. mas era bom saber onde se poe o counter sem ser no i8254
    counter = 0;
    
    //subscrever timer0 (hook_id?)
    if (timer_subscribe_int(&bit_no)){
      return 1;
    }

    //criar loop aqui de iinterrupcao;
    //uma vez por segundo
    while (0 < time) {    //counter conta quantos segundos passaram até chegar ao time pedido
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & bit_no) { /* subscribed interrupt */
                        timer_int_handler(); //apenas soma o counter++;
                        printf(" %d", counter);
                         //counter vai contar ate 60.Ou seja para saber que passou um segundo: nós sabemos que freq = 60Hz, logo se 1 freq = counter/1seg <=> 60Hz = 60 counter / 1seg <=> num segundo há 60 voltas. Entao segs = voltas/60
                        if(counter == 60){
                          counter = 0;
                          timer_print_elapsed_time(); //faz print do counter
                          time--;
                        }
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        } else { /* received standart message, not a notification */
            /* no standart message expected: do nothing */
        }
    }

    if (timer_unsubscribe_int()){
      return 1;
    }

    return 0;
}
