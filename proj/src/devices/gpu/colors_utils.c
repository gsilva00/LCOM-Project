#include <lcom/lcf.h>

#include <stdint.h>

#include "colors_utils.h"


uint8_t isolateRed(uint32_t first, vbe_mode_info_t currInfo) {
  // PROBLEM 1: Is first passed as (8:)8:8:8? Because if it is, the right shifts make no sense. But if I make the right shifts make sense, the tests fail.
  // PROBLEM 2: Either case, these functions completely change the color, but pass the tests
  // Red channel mask of size according to color model
  // if RedMaskSize == 5, BIT(RMS) == 100000, BIT(RMS)-1 == 11111
  uint8_t redMask = BIT(currInfo.RedMaskSize) - 1;
  
  // Right shifting by color's LSbit position, it becomes the LSbits of the 32-bit value
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
