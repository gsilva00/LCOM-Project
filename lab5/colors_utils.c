#include <lcom/lcf.h>

#include <stdint.h>

#include "colors_utils.h"


uint8_t isolateRed(uint32_t first, vbe_mode_info_t currInfo) {
  // Red channel mask of size according to color model
  // if RedMaskSize == 5, BIT(RMS) == 100000, BIT(RMS)-1 == 11111
  uint8_t redMask = BIT(currInfo.RedMaskSize) - 1;
  
  // Right shifting by color's LSbit position, it becomes the LSbits
  // AND mask to ignore the rest
  return (first >> currInfo.RedFieldPosition) & redMask;
}

uint8_t isolateGreen(uint32_t first, vbe_mode_info_t currInfo) {
  uint8_t greenMask = BIT(currInfo.GreenMaskSize) - 1;
  return (first >> currInfo.GreenFieldPosition) & greenMask;
}

uint8_t isolateBlue(uint32_t first, vbe_mode_info_t currInfo) {
  uint8_t blueMask = BIT(currInfo.BlueMaskSize) - 1;
  return (first >> currInfo.BlueFieldPosition) & blueMask;
}
