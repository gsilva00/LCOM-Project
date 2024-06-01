#ifndef PLAYER_H
#define PLAYER_H

#include <lcom/lcf.h>

typedef struct {
  int x, y;
  int width, height;
  int xspeed, yspeed;
  int power, header; //dictates the strength of the kick and the header
  uint32_t time_in_mov;
  uint8_t *map;
  bool orientation; //bool that tells which direction is facing, if true he is facing the right side of the screen, if false he is facing the right side of the screen
  xpm_image_t img;
} Player;

Player *create_player(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed, uint32_t time_in_mov);

void destroy_player(Player *pl);

//int move_player(player *pl);

int player_get_X(Player *pl);

int player_get_Y(Player *pl);

int player_get_xspeed(Player *pl);

int player_get_yspeed(Player *pl);

int player_get_width(Player *pl);

int player_get_height(Player *pl);

int player_get_orientation(Player *pl);

int player_get_power(Player *pl);

int player_get_header(Player *pl);

void move_player1_to_center(Player *pl);

void move_player2_to_center(Player *pl);

void player_set_image(Player *pl, xpm_map_t pic);

#endif
