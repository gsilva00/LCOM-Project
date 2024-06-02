#include "ball.h"


Ball *create_ball(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed) {
  Ball *bl = (Ball *) malloc ( sizeof(Ball));
  if (bl == NULL) return NULL;

  xpm_image_t img;
  // read the ball pixmap
  bl->map = xpm_load(pic, XPM_8_8_8, &img);
  
  if (bl->map == NULL) {
    free(bl);
    return NULL;
  }

  bl->img = img;
  bl->x = 400 - bl->img.width;
  bl->y = y;
  bl->width = width;
  bl->height = height;
  bl->xspeed = xspeed;
  bl->yspeed = yspeed;
  bl->stop = false;

  return bl;
}

void destroy_ball(Ball *bl) {
  if (bl == NULL) return;
  if (bl->map) free(bl->map);
  free(bl);
  bl = NULL;
}

void move_ball_to_center(Ball *bl) {
  bl->x = 400 - bl->img.width;
  bl->y = 490;
  bl->stop = false;
  return;
}
