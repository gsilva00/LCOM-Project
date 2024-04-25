#include <lcom/lcf.h>

#include <stdint.h>

#include "video_macros.h"
#include "graphic_utils.h"


static uint8_t *video_mem;		        // Process (virtual) address to which VRAM is mapped
static vbe_mode_info_t vbe_minfo;
/*
Replaced:
static unsigned hres;             // Horizontal resolution in pixels
static unsigned vres;	            // Vertical resolution in pixels
static unsigned bits_per_pixel;   // Number of VRAM bits per pixel
They are present in the struct's fields
*/


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
  // Set video mode to graphics mode
  r86.ax = INVOKE_VBE_FUNC << 8 | SET_VBE_MODE;
  // Set the specified graphics mode
  r86.bx = SET_LINEAR_FB | mode; 

  // Kernel call switches from 32-bit protected mode to 16-bit real mode - access BIOS calls - and executes specified software interrupt instruction INT
  if (sys_int86(&r86)) return 1;

  return 0;
}


int create_frame_buffer(uint16_t mode) {
  // Get VBE mode info
  memset(&vbe_minfo, 0, sizeof(vbe_minfo));
  if (vbe_get_mode_info(mode, &vbe_minfo)) return 1;
  
  uint8_t bytesPerPix = (vbe_minfo.BitsPerPixel / 8) + ((vbe_minfo.BitsPerPixel % 8) ? 1 : 0);
  unsigned int vram_size = vbe_minfo.XResolution * vbe_minfo.YResolution * bytesPerPix;
  
  struct minix_mem_range mr;
  mr.mr_base = vbe_minfo.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  /* Allow memory mapping */
  int r;
  if((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 1;
  }

  return 0;
}



int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  // Draw the rectangle line by line
  for (uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color)) return 1;
  }

  return 0;
}
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  if (x >= vbe_minfo.XResolution || y >= vbe_minfo.YResolution) return 1;

  uint8_t bytesPerPix = (vbe_minfo.BitsPerPixel / 8) + ((vbe_minfo.BitsPerPixel % 8) ? 1 : 0);
  // Pointer to the starting pixel
  unsigned int initPos = (y * vbe_minfo.XResolution + x) * bytesPerPix;

  for (uint16_t i = 0; i < len; i++) {
    memcpy(&video_mem[initPos + i*bytesPerPix], &color, bytesPerPix);
  }

  return 0;
}


// Getters
uint8_t* get_video_mem() {
  return video_mem;
}
vbe_mode_info_t get_vbe_mode_info() {
  return vbe_minfo;
}
unsigned get_hres() {
  return vbe_minfo.XResolution;
}
unsigned get_vres() {
  return vbe_minfo.YResolution;
}
unsigned get_bits_per_pixel() {
  return vbe_minfo.BitsPerPixel;
}
