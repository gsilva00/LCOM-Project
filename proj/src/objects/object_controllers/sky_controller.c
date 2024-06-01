#include "sky_controller.h"


static uint8_t *sky_map;


int change_sky_image(uint8_t current_hour) {
  printf("%s called!\n", __func__);
  char **next_xpm = NULL;

  if (current_hour == 5) next_xpm = sky_5am_xpm;
  else if (current_hour == 7) next_xpm = sky_7am_xpm;
  else if (current_hour == 10) next_xpm = sky_10am_xpm;
  else if (current_hour == 12) next_xpm = sky_12am_xpm;
  else if (current_hour == 16) next_xpm = sky_4pm_xpm;
  else if (current_hour == 19) next_xpm = sky_7pm_xpm;
  else if (current_hour == 20) next_xpm = sky_8pm_xpm;
  else if (current_hour == 22) next_xpm = sky_10pm_xpm;

  if (next_xpm != NULL) { // If a new sky needs to be loaded
    xpm_image_t img; // Don't need the image information loaded here.
    xpm_map_t next_map = (xpm_map_t) next_xpm;

    sky_map = xpm_load(next_map, XPM_8_8_8, &img);
    if (sky_map == NULL) {
      printf("Error while loading the new sky's XPM\n");
      return 1;
    }
  }

  return 0;
}


uint8_t* get_current_sky() {
  return sky_map;
}
