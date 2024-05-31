#ifndef PLAYER_H
#define PLAYER_H

#include <lcom/lcf.h>

struct Player{
  int x, y;
  int width, height;
  int xspeed, yspeed;
  int power, header; //dictates the strength of the kick and the header
  uint32_t time_in_mov;
  uint8_t *map;
  bool orientation; //bool that tells which direction is facing, if true he is facing the right side of the screen, if false he is facing the right side of the screen
  xpm_image_t img;
};

typedef struct Player player;

player *create_player(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed, uint32_t time_in_mov);

void destroy_player(player *pl);

//int move_player(player *pl);

int player_get_X(player *pl);

int player_get_Y(player *pl);

int player_get_xspeed(player *pl);

int player_get_yspeed(player *pl);

int player_get_width(player *pl);

int player_get_height(player *pl);

int player_get_orientation(player *pl);

int player_get_power(player *pl);

int player_get_header(player *pl);

void move_player1_to_center(player *pl);

void move_player2_to_center(player *pl);

#endif
