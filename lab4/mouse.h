#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

int(mouse_subscribe)(uint16_t *bit_no);

int(mouse_unsubscribe)();

int (disable_stream_data)();

int (enable_stream_data)();



#endif /* MOUSE_H */
