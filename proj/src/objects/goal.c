#include "goal.h"

/*struct Goal{
  int x, y;
  int width, height;
  int orientation; //if 1 the goal is facing the right side
  char *map;
};*/

goal *create_goal(xpm_map_t pic) {
  //allocate space for the "object"
  goal *gl = (goal *) malloc ( sizeof(goal));
  if(gl == NULL)
    return NULL;

  xpm_image_t img;
  gl->map = xpm_load(pic, XPM_8_8_8,&img);
  
  if( gl->map == NULL ) {
    free(gl);
    return NULL;
  }
  gl->img = img;
  return gl;
}

/*void destroy_goal(goal *gl) {
  if( gl == NULL )
    return;
  if( gl->map )
    free(gl->map);
  free(gl);
  gl = NULL;
}

int goal_get_X(goal *gl){
  if(gl == NULL){
    return -1;
  }else{
    return gl->x;
  }
}

int goal_get_Y(goal *gl){
  if(gl == NULL){
    return 1;
  }else{
    return gl->y;
  }
}

int goal_get_width(goal *gl){
  if(gl == NULL){
    return -1;
  }else{
    return gl->width;
  }
}

int goal_get_height(goal *gl){
  if(gl == NULL){
    return -1;
  }else{
    return gl->height;
  }
}

int goal_get_orientation(goal *gl){
  if(gl == NULL){
    return -1;
  }else{
    return gl->orientation;
  }
}*/
