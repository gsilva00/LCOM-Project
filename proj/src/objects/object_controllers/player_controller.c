#include "player_controller.h"


// Time variables to simulate acceleration
static uint8_t time_passed_player_y = 0;
static uint8_t time_passed_player_x = 0;

static int player_y_original;
static int player_y;
static int player_x;
static int player_yspeed;
static int player_xspeed;
static bool chuta;

static int img = 0; // Image number to perform animation
static xpm_map_t player1_map0 = (xpm_map_t) personagem_parado_xpm;
static xpm_map_t player1_map1 = (xpm_map_t) personagem_chutar_frente_1_xpm;
static xpm_map_t player1_map2 = (xpm_map_t) personagem_chutar_frente_2_xpm;
static xpm_map_t player1_map3 = (xpm_map_t) personagem_chutar_frente_3_xpm;
static xpm_map_t player1_map4 = (xpm_map_t) personagem_chutar_frente_4_xpm;
static xpm_map_t player1_map5 = (xpm_map_t) personagem_chutar_frente_5_xpm;
static xpm_map_t player1_map6 = (xpm_map_t) personagem_chutar_frente_6_xpm;
static xpm_map_t player1_map7 = (xpm_map_t) personagem_chutar_tras_1_xpm;
static xpm_map_t player1_map8 = (xpm_map_t) personagem_chutar_tras_2_xpm;
static xpm_map_t player1_map9 = (xpm_map_t) personagem_chutar_tras_3_xpm;
static xpm_map_t player1_map10 = (xpm_map_t) personagem_chutar_tras_4_xpm;
static xpm_map_t player2_map0 = (xpm_map_t) personagem_2_parado_xpm;
static xpm_map_t player2_map1 = (xpm_map_t) personagem_2_chutar_frente_1_xpm;
static xpm_map_t player2_map2 = (xpm_map_t) personagem_2_chutar_frente_2_xpm;
static xpm_map_t player2_map3 = (xpm_map_t) personagem_2_chutar_frente_3_xpm;
static xpm_map_t player2_map4 = (xpm_map_t) personagem_2_chutar_frente_4_xpm;
static xpm_map_t player2_map5 = (xpm_map_t) personagem_2_chutar_frente_5_xpm;
static xpm_map_t player2_map6 = (xpm_map_t) personagem_2_chutar_frente_6_xpm;
static xpm_map_t player2_map7 = (xpm_map_t) personagem_2_chutar_tras_1_xpm;
static xpm_map_t player2_map8 = (xpm_map_t) personagem_2_chutar_tras_2_xpm;
static xpm_map_t player2_map9 = (xpm_map_t) personagem_2_chutar_tras_3_xpm;
static xpm_map_t player2_map10 = (xpm_map_t) personagem_2_chutar_tras_4_xpm;

void initialize_player_values(Player *player) {
  player_y_original = player->y;
  player_y = player->y;
  player_x = player->x;
  player_yspeed = player->yspeed;
  player_xspeed = player->xspeed;
  chuta = false;
  printf("player_y_original: %d\n", player_y_original);
}

void prepare_for_player_jump(Player *player) {
  chuta = true;
  time_passed_player_y = 1;
  time_passed_player_x = 1;
}

bool player_border_collision(Player *player) {
  if (player == NULL) return 1;
  if (player->x + player->width >= 800) {
    player->x--;
    return true;
  }
  if (player->x <= 0) {
    player->x++;
    return true;
  }
  return false;
}

bool player_player_collision(Player *player1, Player *player2) {
  if (player1 == NULL || player2 == NULL) {
    return 1;
  }
  if ((player1->x <= player2->x + player2->width + 5) && (player1->x + player1->width + 5 >= player2->x)) {
    if ((player1->y <= player2->y + player2->height + 5) && (player1->y + player1->height + 5 >= player2->y)) {
      if (player1->x < player2->x) {
        player1->x--;
        player2->x++;
      }
      else {
        player1->x++;
        player2->x--;
      }
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

void change_player_y(Player *player) {
  player->y = player_y - ((player->yspeed * time_passed_player_y) - 5 * (time_passed_player_y * time_passed_player_y));
}

void handle_player_jump(Player *player, PlayerStateJump *player_state_jump) {
  if (get_timer_intCounter() % 2 == 0) {

    change_player_y(player);
    time_passed_player_y++;
    time_passed_player_x++;

    if (player->y >= player_y_original) {
      player->y = player_y_original;
      chuta = false;
      *player_state_jump = PLAYER_JUMP_END;
    }
  }
}

int restart_player_movement(Player *player) {
  player->xspeed = player_xspeed;
  player->yspeed = player_yspeed;
  return 0;
}


void move_player(Player *player, PlayerStateMove *player_state_move, PlayerStateMove *player_state_move_temporary, PlayerStateJump *player_state_jump, PlayerStateJump *player_state_jump_temporary, PlayerStateKick *player_state_kick, PlayerStateKick *player_state_kick_temporary) {
  switch (*player_state_move) {
    case PLAYER_MOVE_LEFT_START:
      initialize_player_values(player);
      *player_state_move = PLAYER_MOVE_LEFT;
      break;
    case PLAYER_MOVE_RIGHT_START:
      initialize_player_values(player);
      *player_state_move = PLAYER_MOVE_RIGHT;
      break;
    case PLAYER_MOVE_LEFT:
      if (get_timer_intCounter() % 2 == 0) {
        player->x -= player->xspeed;
      }
      break;

    case PLAYER_MOVE_RIGHT:
      if (get_timer_intCounter() % 2 == 0) {
        player->x += player->xspeed;
      }
      break;
    case AFTER_PLAYER_MOVE_LEFT:
      if (player->xspeed != 0) {
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      } else {
        *player_state_move = PLAYER_MOVE_LEFT_END;
      }
      break;
    case AFTER_PLAYER_MOVE_RIGHT:
      if (player->xspeed != 0) {
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      } else {
        *player_state_move = PLAYER_MOVE_RIGHT_END;
      }
      break;
    case PLAYER_MOVE_LEFT_END:
      restart_player_movement(player);
      *player_state_move = PLAYER_MOVE_NONE;
      break;
    case PLAYER_MOVE_RIGHT_END:
      restart_player_movement(player);
      *player_state_move = PLAYER_MOVE_NONE;
      break;
    case PLAYER_MOVE_NONE:
      if (*player_state_move_temporary != PLAYER_MOVE_NONE) {
        *player_state_move = *player_state_move_temporary;
        *player_state_move_temporary = PLAYER_MOVE_NONE;
      }
      break;
    default:
      break;
  }

  switch(*player_state_jump) {
    case START_PLAYER_JUMP:
      initialize_player_values(player);
      *player_state_jump = BEFORE_PLAYER_JUMP;
      break;
    case BEFORE_PLAYER_JUMP:
      prepare_for_player_jump(player);
      *player_state_jump = player->yspeed > 0 ? PLAYER_JUMP : PLAYER_JUMP_END;
      break;
    case PLAYER_JUMP:
      handle_player_jump(player, player_state_jump);
      break;
    case PLAYER_JUMP_END:
      restart_player_movement(player);
      *player_state_jump = PLAYER_JUMP_NONE;
      break;
    case PLAYER_JUMP_NONE:
      if (*player_state_jump_temporary != PLAYER_JUMP_NONE) {
        *player_state_jump = *player_state_jump_temporary;
        *player_state_jump_temporary = PLAYER_JUMP_NONE;
      }
      break;
    default:
      break;
  }

  switch(*player_state_kick) {
    case PLAYER_KICK_START:
      if (get_timer_intCounter() % 2 == 0) {
        switch(img) {
          case 0:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map7, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map7, XPM_8_8_8, &player->img);
            }
            break;
          case 1:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map8, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map8, XPM_8_8_8, &player->img);
            }
            break;
          case 2:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map9, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map9, XPM_8_8_8, &player->img);
            }
            break;
          case 3:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map10, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map10, XPM_8_8_8, &player->img);
            }
            break;
          default:
            img = 0;
            *player_state_kick = PLAYER_KICK;
            break;
        }
      }
      break;
    case PLAYER_KICK:
      if (get_timer_intCounter() % 2 == 0) {
        switch(img) {
          case 0:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map1, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map1, XPM_8_8_8, &player->img);
            }
            break;
          case 1:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map2, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map2, XPM_8_8_8, &player->img);
            }
            break;
          case 2:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map3, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map3, XPM_8_8_8, &player->img);
            }
            break;
          case 3:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map4, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map4, XPM_8_8_8, &player->img);
            }
            break;
          case 4:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map5, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map5, XPM_8_8_8, &player->img);
            }
            break;
          case 5:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map6, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map6, XPM_8_8_8, &player->img);
            }
            break;
          default:
            img = 0;
            *player_state_kick = AFTER_PLAYER_KICK;
            break;
        }
      }
      break;
    case AFTER_PLAYER_KICK:
      if (get_timer_intCounter() % 2 == 0) {
        switch(img) {
          case 0:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map6, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map6, XPM_8_8_8, &player->img);
            }
            break;
          case 1:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map5, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map5, XPM_8_8_8, &player->img);
            }
            break;
          case 2:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map4, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map4, XPM_8_8_8, &player->img);
            }
            break;
          case 3:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map3, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map3, XPM_8_8_8, &player->img);
            }
            break;
          case 4:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map2, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map2, XPM_8_8_8, &player->img);
            }
            break;
          case 5:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map1, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map1, XPM_8_8_8, &player->img);
            }
            break;
          case 6:
            img++;
            if (player->orientation == 0) {
              player->map = xpm_load(player1_map0, XPM_8_8_8, &player->img);
            } else {
              player->map = xpm_load(player2_map0, XPM_8_8_8, &player->img);
            }
            break;
          default:
            img = 0;
            *player_state_kick = PLAYER_KICK_NONE;
            break;
        }
      }
      break;
    case PLAYER_KICK_NONE:
      if (*player_state_kick_temporary != PLAYER_KICK_NONE) {
        *player_state_kick = *player_state_kick_temporary;
        *player_state_kick_temporary = PLAYER_KICK_NONE;
      }
      break;
    default:
      break;
  }
}
