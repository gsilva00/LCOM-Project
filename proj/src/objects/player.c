#include "player.h"

struct Player{
  int x, y;
  int width, height;
  int xspeed, yspeed;
  int power, header; //dictates the strength of the kick and the header
  uint8_t *map;
  bool orientation; //bool that tells which direction is facing, if true he is facing the right side of the screen, if false he is facing the right side of the screen
};

player *create_player(xpm_map_t pic, int x, int y, int xspeed, int yspeed) {
  //allocate space for the "object"
  player *pl = (player *) malloc ( sizeof(player));
  if(pl == NULL)
    return NULL;
   xpm_image_t img;
  // read the player pixmap
  pl->map =  xpm_load(pic, XPM_8_8_8,&img);
  if( pl->map == NULL ) {
    free(pl);
    return NULL;
  }
  pl->x = x;
  pl->y = y;
  pl->xspeed = xspeed;
  pl->yspeed = yspeed;
  return pl;
}

void destroy_player(player *pl) {
  if( pl == NULL )
    return;
  if( pl->map )
    free(pl->map);
  free(pl);
  pl = NULL;
}

int move_player(player *pl) {
  if( pl == NULL )
    return 1;
  pl->x += pl->xspeed;
  pl->y += pl->yspeed;
  return 0;
}

int player_get_X(player *pl) {
  if(pl == NULL){
    return -1;
  }else{
    return pl->x;
  }
}

int player_get_Y(player *pl) {
  if(pl == NULL){
    return -1;
  }else{
    return pl->y;
  }
}

int player_get_width(player *pl) {
  if(pl == NULL){
    return -1;
  }else{
    return pl->width;
  }
}

int player_get_height(player *pl) {
  if(pl == NULL){
    return -1;
  }else{
    return pl->height;
  }
}

int player_get_orientation(player *pl){
  if(pl == NULL) {
    return -1;
  }else{
    return pl->orientation;
  }
}

int player_get_power(player *pl){
  if(pl == NULL){
    return -1;
  }else{
    return pl->power;
  }
}

int player_get_header(player *pl){
  if(pl == NULL){
    return -1;
  }else{
    return pl->header;
  }
}
