// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include <lcom/timer.h>
#include "aux_timer.h"
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

  // printf("\nType: %d\n", img.type);
  // printf("Width: %d\n", img.width);
  // printf("Height: %d\n", img.height);
  // printf("Size: %d\n", img.size);
  // printf("Bytes: %p\n\n", img.bytes);

  if (draw_xpm(x, y, img)) return 1;


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

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  // Initialize vram and mode
  // 0x105 mode is asked to be hard-coded
  if (create_frame_buffer(VBE_MODE_105)) return 1;
  if (change_video_mode(VBE_MODE_105)) return 1;


  // Prepare XPM
  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img; // pixmap and metadata  
  uint8_t *map = xpm_load(xpm, type, &img);
  if (map == NULL) return 1;

  // Draw initial position
  if (draw_xpm(xi, yi, img)) return 1;


  // Direction of movement:
  // 1: positive direction of axis; 
  // 0: no movement in that axis;
  // -1: negative direction of axis;
  int8_t hor = 0;
  int8_t vert = 0;
  if (yi == yf && xi < xf) hor = 1;
  else if (yi == yf && xi > xf) hor = -1;
  else if (xi == xf && yi < yf) vert = 1;
  else if (xi == xf && yi > yf) vert = -1;

  // Interrupt loop
  int ipc_status, r;
  message msg;

  uint8_t bit_no;
  if (timer_subscribe_int(&bit_no)) return 1;
  uint32_t timer_int_bit = BIT(bit_no);
  if (kbd_subscribe_int(&bit_no)) return 1;
  uint32_t kbc_int_bit = BIT(bit_no);

  // Interrupts per frame 
  // (way of controlling framerate without without changing interrupt rate)
  // fr_rate is always a divisor of 60 (sys_hz())
  // int_per_frame is at most 60
  uint8_t int_per_frame = sys_hz() / fr_rate;
  // Num of frames per movement when speed < 0
  // (When its one, it moves 1 every frame)
  uint16_t numFrames = 1;

  // Is there movement needed? (edge case)
  bool noMove = false;
  if ((yi == yf && xi == xf) || speed == 0) noMove = true;

  int i = 0;
  uint8_t scancode = 0;
  while (scancode != BREAKCODE_ESC && !noMove) {
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
        if (msg.m_notify.interrupts & timer_int_bit) { // subscribed timer interrupt
            timer_int_handler();

            // Final position reached but ESC not pressed 
            // (avoid doing useless computation with the buffer)
            if (xi == xf && yi == yf) {
              if (get_timer_intCounter() % 60 == 0) { // To avoid spam-printing
                printf("\n %ds passed: The sprite has finished moving, you can press ESC to leave!\n", i++);
              }
              continue;
            }

            if (get_timer_intCounter() % int_per_frame == 0) { // Do something every frame
              if (speed > 0) { // Displacement in pixels between consecutive frames
                // Clear screen for next frame
                // Very naive/brute-forcey implementation
                // Can be massively optimized
                if (vg_draw_rectangle(xi, yi, img.width, img.height, 0xFFFFFF)) return 1;

                // Near the end position, the move can be less than speed
                if (vert == 1) yi = (yi + speed < yf) ? yi + speed : yf;
                else if (vert == -1) yi = (yi - speed > yf) ? yi - speed : yf;
                else if (hor == 1) xi = (xi + speed < xf) ? xi + speed : xf;
                else if (hor == -1) xi = (xi - speed > xf) ? xi - speed : xf;
                if (draw_xpm(xi, yi, img)) return 1;
              }
              else { // Number of frames required for moving 1 pixel
                if (numFrames == abs(speed)) { // Only does something when speed frames have passed
                  // Clear screen for next frame (only when new frame is drawn)
                  if (vg_draw_rectangle(xi, yi, img.width, img.height, 0xFFFFFF)) return 1;

                  if (vert == 1) yi = (yi+1 < yf) ? yi+1 : yf;
                  else if (vert == -1) yi = (yi-1 > yf) ? yi-1 : yf;
                  else if (hor == 1) xi = (xi+1 < xf) ? xi+1 : xf;
                  else if (hor == -1) xi = (xi-1 > xf) ? xi-1 : xf;
                  if (draw_xpm(xi, yi, img)) return 1;

                  numFrames = 1; // Reset until next movement
                } 
                else numFrames++; // Haven't reached enough frames to move 1 pixel yet
              }
            }
          }
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

  if (timer_unsubscribe_int()) return 1;
  if (kbd_unsubscribe_int()) return 1;
  
  if (vg_exit()) return 1;

  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
