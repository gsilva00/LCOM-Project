#include "player.h"

player *create_player(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed, uint32_t time_in_mov) {
  player *pl = (player *) malloc ( sizeof(player));
  if(pl == NULL){
    return NULL;
  }
  xpm_image_t img;
  // read the player pixmap
  pl->map = xpm_load(pic, XPM_8_8_8,&img);
  
  if( pl->map == NULL ) {
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

void destroy_player(player *pl) {
  if( pl == NULL )
    return;
  if( pl->map )
    free(pl->map);
  free(pl);
  pl = NULL;
}

/*int move_player(player *pl) {
  if( pl == NULL )
    return 1;
  pl->x += pl->xspeed;
  pl->y += pl->yspeed;
  return 0;
}*/

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

int player_get_xspeed(player *pl){
  if(pl == NULL){
    return -1;
  }else{
    return pl->xspeed;
  }
}

int player_get_yspeed(player *pl){
  if(pl == NULL){
    return -1;
  }else{
    return pl->yspeed;
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

void move_player1_to_center(player *pl){
  if(pl == NULL){
    return;
  }
  pl->x = 200;
  pl->y = 455;
}

void move_player2_to_center(player *pl){
  if(pl == NULL){
    return;
  }
  pl->x = 600;
  pl->y = 455;
}

void player_set_image(player *pl, xpm_map_t pic){
  if(pl == NULL){
    return;
  }
  xpm_image_t img;
  pl->map = xpm_load(pic, XPM_8_8_8,&img);
  pl->img = img;
}
