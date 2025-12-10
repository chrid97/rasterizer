#include "cg_math.h"
#include "util.c"
#include <math.h>

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

  int steps = max(fabsf(dx), fabsf(dy));

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

void draw_triangle_fill(u8 *fb, Vector2 p0, Vector2 p1, Vector2 p2) {
  // for (int y = p2.y; y > p0.y; y--) {
  //   draw_line(fb, p1.x, p1.y - y, p2.x, p2.y - y);
  // }
  // draw_line(fb, p1.x, p1.y - 1, p2.x, p2.y - 1);
  // draw_line(fb, p1.x, p1.y - 2, p2.x, p2.y - 2);
  // draw_line(fb, p1.x, p1.y - 3, p2.x, p2.y - 3);
  // draw_line(fb, p1.x, p1.y, p2.x, p2.y);
  // draw_line(fb, p2.x, p2.y, p0.x, p0.y);
}

Matrix look_at(Vector4 camera, Vector4 target) {
  Vector4 world_up = (Vector4){0, 1, 0, 0};

  Vector4 f = vec4_normalize(vec4_subtract(target, camera));
  Vector4 r = vec4_normalize(vec4_cross(world_up, f));
  Vector4 u = vec4_cross(f, r);

  Matrix m = {0};
  m.d[0][0] = r.x;
  m.d[0][1] = r.y;
  m.d[0][2] = r.z;
  m.d[0][3] = -vec4_dot(r, camera);

  m.d[1][0] = u.x;
  m.d[1][1] = u.y;
  m.d[1][2] = u.z;
  m.d[1][3] = -vec4_dot(u, camera);

  m.d[2][0] = f.x;
  m.d[2][1] = f.y;
  m.d[2][2] = f.z;
  m.d[2][3] = -vec4_dot(f, camera);

  m.d[3][0] = 0.0f;
  m.d[3][1] = 0.0f;
  m.d[3][2] = 0.0f;
  m.d[3][3] = 1.0f;

  return m;
}

void render(int frame_buffer_length, int browser_canvas_width,
            int browser_canvas_height) {
  u8 *frame_buffer = (u8 *)get_heap_base();
  canvas_width = browser_canvas_width;
  canvas_height = browser_canvas_height;
  const float viewport_width = 2.0f;
  const float viewport_height = 2.0f;

  for (int i = 0; i < frame_buffer_length / 4; i++) {
    int x = i % canvas_width;
    int y = i / canvas_width;
    draw_pixel(frame_buffer, x, y, BLACK);
  }

  Vector4 camera = {0, 0, 1, 1};
  Vector4 target = {0, 0, 0, 1};

  Matrix view = look_at(camera, target);
  Vector4 p0 = {0.7, 0.0, 0.2, 1};
  Vector4 point = vec_mult_matrix(p0, view);

  Vector2 screen = {
      point.x / -point.z,
      point.y / -point.z,
  };

  if (fabsf(screen.x) > viewport_width * 0.5f ||
      fabsf(screen.y) > viewport_height * 0.5f) {
    return;
  }

  Vector2 ndc = {
      (screen.x + viewport_width * 0.5f) / viewport_width,
      (screen.y + viewport_height * 0.5f) / viewport_height,
  };

  int raster_x = floorf(ndc.x * canvas_width);
  int raster_y = floorf((1 - ndc.y) * canvas_height);

  draw_pixel(frame_buffer, raster_x, raster_y, RED);
}
