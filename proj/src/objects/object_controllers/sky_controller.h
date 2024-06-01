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
 * 
 * @return 0 upon success, non-zero otherwise
 */
int change_sky_image(uint8_t current_hour);

/**
 * @brief Getter for the current_sky static global variable
 * @details Getter is needed because of the use of the static keyword (makes the variable only accesible in the file it's declared). Provides encapsulation
 * 
 * @return Address of allocated memory where the XPM image was read
 */
uint8_t* get_current_sky();


#endif // SKY_H
