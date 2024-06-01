#include <lcom/lcf.h>

#include <stdint.h>

#include "gpu.h"
#include "gpu_macros.h"

static uint8_t *video_mem; // Process (virtual) address to which VRAM is mapped
static uint8_t *double_buffer;

static uint8_t *triple_buffer; // para nao estar sempre a copiar o background, ele vai guardar o background e depois so copia para o double buffer

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
  if (sys_int86(&r86) == EFAULT) {
    printf("sys_int86 kernel call failed!\n");
    return 1;
  }

  return 0;
}

int create_frame_buffer(uint16_t mode) {
  // Get VBE mode info
  memset(&vmi, 0, sizeof(vmi));
  if (vbe_get_mode_info(mode, &vmi)) {
    printf("Error while getting vmi!\n");
    return 1;
  }

  bytes_per_pixel = (vmi.BitsPerPixel / 8) + ((vmi.BitsPerPixel % 8) ? 1 : 0);
  unsigned int vram_size = vmi.XResolution * vmi.YResolution * bytes_per_pixel;

  struct minix_mem_range mr;
  mr.mr_base = vmi.PhysBasePtr;
  mr.mr_limit = mr.mr_base + vram_size;

  /* Allow memory mapping */
  int r;
  if ((r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return 1;
  }

  double_buffer = (uint8_t *) malloc(vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8)); // para alocar memoria para o double buffer

  triple_buffer = (uint8_t *) malloc(vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8)); // para alocar memoria para o triple buffer

  return 0;
}

int limpa_buffer(){
  free(double_buffer);
  free(triple_buffer);
  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  // Draw the rectangle - line by line
  for (uint16_t i = 0; i < height; i++) {
    if (vg_draw_hline(x, y + i, width, color)) {
      printf("Error while drawing horizontal line!\n");
      return 1;
    }
  }

  return 0;
}
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  // Draw the line - pixel by pixel
  for (uint16_t i = 0; i < len; i++) {
    if (draw_pixel(x + i, y, color, true)) {
      printf("Error while drawing pixel!\n");
      return 1;
    }
  }

  return 0;
}

int draw_pixel(uint16_t x, uint16_t y, uint32_t color, bool ignore) {
  if (y >= vmi.YResolution) {
    // printf("Pixel coordinates out of screen bounds!\n");
    return 1;
  }

  if (x >= vmi.XResolution) {
    if (!ignore)
    {
      x = x - vmi.XResolution;
    }else{
      return 1;
    }
  }

  // Pixel's position in the buffer
  uint8_t *pixel_pos = double_buffer + ((y * vmi.XResolution + x) * bytes_per_pixel);

  // Checking for error not necessary -> causes segmentation fault (breaks the program)
  memcpy(pixel_pos, &color, bytes_per_pixel);

  return 0;
}

int draw_back(uint16_t xi, uint16_t yi, xpm_image_t img) {
  unsigned int pos;
  // printf("Reached draw_xpm!\n");
  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      pos = (y * img.width + x) * bytes_per_pixel;
      // printf("x:%d, y:%d - ", x, y);
      // printf("blue:%x; green:%x; red:%x\n", *(img.bytes + pos), *(img.bytes + pos+1), *(img.bytes + pos+2));
      uint32_t color = (img.bytes[pos + 2]) << 16 | (img.bytes[pos + 1] << 8) | img.bytes[pos];

      if (color != 0x00FF00) {
        uint8_t *pixel_pos = triple_buffer + (((y+yi) * vmi.XResolution + (x+xi)) * bytes_per_pixel);
        memcpy(pixel_pos, &color, bytes_per_pixel);
      }
    }
  }
  return 0;
}

int draw_back_scoreboard(uint16_t xi, uint16_t yi, uint8_t points1) {
  unsigned int pos;
  xpm_image_t img;
  if(points1 == 0){
    xpm_map_t pic = (xpm_map_t) number_0_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 1){
    xpm_map_t pic = (xpm_map_t) number_1_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 2){
    xpm_map_t pic = (xpm_map_t) number_2_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 3){
    xpm_map_t pic = (xpm_map_t) number_3_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 4){
    xpm_map_t pic = (xpm_map_t) number_4_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 5){
    xpm_map_t pic = (xpm_map_t) number_5_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 6){
    xpm_map_t pic = (xpm_map_t) number_6_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 7){
    xpm_map_t pic = (xpm_map_t) number_7_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 8){
    xpm_map_t pic = (xpm_map_t) number_8_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }else if(points1 == 9){
    xpm_map_t pic = (xpm_map_t) number_9_xpm;
    xpm_load(pic, XPM_8_8_8,&img);
  }
  

  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      pos = (y * img.width + x) * bytes_per_pixel;
      uint32_t color = (img.bytes[pos + 2]) << 16 | (img.bytes[pos + 1] << 8) | img.bytes[pos];

      if (color != 0x00FF00) {
        uint8_t *pixel_pos = triple_buffer + (((y + yi) * vmi.XResolution + (x + xi)) * bytes_per_pixel);
        memcpy(pixel_pos, &color, bytes_per_pixel);
      }
    }
  }
  return 0;
}


int draw_background(uint32_t color) {
  for (uint16_t y = 0; y < vmi.YResolution; y++) {
    for (uint16_t x = 0; x < vmi.XResolution; x++) {

      if (y >= vmi.YResolution) {
        // printf("Pixel coordinates out of screen bounds!\n");
        return 1;
      }

      if (x >= vmi.XResolution) {
        x = x - vmi.XResolution;
      }

      // Pixel's position in the buffer
      uint8_t *pixel_pos = triple_buffer + ((y * vmi.XResolution + x) * bytes_per_pixel);

      // Checking for error not necessary -> causes segmentation fault (breaks the program)
      memcpy(pixel_pos, &color, bytes_per_pixel);
    }
  }

  return 0;
}

int draw_frame_start(){
  memcpy(double_buffer, triple_buffer, vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8));
  return 0;
}

int draw_xpm(uint16_t xi, uint16_t yi, xpm_image_t img, bool ignore) {
  unsigned int pos;
  // printf("Reached draw_xpm!\n");
  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      pos = (y * img.width + x) * bytes_per_pixel;
      // printf("x:%d, y:%d - ", x, y);
      // printf("blue:%x; green:%x; red:%x\n", *(img.bytes + pos), *(img.bytes + pos+1), *(img.bytes + pos+2));
      uint32_t color = (img.bytes[pos + 2] << 16) | (img.bytes[pos + 1] << 8) | img.bytes[pos];

      if (color != 0x00FF00) {
        if (draw_pixel(xi + x, yi + y, color, ignore)) {
          // printf("Error while drawing pixel!\n");
          continue;
        }
      }
    }
  }
  return 0;
}

int draw_frame_end(){
    memcpy(video_mem, double_buffer, vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8));
  return 0;
}

// Getters
uint8_t *get_video_mem() {
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
