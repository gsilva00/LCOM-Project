#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include <lcom/lcf.h>

#include <stdint.h>

#include <objects/player.h>
#include <devices/timer/aux_timer.h>
#include <devices/gpu/gpu.h>

#include "../../images/objects/personagem_2_parado.xpm"
#include "../../images/objects/personagem_parado.xpm"
#include "../../images/objects/personagem_chutar_frente_1.xpm"
#include "../../images/objects/personagem_chutar_frente_2.xpm"
#include "../../images/objects/personagem_chutar_frente_3.xpm"
#include "../../images/objects/personagem_chutar_frente_4.xpm"
#include "../../images/objects/personagem_chutar_frente_5.xpm"
#include "../../images/objects/personagem_chutar_frente_6.xpm"
#include "../../images/objects/personagem_chutar_tras_1.xpm"
#include "../../images/objects/personagem_chutar_tras_2.xpm"
#include "../../images/objects/personagem_chutar_tras_3.xpm"
#include "../../images/objects/personagem_chutar_tras_4.xpm"


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 200
#define BARRIER_START 0
#define BARRIER_END 20
#define BARRIER_START_1 780
#define BARRIER_END_1 800
#define SPEED_REDUCTION_FACTOR 0.65
#define BOUNCE_SPEED_REDUCTION_FACTOR 0.85
#define BOUNCE_OFFSET 50


int player_y_original;
int player_y;
int player_x;
int player_yspeed;
int player_xspeed;
int bounce_offset;
bool chuta;

typedef enum {
  PLAYER_JUMP_NONE,
  START_PLAYER_JUMP,
  BEFORE_PLAYER_JUMP,
  PLAYER_JUMP,
  AFTER_PLAYER_JUMP,
  PLAYER_JUMP_END,
} PlayerStateJump;

typedef enum {
  PLAYER_MOVE_NONE,
  PLAYER_MOVE_LEFT_START,
  PLAYER_MOVE_RIGHT_START,
  PLAYER_MOVE_LEFT,
  PLAYER_MOVE_RIGHT,
  AFTER_PLAYER_MOVE_LEFT,
  AFTER_PLAYER_MOVE_RIGHT,
  PLAYER_MOVE_LEFT_END,
  PLAYER_MOVE_RIGHT_END,
} PlayerStateMove;

typedef enum {
  PLAYER_KICK_NONE,
  PLAYER_KICK_START,
  PLAYER_KICK,
  AFTER_PLAYER_KICK,
} PlayerStateKick;

void move_player(Player *player, PlayerStateMove *player_state_move, PlayerStateMove *player_state_move_temporary, PlayerStateJump *player_state_jump, PlayerStateJump *player_state_jump_temporary, PlayerStateKick *player_state_kick, PlayerStateKick *player_state_kick_temporary);

bool check_player_border(Player *player);

bool check_multiplayer_border(Player *player1, Player *player2);

#endif
