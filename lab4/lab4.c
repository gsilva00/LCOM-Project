// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    //temos que dar enable ao streamer mode escrevendo comando em KCB usando 0xD4 em 0x64. Depois escrevemos 0xEA em 0x60 (o KCB vai entregar o argumento como comando ao rato para ele mudar)
    //O rato irá enviar em OUT_BUF 0x60 um acknoledgement byte ACK (0xFA) se está OK , NACK(0xFE) if invalid byte, ERROR (0xFC) second consecutive invalid byte

    //parar depois de ler um x numero de packets -> 3
    //temos que dar mouse_print_packet()
    //temos que dar subscribe com IRQ_EXCLUSIVE
    //apos interrupt temos que ler OUT_BUF só um byte por interrupt
    //não chamar mouse_print_packet() pois a transferencia é lenta
    //devemos guardar cada byte em packet[] e usar counter para saber em que packet estamos
    //Já que as packets não têm id, não sabemos a quem uma packet[i] pertence. Para resolver, devemos ler cada byte e verificar se o 3º byte é 1. Se for 1, então significa que o byte é o 1º byte de uma sequencia de 3 bytes. 
    return 1;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)() {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}