#include "game_state.h"


void enter_new_state(GameState *game_state, MenuState *menu_state, MenuPauseState *menu_pause_state, bool *done, bool *cursor_active) {
  if (*game_state == MENU) {
    if (*menu_state == HOVER_SINGLEPLAYER) {
      *game_state = PREPARE_SINGLEPLAYER;
    }
    else if (*menu_state == HOVER_MULTIPLAYER) {
      *game_state = PREPARE_MULTIPLAYER;
    }
    else if (*menu_state == HOVER_EXIT) {
      *game_state = GAME_OVER;
      *done = true; // Exits the game
    }
  }
  else if (*game_state == PAUSE) {
    if (*menu_pause_state == HOVER_RESUME) {
      *game_state = PAUSE_TO_PLAY;
    }
    else if (*menu_pause_state == HOVER_GO_BACK) {
      *game_state = PAUSE_TO_MENU;
      *cursor_active = true;
    }
  }
  else if (*game_state == MULTIPLAYER_END || *game_state == TIME_LIMIT) {
    *game_state = PAUSE_TO_MENU;
    *cursor_active = true;
  }
}
