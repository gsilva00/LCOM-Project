#include "graphics.h"

vbe_mode_info_t vmi_p;
static int *frame_buffer;

int set_graphics_mode(uint16_t mode){
  reg86_t r;
  memset(&r, 0, sizeof(reg86_t));
  r.ah = 0x4F;
  r.al = 0x02; 
  r.bx = BIT(14)| mode;
  r.intno = 0x10;
  if( sys_int86(&r) != OK ) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }
  return 0;
}

int set_frame_buffer(uint16_t mode){
  int r;
  memset(&vmi_p, 0, sizeof(vbe_mode_info_t));
  if(vbe_get_mode_info(mode, &vmi_p) != 0){
    return 1;
  }
  unsigned int frame_size = vmi_p.XResolution * vmi_p.YResolution * ((vmi_p.BitsPerPixel + 7)/8);
  struct minix_mem_range mr;
  mr.mr_base = vmi_p.PhysBasePtr;
  mr.mr_limit = mr.mr_base + frame_size;
  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }
  frame_buffer = vm_map_phys(SELF, (void *)mr.mr_base, frame_size);
  if(frame_buffer == MAP_FAILED){
    panic("couldn't map video memory");
    return 1;
  }
  return 0;
}

int paint_pixel(uint16_t x, uint16_t y, uint32_t color){
  if(x > vmi_p.XResolution || vmi_p.YResolution > y){
    return 0;
  }
  uint8_t BytesPerPixel = (vmi_p.BitsPerPixel + 7)/8;
  uint8_t index = (x + y * vmi_p.XResolution) * BytesPerPixel;
  if(memcpy(&frame_buffer[index], &color, BytesPerPixel) == NULL){
    return 1;
  }
  return 0;
}

int paint_line(uint16_t x, uint16_t y, uint16_t length, uint32_t color){
  for(int i = 0; i < length; i++){
    if(paint_pixel(x + i, y, color) != 0){
      return 1;
    }
  }
  return 0;
}

int paint_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(int i = 0; i < height; i++){
    if(y + i > vmi_p.YResolution){
      return 0;
    }
    if(paint_line(x, y + i, width, color) != 0){
      return 1;
    }
  }
  return 0;
}
