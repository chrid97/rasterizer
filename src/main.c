#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)

#define IMAGE_HEIGHT 1000
#define IMAGE_WIDTH 1000
#define VIEWPORT_HEIGHT 1
#define VIEWPORT_WIDTH 1
#define CHANNELS 3
#define BUFFER_SIZE IMAGE_WIDTH * IMAGE_HEIGHT * CHANNELS

// SIGNED
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// UNSIGNED
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct Arena Arena;
struct Arena {
  u8 *base;
  u64 size;
  u64 offset;
};

Arena *arena_alloc(u64 size) {
  Arena *arena = malloc(sizeof(Arena));
  arena->base = malloc(size);
  arena->size = size;
  arena->offset = 0;

  return arena;
}

void *arena_push(Arena *arena, u64 size) {
  assert(arena->offset + size <= arena->size);
  void *ptr = arena->base + arena->offset;
  arena->offset += size;
  return ptr;
}

void arena_free(Arena *a) { a->offset = 0; }

typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  float x;
  float y;
  float z;
} Vector3;

Vector3 Vector3Scale(Vector3 v1, float scalar) {
  return (Vector3){
      v1.x * scalar,
      v1.y * scalar,
      v1.z * scalar,
  };
}

Vector3 Vector3AddValue(Vector3 v1, float value) {
  return (Vector3){
      v1.x * value,
      v1.y * value,
      v1.z * value,
  };
}

Vector3 Vector3Subtract(Vector3 v1, Vector3 v2) {
  return (Vector3){
      v1.x - v2.x,
      v1.y - v2.y,
      v1.z - v2.z,
  };
}

Vector3 Vector3Add(Vector3 v1, Vector3 v2) {
  return (Vector3){
      v1.x + v2.x,
      v1.y + v2.y,
      v1.z + v2.z,
  };
}

float Vector3DotProduct(Vector3 v1, Vector3 v2) {
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
};

float Vector3Length(Vector3 v1) { return sqrtf(Vector3DotProduct(v1, v1)); }

Vector3 Vector3Normalize(Vector3 v) {
  float len = Vector3Length(v);
  return (Vector3){v.x / len, v.y / len, v.z / len};
}

Vector3 Vector3Negate(Vector3 v) { return (Vector3){-v.x, -v.y, -v.z}; }

void swap(Vector2 *a, Vector2 *b) {
  Vector2 tmp = *a;
  *a = *b;
  *b = tmp;
}

void draw_pixel(u8 image_buffer[BUFFER_SIZE], float rx, float ry,
                Vector3 color) {
  int x = (int)(IMAGE_WIDTH / 2) + rx;
  int y = (int)(IMAGE_HEIGHT / 2) - ry;
  if (x < 0 || x >= IMAGE_WIDTH || y < 0 || y >= IMAGE_HEIGHT) {
    return;
  }

  int index = y * (IMAGE_WIDTH * CHANNELS) + (x * CHANNELS);
  image_buffer[index] = color.x;
  image_buffer[index + 1] = color.y;
  image_buffer[index + 2] = color.z;
}

float *interpolate(Arena *arena, float i0, float d0, float i1, float d1) {
  int start = (int)i0;
  int end = (int)i1;
  int count = end - start + 1;
  assert(count > 0);

  float *buffer = arena_push(arena, sizeof(float) * count);

  float a = (d1 - d0) / (i1 - i0);
  float d = d0;
  for (int i = 0; i < count; i++) {
    buffer[i] = d;
    d += a;
  }

  return buffer;
}

void draw_line(Arena *arena, u8 image_buffer[BUFFER_SIZE], Vector2 P0,
               Vector2 P1, Vector3 color) {
  if (fabsf(P1.x - P0.x) > fabsf(P1.y - P0.y)) {
    if (P0.x > P1.x) {
      swap(&P0, &P1);
    }

    int start_x = (int)P0.x;
    int end_x = (int)P1.x;
    int count = end_x - start_x + 1;

    float *buffer = interpolate(arena, P0.x, P0.y, P1.x, P1.y);
    for (int x = start_x; x <= end_x; x++) {
      int index = x - start_x;
      draw_pixel(image_buffer, x, buffer[index], color);
    }
  } else {
    if (P0.y > P1.y) {
      swap(&P0, &P1);
    }

    int start_y = (int)P0.y;
    int end_y = (int)P1.y;
    int count = end_y - start_y + 1;

    float *buffer = interpolate(arena, P0.y, P0.x, P1.y, P1.x);

    for (int y = start_y; y <= end_y; ++y) {
      int index = y - start_y;
      draw_pixel(image_buffer, buffer[index], y, color);
    }
  }
}

void draw_wireframe_triangle(Arena *arena, u8 image_buffer[BUFFER_SIZE],
                             Vector2 P0, Vector2 P1, Vector2 P2,
                             Vector3 color) {
  draw_line(arena, image_buffer, P0, P1, color);
  draw_line(arena, image_buffer, P1, P2, color);
  draw_line(arena, image_buffer, P2, P0, color);
}

void draw_filled_triangle(Arena *arena, u8 image_buffer[BUFFER_SIZE],
                          Vector2 P0, Vector2 P1, Vector2 P2, Vector3 color) {
  if (P1.y < P0.y) {
    swap(&P0, &P1);
  }
  if (P2.y < P0.y) {
    swap(&P0, &P2);
  }
  if (P2.y < P1.y) {
    swap(&P1, &P2);
  }

  float *x01 = interpolate(arena, P0.y, P0.x, P1.y, P1.x);
  float *x12 = interpolate(arena, P1.y, P1.x, P2.y, P2.x);
  float *x02 = interpolate(arena, P0.y, P0.x, P2.y, P2.x);

  int x01_count = (int)P1.y - (int)P0.y + 1;
  int x12_count = (int)P2.y - (int)P1.y + 1;
  int x012_count = x01_count + x12_count;

  float *x012 = arena_push(arena, sizeof(float) * x012_count);

  for (int i = 0; i < x01_count; i++) {
    x012[i] = x01[i];
  }
  for (int i = 0; i < x12_count; i++) {
    x012[x01_count + i] = x12[i];
  }

  int m = x012_count / 2;
  float *x_left = x02;
  float *x_right = x012;

  if (x02[m] > x012[m]) {
    x_left = x012;
    x_right = x02;
  }

  for (int y = (int)P0.y; y <= (int)P2.y; y++) {
    int index = y - (int)P0.y;
    for (int x = (int)x_left[index]; x <= (int)x_right[index]; x++) {
      draw_pixel(image_buffer, x, y, color);
    }
  }

  arena_free(arena);
}

void draw_shaded_triangle(Arena *arena, u8 image_buffer[BUFFER_SIZE],
                          Vector2 P0, Vector2 P1, Vector2 P2, Vector3 color,
                          float h0, float h1, float h2) {
  if (P1.y < P0.y) {
    swap(&P0, &P1);
  }
  if (P2.y < P0.y) {
    swap(&P0, &P2);
  }
  if (P2.y < P1.y) {
    swap(&P1, &P2);
  }

  float *x01 = interpolate(arena, P0.y, P0.x, P1.y, P1.x);
  float *h01 = interpolate(arena, P0.y, h0, P1.y, h1);

  float *x12 = interpolate(arena, P1.y, P1.x, P2.y, P2.x);
  float *h12 = interpolate(arena, P1.y, h1, P2.y, h2);

  float *x02 = interpolate(arena, P0.y, P0.x, P2.y, P2.x);
  float *h02 = interpolate(arena, P0.y, h0, P2.y, h2);

  int x01_count = (int)P1.y - (int)P0.y + 1;
  int x12_count = (int)P2.y - (int)P1.y + 1;
  int x012_count = x01_count + x12_count;

  float *x012 = arena_push(arena, sizeof(float) * x012_count);
  for (int i = 0; i < x01_count; i++) {
    x012[i] = x01[i];
  }
  for (int i = 0; i < x12_count; i++) {
    x012[x01_count + i] = x12[i];
  }

  float *h012 = arena_push(arena, sizeof(float) * x012_count);
  for (int i = 0; i < x01_count; i++) {
    h012[i] = h01[i];
  }
  for (int i = 0; i < x12_count; i++) {
    h012[x01_count + i] = h12[i];
  }

  int m = x01_count;
  float *x_left = x02;
  float *h_left = h02;
  float *x_right = x012;
  float *h_right = h012;

  if (x02[m] > x012[m]) {
    x_left = x012;
    h_left = h012;
    x_right = x02;
    h_right = h02;
  }

  for (int y = (int)P0.y; y <= (int)P2.y; y++) {
    int index = y - (int)P0.y;
    float xl = (int)x_left[index];
    float xr = (int)x_right[index];

    float *h_segment =
        interpolate(arena, xl, h_left[index], xr, h_right[index]);
    for (int x = (int)x_left[index]; x <= (int)x_right[index]; x++) {
      Vector3 shaded_color = Vector3Scale(color, h_segment[x - (int)xl]);
      draw_pixel(image_buffer, x, y, shaded_color);
    }
  }

  arena_free(arena);
}

Vector2 viewport_to_canvas(float x, float y) {
  Vector2 result;
  result.x = (x * IMAGE_WIDTH) / (float)VIEWPORT_WIDTH;
  result.y = (y * IMAGE_HEIGHT) / (float)VIEWPORT_HEIGHT;
  return result;
}

Vector2 project_vertex(Vector3 vertex) {
  float d = 1.0f; // projection plane distance

  float px = d * vertex.x / vertex.z;
  float py = d * vertex.y / vertex.z;

  return viewport_to_canvas(px, py);
}

int main(void) {
  FILE *file = fopen("image.ppm", "w");
  if (!file) {
    fprintf(stderr, "Failed to open file for writing.\n");
    return 1;
  }

  Arena *scratch_arena = arena_alloc(Megabytes(4));

  u8 image_buffer[BUFFER_SIZE] = {0};
  memset(image_buffer, 255, BUFFER_SIZE);

  Vector3 red = {255, 0, 0};
  Vector3 green = {0, 255, 0};
  Vector3 blue = {0, 0, 255};
  Vector3 yellow = {255, 255, 0};
  Vector3 purple = {255, 0, 255};
  Vector3 cyan = {0, 255, 255};

  Vector3 vertices[8] = {
      {1, 1, 1},    // 0
      {-1, 1, 1},   // 1
      {-1, -1, 1},  // 2
      {1, -1, 1},   // 3
      {1, 1, -1},   // 4
      {-1, 1, -1},  // 5
      {-1, -1, -1}, // 6
      {1, -1, -1},  // 7
  };

  int triangles[12][3] = {
      {0, 1, 2}, // red
      {0, 2, 3}, // red

      {4, 0, 3}, // green
      {4, 3, 7}, // green

      {5, 4, 7}, // blue
      {5, 7, 6}, // blue

      {1, 5, 6}, // yellow
      {1, 6, 2}, // yellow

      {4, 5, 1}, // purple
      {4, 1, 0}, // purple

      {2, 6, 7}, // cyan
      {2, 7, 3}, // cyan
  };

  Vector3 tri_colors[12] = {red,    red,    green,  green,  blue, blue,
                            yellow, yellow, purple, purple, cyan, cyan};

  Vector3 translation = {-1.5, 0, 7};

  for (int i = 0; i < 12; i++) {
    int v0 = triangles[i][0];
    int v1 = triangles[i][1];
    int v2 = triangles[i][2];

    Vector2 p0 = project_vertex(Vector3Add(vertices[v0], translation));
    Vector2 p1 = project_vertex(Vector3Add(vertices[v1], translation));
    Vector2 p2 = project_vertex(Vector3Add(vertices[v2], translation));

    draw_wireframe_triangle(scratch_arena, image_buffer, p0, p1, p2,
                            tri_colors[i]);
  }

  fprintf(file, "P3\n%i %i\n 255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  for (int i = 0; i < BUFFER_SIZE; i += 3) {
    int r = image_buffer[i];
    int g = image_buffer[i + 1];
    int b = image_buffer[i + 2];
    fprintf(file, "%i %i %i\n", r, g, b);
  }

  return 0;
}
