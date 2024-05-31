#include "timeboard.h"

timeboard *create_timeboard(xpm_map_t pic) {
  timeboard *tb = (timeboard *) malloc ( sizeof(timeboard));
  if(tb == NULL){
    return NULL;
  }
  xpm_image_t img;
  tb->map = xpm_load(pic, XPM_8_8_8,&img);
  
  if( tb->map == NULL ) {
    free(tb);
    return NULL;
  }
  tb->x = 0;
  tb->y = 0;
  tb->img = img;
  tb->time = 300;
  return tb;
}

void destroy_timeboard(timeboard *tb) {
  if(tb == NULL)
    return;
  if(tb->map )
    free(tb->map);
  free(tb);
  tb = NULL;
}

void add_time(timeboard *tb){
  if (tb->time > 0  && (get_timer_intCounter() % 60 == 0)){
    tb->time -= 1;
  }
}
