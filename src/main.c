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
  Arena *current;
  u64 size;
};

Arena *arena_alloc(u64 size) {
  Arena *arena = malloc(size);
  arena->current = arena;
  arena->size = size;
  return arena;
}

void *arena_push(Arena *arena, u64 size) { arena->current += size; }

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

void swap(Vector3 *a, Vector3 *b) {
  Vector3 tmp = *a;
  *a = *b;
  *b = tmp;
}

void draw_pixel(u8 image_buffer[BUFFER_SIZE], float rx, float ry,
                Vector3 color) {
  int x = (int)(IMAGE_WIDTH / 2) + rx;
  int y = (int)(IMAGE_HEIGHT / 2) - ry;
  int index = y * (IMAGE_WIDTH * CHANNELS) + (x * CHANNELS);
  image_buffer[index] = color.x;
  image_buffer[index + 1] = color.y;
  image_buffer[index + 2] = color.z;
}

void interpolate(Arena *arena, float i0, float d0, float i1, float d1) {
  float a = (d1 - d0) / (i1 - i0);
  float d = d0;
  int count = 0;

  for (int i = i0; i <= i1; i++) {
    // buffer[count++] = d;
    d += a;
  }
}

void draw_line(u8 image_buffer[BUFFER_SIZE], Vector3 P0, Vector3 P1,
               Vector3 color) {
  float buffer[1000];
  int buffer_count = 0;

  if (fabsf(P1.x - P0.x) > fabsf(P1.y - P0.y)) {
    if (P0.x > P1.x) {
      swap(&P0, &P1);
    }
    interpolate(buffer, P0.x, P0.y, P1.x, P1.y);
    for (int x = P0.x; x <= P1.x; x++) {
      int y = x - P0.x;
      draw_pixel(image_buffer, x, buffer[y], color);
    }
  } else {
    if (P0.y > P1.y) {
      swap(&P0, &P1);
    }
    interpolate(buffer, P0.y, P0.x, P1.y, P1.x);
    for (int y = P0.y; y <= P1.y; y++) {
      int x = y - P0.y;
      draw_pixel(image_buffer, buffer[x], y, color);
    }
  }
}

void draw_wireframe_triangle(u8 image_buffer[BUFFER_SIZE], Vector3 P0,
                             Vector3 P1, Vector3 P2, Vector3 color) {
  draw_line(image_buffer, P0, P1, color);
  draw_line(image_buffer, P1, P2, color);
  draw_line(image_buffer, P2, P0, color);
}

void draw_filled_triangle(u8 image_buffer[BUFFER_SIZE], Vector3 P0, Vector3 P1,
                          Vector3 P2, Vector3 color) {
  if (P1.y < P0.y) {
    swap(&P0, &P1);
  }
  if (P2.y < P0.y) {
    swap(&P0, &P2);
  }
  if (P2.y < P1.y) {
    swap(&P1, &P2);
  }

  // TODO replace this buffer with an arena
  float x01[1000];
  interpolate(x01, P0.x, P0.y, P1.x, P1.y);
  float x12[1000];
  interpolate(x12, P1.y, P1.x, P2.y, P2.x);
  float x02[1000];
  interpolate(x02, P0.y, P0.x, P2.y, P2.x);
}

int main(void) {
  FILE *file = fopen("image.ppm", "w");
  if (!file) {
    fprintf(stderr, "Failed to open file for writing.\n");
    return 1;
  }

  u8 image_buffer[BUFFER_SIZE] = {0};
  memset(image_buffer, 255, BUFFER_SIZE);

  Vector3 P0 = {-200, -250, 0};
  Vector3 P1 = {200, 50, 0};
  Vector3 P2 = {20, 250, 0};

  Vector3 RED = {255, 0, 0};
  Vector3 BLUE = {0, 255, 0};
  Vector3 GREEN = {0, 0, 255};
  Vector3 BLACK = {0, 0, 0};

  draw_wireframe_triangle(image_buffer, P0, P1, P2, BLACK);
  // draw_line(image_buffer, P0, P1, RED);

  fprintf(file, "P3\n%i %i\n 255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  for (int i = 0; i < BUFFER_SIZE; i += 3) {
    int r = image_buffer[i];
    int g = image_buffer[i + 1];
    int b = image_buffer[i + 2];
    fprintf(file, "%i %i %i\n", r, g, b);
  }

  return 0;
}
