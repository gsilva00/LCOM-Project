#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <lcom/lcf.h>
#include <stdint.h>

typedef enum {
  STATE_MENU_HOVER_SINGLEPLAYER,
  STATE_MENU_HOVER_MULTIPLAYER,
  STATE_MENU_HOVER_EXIT,
  STATE_MENU_SELECTION
} MenuState;

#endif
