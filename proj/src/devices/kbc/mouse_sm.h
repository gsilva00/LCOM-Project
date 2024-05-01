#ifndef MOUSE_SM_H
#define MOUSE_SM_H

#include <stdint.h>


typedef enum {
  INIT,
  FIRST_LINE,
  VERTEX,
  SECOND_LINE,
  DONE
} mouse_state_t;


// Mouse event handler
int mouse_eh(struct packet *pp, uint8_t x_len, uint8_t tolerance);

struct mouse_ev* mouse_detect_event_mine(struct packet *pp);

// Handlers for each type of event
void lbP_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance);
void lbR_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance);
void rbP_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance);
void rbR_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance);
void bEv_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance);
void mov_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance);


#endif
