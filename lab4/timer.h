#include <lcom/lcf.h>

#include <stdint.h>
#include "i25Timer.h"

int (timer_subscribe)(uint8_t * bit_no_timer);

int (timer_unsubscribe)();

void (timer_ih)();
