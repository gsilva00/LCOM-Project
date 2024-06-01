#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <lcom/lcf.h>

#include <stdint.h>


typedef enum {
  MENU,
  PREPARE_MULTIPLAYER,
  MULTIPLAYER,
  PREPARE_SINGLEPLAYER,
  SINGLEPLAYER,
  PREPARE_PAUSE,
  PAUSE,
  PAUSE_TO_PLAY,
  PAUSE_TO_MENU,
  STATE_GAME_END,
  TIME_LIMIT,
  GOAL,
  STATE_GAME_OVER
} GameState;

typedef enum {
  HOVER_SINGLEPLAYER,
  HOVER_MULTIPLAYER,
  HOVER_EXIT,
} MenuState;

typedef enum {
  HOVER_RESUME,
  HOVER_GO_BACK
} MenuPauseState;

#endif
