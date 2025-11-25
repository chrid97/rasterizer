#include "math.h"
#include "util.c"

#define COLOR_CHANNELS 4

const Vector3 BLACK = {0, 0, 0};
const Vector3 RED = {255, 0, 0};
const Vector3 GREEN = {0, 255, 0};
const Vector3 BLUE = {0, 0, 255};
const Vector3 WHITE = {255, 255, 255};

void draw_pixel(u8 *fb, int x, int y, int canvas_width, Vector3 color) {
  int index = (y * canvas_width + x) * COLOR_CHANNELS;

  fb[index] = (int)color.x;     // R
  fb[index + 1] = (int)color.y; // G
  fb[index + 2] = (int)color.z; // B
  fb[index + 3] = 255;          // A
}

void render(u8 *frame_buffer, int frame_buffer_length, int canvas_width,
            int canvas_height) {

  for (int i = 0; i < frame_buffer_length / 4; i++) {
    int x = i % canvas_width;
    int y = i / canvas_width;
    draw_pixel(frame_buffer, x, y, canvas_width, RED);
  }

  Vector2 p0 = {0, 0};
  Vector2 p1 = {canvas_width - 1, 0};
  // for (int x = p0.x; x <= p1.x; x += 4) {
  //   // int index = x;
  //   // frame_buffer[index + 0] = 255; // R
  //   // frame_buffer[index + 1] = 0;   // G
  //   // frame_buffer[index + 2] = 0;   // B
  //   // frame_buffer[index + 3] = 255; // A
  //   draw_pixel(frame_buffer, x, 0, canvas_width, RED);
  // }
  draw_pixel(frame_buffer, 0, 0, canvas_width, RED);
  int index = 12;
  frame_buffer[index + 0] = (u8)RED.x; // R
  frame_buffer[index + 1] = (u8)RED.y; // G
  frame_buffer[index + 2] = (u8)RED.z; // B
  frame_buffer[index + 3] = (u8)255;   // A
}
