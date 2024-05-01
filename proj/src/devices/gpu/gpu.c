#include <lcom/lcf.h>

#include <stdint.h>

#include "gpu_macros.h"
#include "gpu.h"


static uint8_t *video_mem;		    // Process (virtual) address to which VRAM is mapped
static vbe_mode_info_t vmi;
/*
Above replaced:
static unsigned hres;             // Horizontal resolution in pixels
static unsigned vres;	            // Vertical resolution in pixels
static unsigned bits_per_pixel;   // Number of VRAM bits per pixel
They are present in the struct's fields
*/
static uint8_t bytes_per_pixel;


int change_video_mode(uint16_t mode) {
  reg86_t r86;
  // Avoid unexpected behavior by libx86emu when making INT 0x10 call
  memset(&r86, 0, sizeof(r86));

  r86.intno = INT_10;
  
  // Set video mode to graphics mode
  r86.ax = INVOKE_VBE_FUNC << 8 | SET_VBE_MODE;
  /*
  Above replaced:
  r86.ah = INVOKE_VBE_FUNC;
  r86.al = SET_VBE_MODE;
  */

  // Set the specified graphics mode
  r86.bx = SET_LINEAR_FB | mode; 

  // Kernel call switches from 32-bit protected mode to 16-bit real mode - access BIOS calls - and executes specified software interrupt instruction INT
  if (sys_int86(&r86) == EFAULT) {printf("sys_int86 kernel call failed!\n"); return 1;}

  return 0;
}


int create_frame_buffer(uint16_t mode) {
  // Get VBE mode info
  memset(&vmi, 0, sizeof(vmi));
  if (vbe_get_mode_info(mode, &vmi)) {printf("Error while getting vmi!\n"); return 1;}
  
  bytes_per_pixel = (vmi.BitsPerPixel / 8) + ((vmi.BitsPerPixel % 8) ? 1 : 0);
  unsigned int vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;
  
  struct minix_mem_range mr;
  mr.mr_base = vmi.PhysBasePtr;
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
  // Draw the rectangle - line by line
  for (uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color)) {
      printf("Error while drawing horizontal line!\n");
      return 1;
    }
  }

  return 0;
}
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  // Draw the line - pixel by pixel
  for (uint16_t i = 0; i < len; i++) {
    if (draw_pixel(x+i, y, color)) {
      printf("Error while drawing pixel!\n");
      return 1;
    }
  }

  return 0;
}

int draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x >= vmi.XResolution || y >= vmi.YResolution) {
    printf("Pixel coordinates out of screen bounds!\n");
    return 1;
  }

  // Pixel's position in the buffer
  unsigned int pos = (y * vmi.XResolution + x) * bytes_per_pixel;

  // Checking for error not necessary -> causes segmentation fault (breaks the program)
  memcpy(&video_mem[pos], &color, bytes_per_pixel);

  return 0;
}

int draw_xpm(uint16_t xi, uint16_t yi, xpm_image_t img) {
  unsigned int pos;
  
  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      pos = (y * img.width + x) * bytes_per_pixel;
      if (draw_pixel(xi + x, yi + y, *(img.bytes + pos))) {
        printf("Error while drawing pixel!\n");
        return 1;
      }
    }
  }

  return 0;
}

// Getters
uint8_t* get_video_mem() {
  return video_mem;
}
vbe_mode_info_t get_vbe_mode_info() {
  return vmi;
}
unsigned get_hres() {
  return vmi.XResolution;
}
unsigned get_vres() {
  return vmi.YResolution;
}
unsigned get_bytes_per_pixel() {
  return bytes_per_pixel;
}
