#include <stdint.h>

#define BIT(n) 1 << n

typedef struct {
uint16_t x_res; // horizontal resolution
uint16_t y_res; // vertical resolution
uint8_t bpp; // bits per pixel
uint8_t r_sz; // red component size
uint8_t r_pos; // red component LSB position
uint8_t g_sz; // green component size
uint8_t g_pos; // green component LSB position
uint8_t b_sz; // blue component size
uint8_t b_pos; // blue component LSB position
phys_addr_t phys_addr; // video ram base phys address
} lpv_mode_info_t;

enum lpv_dir_t {
  lpv_hor, // horizontal line
  lpv_vert // vertical line
};

lpv_mode_info_t lmi_p;
uint8_t *frame_buffer;

int pp_test_line(uint8_t mode, enum lpv_dir_t dir, uint16_t x,
uint16_t y, uint16_t len, uint32_t color, uint32_t delay){
  if(mode == 0) return 0;
  if(mode < 0 || mode > 4) return 1;
  if(lpv_get_mode_info(mode, &lmi_p) != 0){
    return 1;
  }
  frame_buffer = video_map_phys(lmi_p.phys_addr, lmi_p.x_res*lmi_p.y_res*((lmi_p.bpp + 7)/8));
  if(frame_buffer == NULL){
    return 1;
  }
  if(lpv_set_mode(mode) != 0){
    return 1;
  }
  if(mode == 1){
    color &= (BIT(8) - 1);
  } else{
    uint32_t blue = color & (BIT(lmi_p.b_sz) - 1);
    color >> 8;
    uint32_t green = color & (BIT(lmi_p.g_sz) - 1);
    color >> 8;
    uint32_t red = color & (BIT(lmi_p.r_sz) - 1);
    color >> 8;
    color = (red << lmi_p.r_pos) | (green << lmi_p.g_pos) | (blue << lmi_p.b_pos);
  }
  if(dir == lpv_hor){
    for(int i = 0; i < len; i++){
      if(x + i > lmi_p.x_res || y > lmi_p.y_res){}
      else{
        uint8_t index = (y*lmi_p.x_res + (x+i))*((lmi_p.bpp + 7)/8);
        if(memcpy(&frame_buffer[index]), &color, (lmi_p.bpp + 7)/8 == NULL){
          return 1;
        }
      }
    }
  }else if (dir == lpv_vert){
    for(int i = 0; i < len; i++){
      if(y + i > lmi_p.y_res || x > lmi_p.x_res){}
      else{
        uint8_t index = (y*lmi_p.x_res + (x+i))*((lmi_p.bpp + 7)/8);
        if(memcpy(&frame_buffer[index]), &color, (lmi_p.bpp + 7)/8 == NULL){
          return 1;
        }
      }
    }
  }else{
    return 1;
  }
  sleep(delay);
  if(lpv_set_mode(0) != 0){
    return 1;
  }
}