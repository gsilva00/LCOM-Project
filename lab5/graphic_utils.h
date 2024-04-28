#ifndef GRAPHIC_UTILS_H
#define GRAPHIC_UTILS_H

#include <lcom/lcf.h>
#include <stdint.h>

/**
 * @brief Changes the video mode to the specified mode.
 * 
 * @param mode The video mode to change to.
 * @return 0 on success, non-zero otherwise.
 */
int change_video_mode(uint16_t mode);

/**
 * Creates a frame buffer for the specified VBE mode.
 *
 * @param mode The VBE mode to set.
 * @return 0 if successful, 1 otherwise.
 */
int create_frame_buffer(uint16_t mode);


int draw_pixel(uint16_t x, uint16_t y, uint32_t color);
  

// Getters

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
 * @brief Gets the number of bits per pixel.
 * 
 * @return Number of bits per pixel.
 */
unsigned get_bits_per_pixel();

#endif
