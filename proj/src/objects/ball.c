#include "ball.h"

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
  bl->stop = false;
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

/*int move_ball(ball *bl) {
  if( bl == NULL )
    return 1;
  bl->x += bl->xspeed;
  bl->y += bl->yspeed;
  return 0;
}*/

int ball_walls_collision(ball *bl, vbe_mode_info_t vmi){
  if(bl == NULL){
    return 1;
  }
  if(bl->x <= 0 || bl->x >= vmi.XResolution){ //hitting side walls
    bl->xspeed = -((bl->xspeed/10)*9); //xspeed reduces by 10% add changes direction
    bl->yspeed = (bl->yspeed/10)*9; //yspeed reduces by 10%
    return 0; 
  }else if(bl->y <= 0 || bl->y >= vmi.YResolution){ //hitting floor or ceiling
    bl->xspeed = (bl->xspeed/10)*9; //xspeed reduces by 10%
    bl->yspeed = -((bl->yspeed/10)*9); //yspeed reduces by 10% and changes direction
    return 0;
  }else{
    return 0;
  }
}
  
int ball_player_collision(ball *bl, player *pl){
  if(bl == NULL || pl == NULL){
    return 1;
  }
  if(player_get_orientation(pl) == 1){  //if pl is the one facing the right
    if((bl->x + bl->width/2) >= (player_get_X(pl) + (player_get_width(pl)*2)/3) &&
    (bl->x <= player_get_X(pl) + player_get_width(pl))){
      if(bl->y + bl->height >= player_get_Y(pl) && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = player_get_header(pl);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed = -player_get_header(pl);
            return 0;
          }else{
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= player_get_header(pl);
            return 0;
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(pl) + player_get_height(pl)/2 && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)){
        if(bl->xspeed < 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_power(pl);
            bl->yspeed *= -player_get_power(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_power(pl);
            bl->yspeed = -player_get_power(pl);
            return 0;
          }else{
            bl->xspeed *= -player_get_power(pl);
            bl->yspeed *= player_get_power(pl);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = player_get_power(pl);
            bl->yspeed *= -player_get_power(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = player_get_power(pl);
            bl->yspeed = -player_get_power(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed *= player_get_power(pl);
            bl->yspeed *= -player_get_power(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else{}
    }else if((bl->x + bl->width/2) > (player_get_X(pl) + player_get_width(pl)/3) &&
    ((bl->x + bl->width/2) < player_get_X(pl) + (player_get_width(pl)*2)/3)){
      if(bl->y + bl->height >= player_get_Y(pl) && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(pl) + player_get_height(pl)/2 && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else{
        return 0; //The ball isn't hitting the player
      }
    }else if(((bl->x + bl->width) >= player_get_X(pl)) && ((bl->x + bl->width/2) <= player_get_X(pl) + player_get_width(pl)/3)){
      if(bl->y + bl->height >= player_get_Y(pl) && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed = -player_get_header(pl);
            return 0;
          }else{
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= player_get_header(pl);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(pl);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(pl) + player_get_height(pl)/2 && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = 9/10;
            bl->yspeed = 9/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -(bl->xspeed*9)/10;
            bl->yspeed *= (bl->yspeed*9)/10;
            return 0;
          }else{
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
            return 0;
          }
        }
      }else{
        return 0; //The ball isn't hitting the player
      }
    }else{
      return 0; //The ball isn't hitting the player
    }
  }else if (player_get_orientation(pl) == 0){
    if((bl->x + bl->width) >= player_get_X(pl) &&
    ((bl->x + bl->width/2) <= player_get_X(pl) + player_get_width(pl)/3)){
      if(bl->y + bl->height >= player_get_Y(pl) && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed = -player_get_header(pl);
            return 0;
          }else{
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= player_get_header(pl);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(pl);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(pl) + player_get_height(pl)/2 && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_power(pl);
            bl->yspeed *= -player_get_power(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_power(pl);
            bl->yspeed = -player_get_power(pl);
            return 0;
          }else{
            bl->xspeed *= -player_get_power(pl);
            bl->yspeed *= player_get_power(pl);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = player_get_power(pl);
            bl->yspeed *= -player_get_power(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = player_get_power(pl);
            bl->yspeed = -player_get_power(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed *= player_get_power(pl);
            bl->yspeed *= -player_get_power(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else{
        return 0; //The ball isn't hitting the player
      }
    }else if((bl->x + bl->width/2) > (player_get_X(pl) + player_get_width(pl)/3) &&
    (bl->x < player_get_X(pl) + (player_get_width(pl)*2)/3)){
      if(bl->y + bl->height >= player_get_Y(pl) && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)/2){
        if(bl->xspeed > 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(pl) + player_get_height(pl)/2 && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)){
        if(bl->xspeed < 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else{}
    }else if(((bl->x + bl->width/2) >= player_get_X(pl) + (player_get_width(pl)*2)/3) && (bl->x <= player_get_X(pl) + player_get_width(pl))){
      if(bl->y + bl->height >= player_get_Y(pl) && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)/2){
        if(bl->xspeed < 0){
          if(bl->yspeed > 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed = -player_get_header(pl);
            return 0;
          }else{
            bl->xspeed *= -player_get_header(pl);
            bl->yspeed *= player_get_header(pl);
            return 0;
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed > 0){
            bl->xspeed = -player_get_header(pl);//artitrable variable, can be changed
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed > 0){
            bl->xspeed *= player_get_header(pl);
            bl->yspeed *= -player_get_header(pl);
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }
      }else if(bl->y + bl->height >= player_get_Y(pl) + player_get_height(pl)/2 && bl->y + bl->height <= player_get_Y(pl) + player_get_height(pl)){
        if(bl->xspeed > 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed = -(bl->yspeed*9)/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else if(bl->xspeed == 0){
          if(bl->yspeed < 0){
            bl->xspeed = (bl->xspeed*9)/10;
            bl->yspeed *= -(bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed = 9/10;
            bl->yspeed = 9/10;
            return 0;
          }else{
            return 1; //ball impossible to be hit by that player at that angle
          }
        }else{
          if(bl->yspeed < 0){
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
            return 0;
          }else if(bl->yspeed == 0){
            bl->xspeed *= -(bl->xspeed*9)/10;
            bl->yspeed *= (bl->yspeed*9)/10;
            return 0;
          }else{
            bl->xspeed = -(bl->xspeed*9)/10;
            bl->yspeed = (bl->yspeed*9)/10;
            return 0;
          }
        }
      }else{
        return 0; //The ball isn't hitting the player
      }
    }else{
      return 0; //The ball isn't hitting the player
    }
  }else{
    return 1;
  }
}

int ball_goal_collision(ball *bl, goal *gl){
  if(bl == NULL || gl == NULL){
    return 1;
  }else{
    if(goal_get_orientation(gl) == 1){ //on the left side of the pitch
      if((bl->y > goal_get_Y(gl)) && (bl->y <= goal_get_height(gl) + goal_get_Y(gl))){//below the
        if((bl->x >= goal_get_X(gl)) && bl->x <= goal_get_X(gl) + goal_get_width(gl)){
          return 0; //a goal was scored
        }else{
          return 0; //not in the goal
        }
      }else if(bl->y == goal_get_Y(gl)){
        if(bl->x >= goal_get_X(gl) && bl->x < goal_get_X(gl) + goal_get_width(gl)){
          bl->xspeed *= 9/10;
          bl->yspeed *= -9/10;
          return 0;
        }else if(bl->x == goal_get_X(gl) + goal_get_width(gl)){
          bl->yspeed *= -9/10;
          if(bl->xspeed < 0){
            bl->xspeed *= -9/10;
          }else{
            bl->xspeed *= 9/10;
          }
          return 0;
        }else{
          return 0; //not in the goal
        }
      }else{
        return 0; //No collision between ball and goal
      }
    }else if(goal_get_orientation(gl) == 0){
      if((bl->y > goal_get_Y(gl)) && (bl->y < goal_get_height(gl) + goal_get_Y(gl))){
        if((bl->x >= goal_get_X(gl)) && bl->x <= goal_get_X(gl) + goal_get_width(gl)){
          return 0; //a goal was scored
        }else{
          return 0; //not in the goal
        }
      }else if(bl->y == goal_get_Y(gl)){
        if(bl->x > goal_get_X(gl) && bl->x <= goal_get_X(gl) + goal_get_width(gl)){
          bl->xspeed *= 9/10;
          bl->yspeed *= -9/10;
          return 0;
        }else if(bl->x == goal_get_X(gl)){
          bl->yspeed *= -9/10;
          if(bl->xspeed < 0){
            bl->xspeed *= -9/10;
          }else{
            bl->xspeed *= 9/10;
          }
          return 0;
        }else{
          return 0; //not in the goal
        }
      }else{
        return 0; //No collision between ball and goal
      }
    }else{
      return 1; //Direction error
    }
  }
}

int detect_collisions(ball *bl, vbe_mode_info_t vmi_p, player *p1, player *p2, goal *gl1, goal *gl2){
  if(bl == NULL || p1 == NULL || p2 == NULL || gl1 == NULL || gl2 == NULL){
    return 1;
  }
  if(player_get_orientation(p1) == player_get_orientation(p2) || goal_get_orientation(gl1) == goal_get_orientation(gl2)){
    return 1;
  }
  if(ball_walls_collision(bl, vmi_p) != 0) {
    return 1;
  }
  if(ball_player_collision(bl, p1) != 0){
    return 1;
  }
  if(ball_player_collision(bl, p2) != 0){
    return 1;
  }
  if(ball_goal_collision(bl, gl1) != 0){
    return 1;
  }
  if(ball_player_collision(bl, gl2) != 0){
    return 1;
  }
}
