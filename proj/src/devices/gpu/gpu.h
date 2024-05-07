#ifndef GPU_H
#define GPU_H

#include <lcom/lcf.h>
#include <stdint.h>

/**
 * @brief Changes the video mode to the specified mode.
 * 
 * @param mode video mode to change to.
 * @return 0 on success, non-zero otherwise.
 */
int change_video_mode(uint16_t mode);

/**
 * Creates a frame buffer for the specified VBE mode.
 *
 * @param mode VBE mode to set.
 * @return 0 on success, non-zero otherwise.
 */
int create_frame_buffer(uint16_t mode);

int limpa_buffer();


int draw_pixel(uint16_t x, uint16_t y, uint32_t color);

int draw_background(uint32_t color);


int draw_xpm(uint16_t x, uint16_t y, xpm_image_t img);


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
 * @brief Gets the number of bytes per pixel.
 * 
 * @return Number of bytes per pixel.
 */
unsigned get_bytes_per_pixel();


#endif
