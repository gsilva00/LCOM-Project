#include "player.h"


Player *create_player(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed, bool orientation) {
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
  pl->orientation = orientation;

  return pl;
}

void destroy_player(Player *pl) {
  if (pl == NULL) return;
  if (pl->map) free(pl->map);
  free(pl);
  pl = NULL;
}

void move_player1_to_center(Player *pl) {
  if (pl == NULL) return;
  
  pl->x = 200 - pl->width;
  pl->y = 455;
}

void move_player2_to_center(Player *pl) {
  if (pl == NULL) return;

  pl->x = 600;
  pl->y = 455;
}
