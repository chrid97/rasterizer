#include "math.h"
#include "util.c"

#define COLOR_CHANNELS 4

extern unsigned char __heap_base;

const Vector3 BLACK = {0, 0, 0};
const Vector3 RED = {255, 0, 0};
const Vector3 GREEN = {0, 255, 0};
const Vector3 BLUE = {0, 0, 255};
const Vector3 WHITE = {255, 255, 255};

extern void js_log(int x);

__attribute__((used)) uint32_t get_heap_base() {
  return (uint32_t)&__heap_base;
}

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
    draw_pixel(frame_buffer, x, y, canvas_width, BLACK);
  }

  Vector2 p0 = {0, 10};
  Vector2 p1 = {canvas_width - 1, 0};
  for (int x = p0.x; x <= p1.x; x++) {
    draw_pixel(frame_buffer, x, p0.y, canvas_width, RED);
  }
}
