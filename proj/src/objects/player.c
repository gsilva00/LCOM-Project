#include "player.h"


Player *create_player(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed, uint32_t time_in_mov) {
  Player *pl = (Player *) malloc (sizeof(Player));
  if (pl == NULL) return NULL;

  xpm_image_t img;
  // read the player pixmap
  pl->map = xpm_load(pic, XPM_8_8_8, &img);
  
  if (pl->map == NULL) {
    free(pl);
    return NULL;
  }

  pl->x = x;
  pl->y = y;
  pl->width = width;
  pl->height = height;
  pl->xspeed = xspeed;
  pl->yspeed = yspeed;
  pl->img = img;

  return pl;
}

void destroy_player(Player *pl) {
  if (pl == NULL) return;
  if (pl->map) free(pl->map);
  free(pl);
  pl = NULL;
}

/*int move_player(player *pl) {
  if ( pl == NULL )
    return 1;
  pl->x += pl->xspeed;
  pl->y += pl->yspeed;
  return 0;
}*/

int player_get_X(Player *pl) {
  if (pl == NULL) return -1;

  return pl->x;
}

int player_get_Y(Player *pl) {
  if (pl == NULL) return -1;
  
  return pl->y;
}

int player_get_xspeed(Player *pl) {
  if (pl == NULL) return -1;
  
  return pl->xspeed;
}

int player_get_yspeed(Player *pl) {
  if (pl == NULL) return -1;
  
  return pl->yspeed;
}

int player_get_width(Player *pl) {
  if (pl == NULL) return -1;
  
  return pl->width;
}

int player_get_height(Player *pl) {
  if (pl == NULL) return -1;
  
  return pl->height;
}

int player_get_orientation(Player *pl) {
  if (pl == NULL) return -1;

  return pl->orientation;
}

int player_get_power(Player *pl) {
  if (pl == NULL) return -1;

  return pl->power;
}

int player_get_header(Player *pl) {
  if (pl == NULL) return -1;

  return pl->header;
}

void move_player1_to_center(Player *pl) {
  if (pl == NULL) return;
  
  pl->x = 200;
  pl->y = 455;
}

void move_player2_to_center(Player *pl) {
  if (pl == NULL) return;

  pl->x = 600;
  pl->y = 455;
}

void player_set_image(Player *pl, xpm_map_t pic) {
  if (pl == NULL) return;

  xpm_image_t img;
  pl->map = xpm_load(pic, XPM_8_8_8, &img);
  pl->img = img;
}
