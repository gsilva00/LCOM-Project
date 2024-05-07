#ifndef PLAYER_H
#define PLAYER_H

#include <lcom/lcf.h>

struct Player{
  int x, y;
  int width, height;
  int xspeed, yspeed;
  int power, header; //dictates the strength of the kick and the header
  uint8_t *map;
  bool orientation; //bool that tells which direction is facing, if true he is facing the right side of the screen, if false he is facing the right side of the screen
  xpm_image_t img;
};

typedef struct Player player;

player *create_player(xpm_map_t pic, int x, int y, int xspeed, int yspeed);

void destroy_player(player *pl);

//int move_player(player *pl);

int player_get_X(player *pl);

int player_get_Y(player *pl);

int player_get_width(player *pl);

int player_get_height(player *pl);

int player_get_orientation(player *pl);

int player_get_power(player *pl);

int player_get_header(player *pl);
#endif
