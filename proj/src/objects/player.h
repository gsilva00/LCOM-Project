#ifndef PLAYER_H
#define PLAYER_H

struct Player;
typedef struct Player player;

player *create_player(char *pic[], int x, int y, int xspeed, int yspeed);

void destroy_player(player *pl);

int move_player(player *pl);

int player_get_X(player *pl);

int player_get_Y(player *pl);

int player_get_width(player *pl);

int player_get_height(player *pl);

bool player_get_orientation(player *pl);

int player_get_power(player *pl);

int player_get_header(player *pl);
#endif