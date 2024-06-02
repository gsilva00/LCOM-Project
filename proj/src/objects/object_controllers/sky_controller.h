#ifndef SKY_H
#define SKY_H

#include <lcom/lcf.h>

#include <stdint.h>

#include "../../images/backgrounds/sky_5am.xpm"
#include "../../images/backgrounds/sky_7am.xpm"
#include "../../images/backgrounds/sky_10am.xpm"
#include "../../images/backgrounds/sky_12pm.xpm"
#include "../../images/backgrounds/sky_4pm.xpm"
#include "../../images/backgrounds/sky_7pm.xpm"
#include "../../images/backgrounds/sky_8pm.xpm"
#include "../../images/backgrounds/sky_10pm.xpm"


/**
 * @brief Function to change the sky background based on the real-time of the computer.
 * @param current_hour Current hour to determine the background image
 * @param sky_img Pointer to store the newly loaded sky image
 * 
 * @return 0 upon success, non-zero otherwise
 */
int change_sky_image(uint8_t current_hour, xpm_image_t *sky_img);


#endif // SKY_H
