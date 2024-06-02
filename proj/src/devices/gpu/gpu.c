#include "gpu.h"
#include "gpu_macros.h"


static uint8_t *video_mem; // Process (virtual) address to which VRAM is mapped
static uint8_t *double_buffer; // For double-buffering -> Prepare next frame
static uint8_t *game_background; // 3rd buffer -> stores the constant/not-moving parts of the game

static vbe_mode_info_t vmi;
static uint8_t bytes_per_pixel;

int change_video_mode(uint16_t mode) {
  reg86_t r86;
  // Avoid unexpected behavior by libx86emu when making INT 0x10 call
  memset(&r86, 0, sizeof(r86));

  r86.intno = INT_10;

  // Set video mode to graphics mode
  r86.ax = INVOKE_VBE_FUNC << 8 | SET_VBE_MODE;

  // Set the specified graphics mode
  r86.bx = SET_LINEAR_FB | mode;

  // Kernel call switches from 32-bit protected mode to 16-bit real mode - access BIOS calls - and executes specified software interrupt instruction INT
  if (sys_int86(&r86) == EFAULT) {
    printf("Error: sys_int86 kernel call!\n");
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

  double_buffer = (uint8_t *) malloc(vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8));

  game_background = (uint8_t *) malloc(vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8));

  return 0;
}


void prep_next_frame() {
  memcpy(double_buffer, game_background, vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8));
}
void end_next_frame() {
  memcpy(video_mem, double_buffer, vmi.XResolution * vmi.YResolution * ((vmi.BitsPerPixel + 7) / 8));
}


int free_buffers() {
  free(video_mem);
  free(double_buffer);
  free(game_background);
  return 0;
}


int draw_pixel(uint16_t x, uint16_t y, uint32_t color, bool ignore) {
  if (y >= vmi.YResolution) return 1;

  if (x >= vmi.XResolution) {
    if (!ignore) x = x - vmi.XResolution;
    else return 1;
  }

  uint8_t *pixel_pos = double_buffer + ((y * vmi.XResolution + x) * bytes_per_pixel);

  memcpy(pixel_pos, &color, bytes_per_pixel);

  return 0;
}


int init_background(uint16_t *x_coords, uint16_t *y_coords, xpm_image_t *imgs, size_t num_elements) {
  for (size_t i = 0; i < num_elements; i++) {
    if (draw_to_background(x_coords[i], y_coords[i], imgs[i])) {
      printf("Error while drawing element %zu!\n", i);
      return 1;
    }
  }

  return 0;
}

int draw_to_background(uint16_t xi, uint16_t yi, xpm_image_t img) {
  unsigned int pos;
  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      pos = (y * img.width + x) * bytes_per_pixel;
      uint32_t color = (img.bytes[pos + 2]) << 16 | (img.bytes[pos + 1] << 8) | img.bytes[pos];

      if (color != 0x00FF00) {
        uint8_t *pixel_pos = game_background + (((y+yi) * vmi.XResolution + (x+xi)) * bytes_per_pixel);
        memcpy(pixel_pos, &color, bytes_per_pixel);
      }
    }
  }
  return 0;
}

int draw_nums_to_background(uint16_t xi, uint16_t yi, uint8_t points1) {
  unsigned int pos;
  xpm_image_t img;

  char **numbers_xpm[] = {number_0_xpm, number_1_xpm, number_2_xpm, number_3_xpm, number_4_xpm, number_5_xpm, number_6_xpm, number_7_xpm, number_8_xpm, number_9_xpm};

  if (points1 >= 0 && points1 <= 9) {
    xpm_load((xpm_map_t) numbers_xpm[points1], XPM_8_8_8, &img);
  }
  

  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      pos = (y * img.width + x) * bytes_per_pixel;
      uint32_t color = (img.bytes[pos + 2]) << 16 | (img.bytes[pos + 1] << 8) | img.bytes[pos];

      if (color != 0x00FF00) {
        uint8_t *pixel_pos = game_background + (((y + yi) * vmi.XResolution + (x + xi)) * bytes_per_pixel);
        memcpy(pixel_pos, &color, bytes_per_pixel);
      }
    }
  }
  return 0;
}


int draw_xpm(uint16_t xi, uint16_t yi, xpm_image_t img, bool ignore) {
  unsigned int pos;

  for (int y = 0; y < img.height; y++) {
    for (int x = 0; x < img.width; x++) {
      pos = (y * img.width + x) * bytes_per_pixel;;
      uint32_t color = (img.bytes[pos + 2] << 16) | (img.bytes[pos + 1] << 8) | img.bytes[pos];

      if (color != 0x00FF00) {
        if (draw_pixel(xi + x, yi + y, color, ignore)) {
          continue;
        }
      }
    }
  }
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
