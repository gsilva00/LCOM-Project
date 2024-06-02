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
  MULTIPLAYER_END,
  TIME_LIMIT,
  GOAL,
  GAME_OVER
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


/**
 * @brief Given the current states, changes them to progress the game
 * @param game_state, menu_state, menu_pause_state, done, cursor_active Current states Current states of the game
*/
void enter_new_state(GameState *game_state, MenuState *menu_state, MenuPauseState *menu_pause_state, bool *done, bool *cursor_active);


#endif
