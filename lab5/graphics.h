#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <stdint.h>
#include <machine/int86.h>

int set_graphics_mode(uint16_t mode);
int set_frame_buffer(uint16_t mode);
int paint_pixel(uint16_t x, uint16_t y, uint32_t color);
int paint_line(uint16_t x, uint16_t y, uint16_t length, uint32_t color);
int paint_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
