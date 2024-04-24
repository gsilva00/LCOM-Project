#include <lcom/lcf.h>

#include <stdint.h>
#include "video_macros.h"
#include "graphic_utils.h"

int change_video_mode(uint16_t mode) {
  reg86_t r86;
  // Avoid unexpected behavior by libx86emu when making INT 0x10 call
  memset(&r86, 0, sizeof(r86));

  r86.intno = INT_10;
  /*
  Breakdown of instruction below:
  r86.ah = INVOKE_VBE_FUNC;
  r86.al = SET_VBE_MODE;
  */
  r86.ax = INVOKE_VBE_FUNC << 8 | SET_VBE_MODE;
  r86.bx = SET_LINEAR_FB | mode; 

  // Kernel call switches from 32-bit protected mode to 16-bit real mode - access BIOS calls - and executes specified software interrupt instruction INT
  if (sys_int86(&r86)) return 1;

  return 0;
}
