#include "scoreboard.h"

Scoreboard *create_scoreboard(xpm_map_t pic) {
  Scoreboard *sc = (Scoreboard *) malloc (sizeof(Scoreboard));
  if (sc == NULL) return NULL;

  xpm_image_t img;
  sc->map = xpm_load(pic, XPM_8_8_8, &img);
  
  if (sc->map == NULL) {
    free(sc);
    return NULL;
  }

  sc->x = 400 - img.width/2;
  sc->y = 0;
  sc->img = img;
  sc->points1 = 0;
  sc->points2 = 0;

  return sc;
}

void destroy_scoreboard(Scoreboard *sc) {
  if (sc == NULL) return;
  if (sc->map ) free(sc->map);
  free(sc);
  sc = NULL;
}

void add_points_1(Scoreboard *sc) {
  sc->points1 += 1; 
}

void add_points_2(Scoreboard *sc) {
  sc->points2 += 1; 
}

