#ifndef GRAPHIC_UTILS_H
#define GRAPHIC_UTILS_H

#include <lcom/lcf.h>
#include <stdint.h>

/**
 * @brief Changes the video mode to the specified mode.
 * 
 * @param mode The video mode to change to.
 * @return int Returns 0 on success, non-zero otherwise.
 */
int change_video_mode(uint16_t mode);

int create_frame_buffer(uint16_t mode);

// Getters

/**
 * @brief Gets the pointer to the video memory.
 * 
 * @return char* Returns a pointer to the video memory.
 */
uint8_t* get_video_mem();

/**
 * @brief Gets the horizontal resolution of the screen.
 * 
 * @return unsigned Returns the horizontal resolution.
 */
unsigned get_hres();

/**
 * @brief Gets the vertical resolution of the screen.
 * 
 * @return unsigned Returns the vertical resolution.
 */
unsigned get_vres();

/**
 * @brief Gets the number of bits per pixel.
 * 
 * @return unsigned Returns the number of bits per pixel.
 */
unsigned get_bits_per_pixel();

#endif
