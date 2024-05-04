#include <lcom/lcf.h>
#include "ball.h"
#include "player.h"

ball *create_ball(xpm_map_t pic, int x, int y, int xspeed, int yspeed, uint32_t time_in_mov) {
  //allocate space for the "object"
  ball *bl = (ball *) malloc ( sizeof(ball));
  if(bl == NULL){
    return NULL;
  }
  xpm_image_t img;
  // read the ball pixmap
  bl->map = xpm_load(pic, XPM_8_8_8,&img);
  if( bl->map == NULL ) {
    free(bl);
    return NULL;
  }
  bl->x = x;
  bl->y = y;
  bl->xspeed = xspeed;
  bl->yspeed = yspeed;
  bl->img = img;
  return bl;
}

void destroy_ball(ball *bl) {
  if(bl == NULL)
    return;
  if(bl->map )
    free(bl->map);
  free(bl);
  bl = NULL;
}

int move_ball(ball *bl) {
  if( bl == NULL )
    return 1;
  bl->x += bl->xspeed;
  bl->y += bl->yspeed;
  return 0;
}

int ball_collision(ball *bl, vbe_mode_info_t vmi, player *p1, player *p2){
  if(bl->x <= 0 || bl->x >= vmi.XResolution){ //hitting side walls
    bl->xspeed = -((bl->xspeed/10)*9); //xspeed reduces by 10% add changes direction
    bl->yspeed = (bl->yspeed/10)*9; //yspeed reduces by 10%
    return 0; 
  }else if(bl->y <= 0 || bl->y >= vmi.YResolution){ //hitting floor or ceiling
    bl->xspeed = (bl->xspeed/10)*9; //xspeed reduces by 10%
    bl->yspeed = -((bl->yspeed/10)*9); //yspeed reduces by 10% and changes direction
    return 0;
  }else if(player_get_orientation(p1) == 1 && player_get_orientation(p2) == 0){  //if p1 is the one facing the right
    if((bl->x + bl->width/2) >= (player_get_X(p1) + (player_get_width(p1)*2)/3) &&
    (bl->x <= player_get_X(p1) + player_get_width(p1))){
      if(bl->y + bl->height >= player_get_Y(p1) && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = player_get_header(p1);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed = -player_get_header(p1);
            return 0;
          }else{
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= player_get_header(p1);
            return 0;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p1) + player_get_height(p1)/2 && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)){
        if(bl->xspeed < 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_power(p1);
            bl->yspeed *= -player_get_power(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_power(p1);
            bl->yspeed = -player_get_power(p1);
            return 0;
          }else{
            bl->xspeed *= -player_get_power(p1);
            bl->yspeed *= player_get_power(p1);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = player_get_power(p1);
            bl->yspeed *= -player_get_power(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = player_get_power(p1);
            bl->yspeed = -player_get_power(p1);
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed *= player_get_power(p1);
            bl->yspeed *= -player_get_power(p1);
          }else{
            return 1;
          }
        }
      }else{}
    }else if((bl->x + bl->width/2) > (player_get_X(p1) + player_get_width(p1)/3) &&
    ((bl->x + bl->width/2) < player_get_X(p1) + (player_get_width(p1)*2)/3)){
      if(bl->y + bl->height >= player_get_Y(p1) && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p1) + player_get_height(p1)/2 && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
          }else{
            return 1;
          }
        }
      }else{}
    }else if(((bl->x + bl->width) >= player_get_X(p1)) && ((bl->x + bl->width/2) <= player_get_X(p1) + player_get_width(p1)/3)){
      if(bl->y + bl->height >= player_get_Y(p1) && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed = -player_get_header(p1);
          }else{
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= player_get_header(p1);
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(p1);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p1) + player_get_height(p1)/2 && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = 9/10;
            bl->yspeed = 9/10;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -(bl->xspeed*9)/10;
            bl->yspeed *= (bl->yspeed*9)/10;
          }else{
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }
        }
      }else{}
    }else if((bl->x + bl->width) >= player_get_X(p2) &&
    ((bl->x + bl->width/2) <= player_get_X(p2) + player_get_width(p2)/3)){
      if(bl->y + bl->height >= player_get_Y(p2) && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed = -player_get_header(p2);
          }else{
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= player_get_header(p2);
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(p2);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p2) + player_get_height(p2)/2 && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_power(p2);
            bl->yspeed *= -player_get_power(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_power(p2);
            bl->yspeed = -player_get_power(p2);
            return 0;
          }else{
            bl->xspeed *= -player_get_power(p2);
            bl->yspeed *= player_get_power(p2);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = player_get_power(p2);
            bl->yspeed *= -player_get_power(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = player_get_power(p2);
            bl->yspeed = -player_get_power(p2);
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed *= player_get_power(p2);
            bl->yspeed *= -player_get_power(p2);
          }else{
            return 1;
          }
        }
      }else{}
    }else if((bl->x + bl->width/2) > (player_get_X(p2) + player_get_width(p2)/3) &&
    (bl->x < player_get_X(p2) + (player_get_width(p2)*2)/3)){
      if(bl->y + bl->height >= player_get_Y(p2) && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p2) + player_get_height(p2)/2 && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
          }else{
            return 1;
          }
        }
      }else{}
    }else if(((bl->x + bl->width/2) >= player_get_X(p2) + (player_get_width(p2)*2)/3) && (bl->x <= player_get_X(p2) + player_get_width(p2))){
      if(bl->y + bl->height >= player_get_Y(p2) && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)/2){
        if(bl->xspeed < 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed = -player_get_header(p2);
          }else{
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= player_get_header(p2);
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(p2);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p2) + player_get_height(p2)/2 && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)){
        if(bl->xspeed > 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = 9/10;
            bl->yspeed = 9/10;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -(bl->xspeed*9)/10;
            bl->yspeed *= (bl->yspeed*9)/10;
          }else{
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }
        }
      }else{}
    }
  }else if(player_get_orientation(p1) == 0 && player_get_orientation(p2) == 1){  //if p1 is the one facing the right
    if((bl->x + bl->width/2) >= (player_get_X(p2) + (player_get_width(p2)*2)/3) &&
    (bl->x <= player_get_X(p2) + player_get_width(p2))){
      if(bl->y + bl->height >= player_get_Y(p2) && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = player_get_header(p2);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed = -player_get_header(p2);
            return 0;
          }else{
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= player_get_header(p2);
            return 0;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p2) + player_get_height(p2)/2 && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)){
        if(bl->xspeed < 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_power(p2);
            bl->yspeed *= -player_get_power(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_power(p2);
            bl->yspeed = -player_get_power(p2);
            return 0;
          }else{
            bl->xspeed *= -player_get_power(p2);
            bl->yspeed *= player_get_power(p2);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = player_get_power(p2);
            bl->yspeed *= -player_get_power(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = player_get_power(p2);
            bl->yspeed = -player_get_power(p2);
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed *= player_get_power(p2);
            bl->yspeed *= -player_get_power(p2);
          }else{
            return 1;
          }
        }
      }else{}
    }else if((bl->x + bl->width/2) > (player_get_X(p2) + player_get_width(p2)/3) &&
    ((bl->x + bl->width/2) < player_get_X(p2) + (player_get_width(p2)*2)/3)){
      if(bl->y + bl->height >= player_get_Y(p2) && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p2) + player_get_height(p2)/2 && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
          }else{
            return 1;
          }
        }
      }else{}
    }else if(((bl->x + bl->width) >= player_get_X(p2)) && ((bl->x + bl->width/2) <= player_get_X(p2) + player_get_width(p2)/3)){
      if(bl->y + bl->height >= player_get_Y(p2) && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed = -player_get_header(p2);
          }else{
            bl->xspeed *= -player_get_header(p2);
            bl->yspeed *= player_get_header(p2);
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(p2);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p2);
            bl->yspeed *= -player_get_header(p2);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p2) + player_get_height(p2)/2 && bl->y + bl->height <= player_get_Y(p2) + player_get_height(p2)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = 9/10;
            bl->yspeed = 9/10;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -(bl->xspeed*9)/10;
            bl->yspeed *= (bl->yspeed*9)/10;
          }else{
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }
        }
      }else{}
    }else if((bl->x + bl->width) >= player_get_X(p1) &&
    ((bl->x + bl->width/2) <= player_get_X(p1) + player_get_width(p1)/3)){
      if(bl->y + bl->height >= player_get_Y(p1) && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed = -player_get_header(p1);
          }else{
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= player_get_header(p1);
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(p1);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p1) + player_get_height(p1)/2 && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_power(p1);
            bl->yspeed *= -player_get_power(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_power(p1);
            bl->yspeed = -player_get_power(p1);
            return 0;
          }else{
            bl->xspeed *= -player_get_power(p1);
            bl->yspeed *= player_get_power(p1);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = player_get_power(p1);
            bl->yspeed *= -player_get_power(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = player_get_power(p1);
            bl->yspeed = -player_get_power(p1);
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed *= player_get_power(p1);
            bl->yspeed *= -player_get_power(p1);
          }else{
            return 1;
          }
        }
      }else{}
    }else if((bl->x + bl->width/2) > (player_get_X(p1) + player_get_width(p1)/3) &&
    (bl->x < player_get_X(p1) + (player_get_width(p1)*2)/3)){
      if(bl->y + bl->height >= player_get_Y(p1) && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p1) + player_get_height(p1)/2 && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
          }else{
            return 1;
          }
        }
      }else{}
    }else if(((bl->x + bl->width/2) >= player_get_X(p1) + (player_get_width(p1)*2)/3) && (bl->x <= player_get_X(p1) + player_get_width(p1))){
      if(bl->y + bl->height >= player_get_Y(p1) && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)/2){
        if(bl->xspeed < 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed = -player_get_header(p1);
          }else{
            bl->xspeed *= -player_get_header(p1);
            bl->yspeed *= player_get_header(p1);
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(p1);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(p1);
            bl->yspeed *= -player_get_header(p1);
            return 0;
          }else{
            return 1;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(p1) + player_get_height(p1)/2 && bl->y + bl->height <= player_get_Y(p1) + player_get_height(p1)){
        if(bl->xspeed > 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = 9/10;
            bl->yspeed = 9/10;
          }else{
            return 1;
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -(bl->xspeed*9)/10;
            bl->yspeed *= (bl->yspeed*9)/10;
          }else{
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
          }
        }
      }else{}
    }
  }else{
    return 1; //Direction error
  }
  return 0; //return 1????0???? too long to check
}
