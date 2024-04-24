#include <lcom/lcf.h>

#include <stdint.h>

#include "mouse_sm.h"


int mouse_eh(struct packet *pp, uint8_t x_len, uint8_t tolerance) {
  static mouse_state_t state = INIT;

  void (*evHs[])(mouse_state_t*, struct packet, uint8_t, uint8_t) = {
    lbP_handler, 
    lbR_handler, 
    rbP_handler, 
    rbR_handler, 
    bEv_handler, 
    mov_handler
  };

  struct mouse_ev* currEv = mouse_detect_event_mine(pp);

  (*evHs[currEv->type])(&state, *pp, x_len, tolerance);

  return state == DONE;
}

// Lab4.h's (mouse_detect_event) gives compilation error
struct mouse_ev* mouse_detect_event_mine(struct packet *pp) {
  static struct mouse_ev event; // Stores the previous mouse event
  static uint8_t lastButtons = 0; // Stores previous mouse buttons clicked (3LSbits, bit 0 for lb, bit 1 for mb, bit 2 for rb)

  if (pp == NULL) {
    printf("Pointer to packet was null!\n");
    return &event;
  }
  
  uint8_t currButtons = 0;
  if (pp->lb) currButtons |= BIT(0);
  if (pp->rb) currButtons |= BIT(1);
  if (pp->mb) currButtons |= BIT(2);

  if (currButtons == 0x01 && lastButtons != 0x01) {
    event.type = LB_PRESSED;
    lastButtons = 0x01;
  }
  else if (currButtons == 0x00 && lastButtons == 0x01) {
    event.type = LB_RELEASED;
    lastButtons = 0x00;
  }
  else if (currButtons == 0x02 && lastButtons != 0x02) {
    event.type = RB_PRESSED;
    lastButtons = 0x02;
  }
  else if (currButtons == 0x00 && lastButtons == 0x02) {
    event.type = RB_RELEASED;
    lastButtons = 0x00;
  }
  else if (currButtons == lastButtons && (pp->delta_x || pp->delta_y)) {
    event.type = MOUSE_MOV;
  }
  else event.type = BUTTON_EV; // No meaningful event (middle button clicked, multiple buttons clicked, ...)
  
  event.delta_x = pp->delta_x;
  event.delta_y = pp->delta_y;

  return &event;
}


void lbP_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance) {
  switch(*state) {
    case INIT:
      *state = FIRST_LINE;
      break;
    case FIRST_LINE:
      if (pp.delta_x < 0-tolerance) {
        printf("The negative displacement in the X axis is too large!\n");
        *state = INIT;
        return;
      }
      if (pp.delta_y < 0-tolerance) {
        printf("The negative displacement in the Y axis is too large!\n");
        *state = INIT;
        return;
      }
      if (abs((double)(pp.delta_y/pp.delta_x)) < 1) {
        printf("The slope of the line is too small!\n");
        *state = INIT;
        return;
      }
      if (pp.delta_x < x_len) {
        printf("X-axis displacement is too small!\n");
        *state = INIT;
        return;
      }

      break;
    case VERTEX:
      *state = FIRST_LINE;
      break;
    case SECOND_LINE:
      *state = INIT;
      break;  
    case DONE:
      // Do nothing
      break;
  }
}

void lbR_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance) {
  switch(*state) {
    case INIT:
      // Do nothing
      break;
    case FIRST_LINE:
      *state = VERTEX;
      break;
    case VERTEX:
      *state = INIT;
      break;
    case SECOND_LINE:
      // Do nothing
      break; 
    case DONE:
      // Do nothing
      break;
  }
}

void rbP_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance) {
  switch(*state) {
    case INIT:
      // Do nothing
      break;
    case FIRST_LINE:
      *state = INIT;
      break;
    case VERTEX:
      *state = SECOND_LINE;
      break;
    case SECOND_LINE:
      if (pp.delta_x < 0-tolerance) {
        printf("The negative displacement in the X axis is too large!\n");
        *state = INIT;
        return;
      }
      if (pp.delta_y > tolerance) { // The Y displacement can't be positive (line goes down)
        printf("The positive displacement in the Y axis is too large!\n");
        *state = INIT;
        return;
      }
      if (abs((double)(pp.delta_y/pp.delta_x)) < 1) {
        printf("The slope of the line is too small!\n");
        *state = INIT;
        return;
      }
      if (pp.delta_x < x_len) {
        printf("X-axis displacement is too small!\n");
        *state = INIT;
        return;
      }

      break; 
    case DONE:
      // Do nothing
      break;
  }
}

void rbR_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance) {
  switch(*state) {
    case INIT:
      // Do nothing
      break;
    case FIRST_LINE:
      *state = INIT;
      break;
    case VERTEX:
      *state = INIT;
      break;
    case SECOND_LINE:
      *state = DONE;
      break; 
    case DONE:
      // Do nothing
      break;
  }
}

void bEv_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance) {
  switch(*state) {
    case INIT:
      // Do nothing
      break;
    case FIRST_LINE:
      *state = INIT;
      break;
    case VERTEX:
      *state = INIT;
      break;
    case SECOND_LINE:
      *state = INIT;
      break; 
    case DONE:
      // Do nothing
      break;
  }
}

void mov_handler(mouse_state_t *state, struct packet pp, uint8_t x_len, uint8_t tolerance) {
  switch(*state) {
    case INIT:
      // Do nothing
      break;
    case FIRST_LINE:
      // Do nothing
      break;
    case VERTEX:
      if (abs(pp.delta_x) > tolerance) {
        printf("The displacement in the X axis in the vertex position is too large!\n");
        *state = INIT;
        return;
      }
      if (abs(pp.delta_y) > tolerance) {
        printf("The displacement in the Y axis in the vertex position is too large!\n");
        *state = INIT;
        return;
      }
      break;
    case SECOND_LINE:
      // Do nothing
      break; 
    case DONE:
      // Do nothing
      break;
  }
}
