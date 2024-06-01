#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 200
#define BARRIER_START 0
#define BARRIER_END 20
#define BARRIER_START_1 780
#define BARRIER_END_1 800
#define SPEED_REDUCTION_FACTOR 0.65
#define BOUNCE_SPEED_REDUCTION_FACTOR 0.85
#define BOUNCE_OFFSET 50

#include <lcom/lcf.h>
#include <stdint.h>
#include <objects/player.h>
#include <devices/timer/aux_timer.h>
#include <devices/gpu/gpu.h>
#include "objects/xpm/personagem_2_parado.xpm"
#include "objects/xpm/personagem_parado.xpm"
#include "objects/xpm/personagem_chutar_frente_1.xpm"
#include "objects/xpm/personagem_chutar_frente_2.xpm"
#include "objects/xpm/personagem_chutar_frente_3.xpm"
#include "objects/xpm/personagem_chutar_frente_4.xpm"
#include "objects/xpm/personagem_chutar_frente_5.xpm"
#include "objects/xpm/personagem_chutar_frente_6.xpm"
#include "objects/xpm/personagem_chutar_tras_1.xpm"
#include "objects/xpm/personagem_chutar_tras_2.xpm"
#include "objects/xpm/personagem_chutar_tras_3.xpm"
#include "objects/xpm/personagem_chutar_tras_4.xpm"

  int player_y_original;
  int player_y;
  int player_x;
  int player_yspeed;
  int player_xspeed;
  int bounce_offset;
  bool chuta;

  typedef enum {
    STATE_PLAYER_JUMP_NONE,
    STATE_START_PLAYER_JUMP,
    STATE_BEFORE_PLAYER_JUMP,
    STATE_PLAYER_JUMP,
    STATE_AFTER_PLAYER_JUMP,
    STATE_PLAYER_JUMP_END,
  } PlayerStateJump;

  typedef enum {
    STATE_PLAYER_MOVE_NONE,
    STATE_PLAYER_MOVE_LEFT_START,
    STATE_PLAYER_MOVE_RIGHT_START,
    STATE_PLAYER_MOVE_LEFT,
    STATE_PLAYER_MOVE_RIGHT,
    STATE_AFTER_PLAYER_MOVE_LEFT,
    STATE_AFTER_PLAYER_MOVE_RIGHT,
    STATE_PLAYER_MOVE_LEFT_END,
    STATE_PLAYER_MOVE_RIGHT_END,
  } PlayerStateMove;

  typedef enum {
    STATE_PLAYER_KICK_NONE,
    STATE_PLAYER_KICK_START,
    STATE_PLAYER_KICK,
    STATE_AFTER_PLAYER_KICK,
  } PlayerStateKick;

void(move_player)(player *player, PlayerStateMove *player_state_move, PlayerStateMove *player_state_move_temporary, PlayerStateJump *player_state_jump, PlayerStateJump *player_state_jump_temporary, PlayerStateKick *player_state_kick, PlayerStateKick *player_state_kick_temporary);

bool check_player_border(player *player);

bool check_multiplayer_border(player *player1, player *player2);

#endif
