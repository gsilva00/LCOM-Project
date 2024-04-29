#ifndef COLORS_UTILS_H
#define COLORS_UTILS_H

#include <lcom/lcf.h>


uint8_t isolateRed(uint32_t first, vbe_mode_info_t currInfo);
uint8_t isolateGreen(uint32_t first, vbe_mode_info_t currInfo);
uint8_t isolateBlue(uint32_t first, vbe_mode_info_t currInfo);

#endif /* COLORS_UTILS_H */
