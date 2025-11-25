#include "math.h"
#include "util.c"

#define COLOR_CHANNELS 4

void draw_pixel(u8 *fb, int x, int y, int canvas_width) {
  int index = (y * canvas_width + x) * COLOR_CHANNELS;

  fb[index + 0] = 255; // R
  fb[index + 1] = 0;   // G
  fb[index + 2] = 0;   // B
  fb[index + 3] = 255; // A
}

void render(u8 *frame_buffer, int frame_buffer_length, int canvas_width,
            int canvas_height) {

  for (int i = 0; i < frame_buffer_length / 4; i++) {
    int x = i % canvas_width;
    int y = i / canvas_width;
    draw_pixel(frame_buffer, x, y, canvas_width);
  }
}
