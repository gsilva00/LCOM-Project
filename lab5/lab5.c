// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "video_macros.h"
#include "graphic_utils.h"
#include "keyboard.h"
#include "kbc.h"
#include "colors_utils.h"
#include "i8042.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(change_video_mode(mode)) return 1;

  sleep(delay);

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  // Step 1: Map video memory (VRAM) to process's address space                         
  if (create_frame_buffer(mode)) return 1;

  // Step 2: Change video mode to the one specified in the mode argument
  if (change_video_mode(mode)) return 1;

  // Step 3: Draw the rectangle
  if (vg_draw_rectangle(x, y, width, height, color)) return 1;

  // Step 4: Wait for ESC key release and reset Minix to text mode
  int ipc_status, r;
  message msg;

  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no)) return 1;
  uint32_t kbc_int_bit = BIT(bit_no);

  uint8_t scancode = get_scancode();
  while (scancode != BREAKCODE_ESC) {
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & kbc_int_bit) { // subscribed keyboard interrupt
            kbc_ih();

            scancode = get_scancode();
          }
          break;
        default:
          // no other notifications expected: do nothing
          break;
      }
    } 
    else { // received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
  if (kbd_unsubscribe_int()) return 1;
  
  if (vg_exit()) return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (create_frame_buffer(mode)) return 1;
  if (change_video_mode(mode)) return 1;

  unsigned int xRecSize = get_hres() / no_rectangles;
  unsigned int yRecSize = get_vres() / no_rectangles;
  for (uint8_t row = 0; row < no_rectangles; row++) {
    for (uint8_t col = 0; col < no_rectangles; col++) {
      vbe_mode_info_t currInfo = get_vbe_mode_info();
      uint32_t color = 0;

      uint8_t firstRed = isolateRed(first, currInfo);
      uint8_t firstGreen = isolateGreen(first, currInfo);
      uint8_t firstBlue = isolateBlue(first, currInfo);

      if (currInfo.MemoryModel == INDEXED) {
        // Go get color from indexed mode color table with the following index
        // uint8_t index = (first + (row * no_rectangles + col) * step) % (1 << currInfo.BitsPerPixel);
        color = 0x008C2D19; // placeholder
      }
      else if (currInfo.MemoryModel == DIRECT_COLOR) {
        uint8_t red = (firstRed + col * step) % (1 << currInfo.RedMaskSize);
        uint8_t green = (firstGreen + row * step) % (1 << currInfo.GreenMaskSize);
        uint8_t blue = 	(firstBlue + (col + row) * step) % (1 << currInfo.BlueMaskSize);	

        color = 0x00000000 | (red << currInfo.RedFieldPosition) | (green << currInfo.GreenFieldPosition) | blue << currInfo.BlueFieldPosition;
      }

      if (vg_draw_rectangle(col*xRecSize, row*yRecSize, xRecSize, yRecSize, color)) return 1;
    }
  }

  int ipc_status, r;
  message msg;

  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no)) return 1;
  uint32_t kbc_int_bit = BIT(bit_no);

  uint8_t scancode = get_scancode();
  while (scancode != BREAKCODE_ESC) {
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & kbc_int_bit) { // subscribed keyboard interrupt
            kbc_ih();

            scancode = get_scancode();
          }
          break;
        default:
          // no other notifications expected: do nothing
          break;
      }
    } 
    else { // received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
  if (kbd_unsubscribe_int()) return 1;
  
  if (vg_exit()) return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  // 0x105 mode is asked to be hard-coded
  if (create_frame_buffer(VBE_MODE_105)) return 1;
  if (change_video_mode(VBE_MODE_105)) return 1;

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img; // pixmap and metadata  

  // Pixmap itself - 2D - array of colors (indexed in this case)
  // Points to the first element in the array
  uint8_t *map = xpm_load(xpm, type, &img);

  if (map == NULL) return 1;

  for (int y_disp = 0; y_disp < img.height; y_disp++) {
    for (int x_disp = 0; x_disp < img.width; x_disp++) {
      draw_pixel(x + x_disp, y + y_disp, *map);
      map++; // Moves the pointer by bytesPerPix amount (1 byte in XPM_INDEXED)
    }
  }

  int ipc_status, r;
  message msg;

  uint8_t bit_no;
  if (kbd_subscribe_int(&bit_no)) return 1;
  uint32_t kbc_int_bit = BIT(bit_no);

  uint8_t scancode = get_scancode();
  while (scancode != BREAKCODE_ESC) {
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & kbc_int_bit) { // subscribed keyboard interrupt
            kbc_ih();

            scancode = get_scancode();
          }
          break;
        default:
          // no other notifications expected: do nothing
          break;
      }
    } 
    else { // received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
  if (kbd_unsubscribe_int()) return 1;
  
  if (vg_exit()) return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
