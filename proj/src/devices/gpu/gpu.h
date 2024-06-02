#ifndef GPU_H
#define GPU_H

#include <lcom/lcf.h>

#include <stdint.h>

#include "../../images/numbers/0.xpm"
#include "../../images/numbers/1.xpm"
#include "../../images/numbers/2.xpm"
#include "../../images/numbers/3.xpm"
#include "../../images/numbers/4.xpm"
#include "../../images/numbers/5.xpm"
#include "../../images/numbers/6.xpm"
#include "../../images/numbers/7.xpm"
#include "../../images/numbers/8.xpm"
#include "../../images/numbers/9.xpm"


/**
 * @brief Changes the video mode to the specified mode.
 * 
 * @param mode video mode to change to.
 * @return 0 on success, non-zero otherwise.
 */
int change_video_mode(uint16_t mode);

/**
 * @brief Creates a frame buffer for the specified VBE mode.
 *
 * @param mode VBE mode to set.
 * @return 0 on success, non-zero otherwise.
 */
int create_frame_buffer(uint16_t mode);

/**
 * @brief Function to deallocate buffers from memory
 * 
 * @return 0 on success, non-zero otherwise
*/
int free_buffers();

/**
 * @brief Function to initialize third buffer that stores (unchanging) the background of the game
 * @details Calls the draw_to_background() function
 * 
 * @param x_coords, y_coords Arrays with the coordinates of the game's background sprites
 * @param imgs Array with the images (with type xpm_image_t)
 * @param num_elements Number of elements to draw 
 * @return 0 on success, non-zero otherwise
*/
int init_background(uint16_t *x_coords, uint16_t *y_coords, xpm_image_t *imgs, size_t num_elements);

/**
 * @brief Auxiliary function to init_background()
 * @details 
 * 
 * @param xi, yi Coordinates of the upper right corner of the image
 * @param img Image of the object
 * @return 0 on success, non-zero otherwise
*/
int draw_to_background(uint16_t xi, uint16_t yi, xpm_image_t img);

/**
 * @brief Function to draw small numbers to the third buffer
 * 
 * @param xi, yi Coordinates of the upper left corner of the image
 * @param points1 Image of the number
 * @return 0 on success, non-zero otherwise
*/
int draw_nums_to_background(uint16_t xi, uint16_t yi, uint8_t points1);

/**
 * @brief Draws a single pixel into the second_buffer
 * @details Calculates the position in the buffer and copies the color to the double buffer
 * 
 * @param x, y Coordinates of the pixel
 * @param color Color of the pixel
 * @param ignore Bool indicating if should ignore the collision with the borders of the screen 
 * @return 0 on success, non-zero otherwise
 * 
*/
int draw_pixel(uint16_t x, uint16_t y, uint32_t color, bool ignore);

/**
 * @brief
 * 
 * @param xi, yi Coordinates of the upper left corner of the image
 * @param img
 * @param ignore
 * @return 0 on success, non-zero otherwise
*/
int draw_xpm(uint16_t xi, uint16_t yi, xpm_image_t img, bool ignore);

/**
 * @brief Copy the entirety of the contents from the third buffer to the second buffer
 * @details First step to then draw the rest of the second buffer
*/
void prep_next_frame();

/**
 * @brief Copy the entirety of the contents from the second buffer to the video memory.
 * @details Last step after drawing everything in the second buffer. Effectively shows things on screen
*/
void end_next_frame();


/**
 * @brief Gets the pointer to the video memory.
 * 
 * @return Pointer to the video memory.
 */
uint8_t* get_video_mem();

/**
 * @brief Retrieves the general VBE mode information.
 *
 * @return VBE mode information.
 */
vbe_mode_info_t get_vbe_mode_info();

/**
 * @brief Gets the horizontal resolution of the screen.
 * 
 * @return Horizontal resolution of the frame buffer.
 */
unsigned get_hres();

/**
 * @brief Gets the vertical resolution of the screen.
 * 
 * @return Vertical resolution of the frame buffer.
 */
unsigned get_vres();

/**
 * @brief Gets the number of bytes per pixel.
 * 
 * @return Number of bytes per pixel.
 */
unsigned get_bytes_per_pixel();


#endif
