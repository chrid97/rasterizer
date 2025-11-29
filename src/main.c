#include "math.h"
#include "util.c"

#define COLOR_CHANNELS 4

extern unsigned char __heap_base;

const Vector3 BLACK = {0, 0, 0};
const Vector3 RED = {255, 0, 0};
const Vector3 GREEN = {0, 255, 0};
const Vector3 BLUE = {0, 0, 255};
const Vector3 WHITE = {255, 255, 255};

static int canvas_width;
static int canvas_height;

extern void js_log(int x);

__attribute__((used)) uint32_t get_heap_base() {
  return (uint32_t)&__heap_base;
}

void draw_pixel(u8 *fb, int x, int y, Vector3 color) {
  int index = (y * canvas_width + x) * COLOR_CHANNELS;

  fb[index] = (int)color.x;     // R
  fb[index + 1] = (int)color.y; // G
  fb[index + 2] = (int)color.z; // B
  fb[index + 3] = 255;          // A
}

// TODO Replace with Bresenham’s line algorithm
void draw_line(u8 *frame_buffer, int x0, int y0, int x1, int y1) {
  float dx = x1 - x0;
  float dy = y1 - y0;

  int steps = max(absf(dx), absf(dy));

  if (steps != 0) {
    float x_step = dx / steps;
    float y_step = dy / steps;

    float x = x0;
    float y = y0;
    for (int i = 0; i <= steps; i++) {
      draw_pixel(frame_buffer, (int)(x), (int)(y), RED);
      x += x_step;
      y += y_step;
    }
  }
}

void draw_triangle(u8 *fb, Vector2 p0, Vector2 p1, Vector2 p2) {
  draw_line(fb, p0.x, p0.y, p1.x, p1.y);
  draw_line(fb, p1.x, p1.y, p2.x, p2.y);
  draw_line(fb, p2.x, p2.y, p0.x, p0.y);
}

void render(u8 *frame_buffer, int frame_buffer_length, int browser_canvas_width,
            int browser_canvas_height) {

  canvas_width = browser_canvas_width;
  canvas_height = browser_canvas_height;
  js_log(canvas_width);

  for (int i = 0; i < frame_buffer_length / 4; i++) {
    int x = i % canvas_width;
    int y = i / canvas_width;
    draw_pixel(frame_buffer, x, y, BLACK);
  }

  Vector2 p0 = {100, 10};
  Vector2 p1 = {50, 200};
  Vector2 p2 = {150, 200};
  draw_triangle(frame_buffer, p0, p1, p2);
}
