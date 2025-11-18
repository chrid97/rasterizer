#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define IMAGE_HEIGHT 500
#define IMAGE_WIDTH 500
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

void draw_pixel(u8 image_buffer[BUFFER_SIZE], float rx, float ry) {
  int x = (int)(IMAGE_WIDTH / 2) + rx;
  int y = (int)(IMAGE_HEIGHT / 2) + ry;
  int index = y * (IMAGE_WIDTH * CHANNELS) + (x * CHANNELS);
  printf("(%i, %i) %i\n", x, y, index);
  image_buffer[index] = 0;
  image_buffer[index + 1] = 0;
  image_buffer[index + 2] = 0;
}

void interpolate(float buffer[1000], float i0, float d0, float i1, float d1) {
  float a = (d1 - d0) / (i1 - i0);
  float d = d0;
  int count = 0;

  for (int i = i0; i <= i1; i++) {
    buffer[count++] = d;
    d += a;
  }
}

void draw_line(u8 image_buffer[BUFFER_SIZE], Vector3 P0, Vector3 P1) {
  float buffer[1000];
  int buffer_count = 0;

  if (fabsf(P1.x - P0.x) > fabsf(P1.y - P0.y)) {
    if (P0.x > P1.x) {
      Vector3 tmp = P0;
      P0 = P1;
      P1 = tmp;
    }
    interpolate(buffer, P0.x, P0.y, P1.x, P1.y);
    for (int x = P0.x; x <= P1.x; x++) {
      int y = x - P0.x;
      draw_pixel(image_buffer, x, buffer[y]);
    }
  } else {
    if (P0.y > P1.y) {
      Vector3 tmp = P0;
      P0 = P1;
      P1 = tmp;
    }
    interpolate(buffer, P0.y, P0.x, P1.y, P1.x);
    for (int y = P0.y; y <= P1.y; y++) {
      int x = y - P0.y;
      draw_pixel(image_buffer, buffer[x], y);
    }
  }
}

void draw_wireframe_triangle(u8 image_buffer[BUFFER_SIZE], Vector3 P0,
                             Vector3 P1, Vector3 P2) {
  draw_line(image_buffer, P0, P1);
  draw_line(image_buffer, P1, P2);
  draw_line(image_buffer, P2, P0);
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

  draw_wireframe_triangle(image_buffer, P0, P1, P2);

  fprintf(file, "P3\n%i %i\n 255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
  for (int i = 0; i < BUFFER_SIZE; i += 3) {
    int r = image_buffer[i];
    int g = image_buffer[i + 1];
    int b = image_buffer[i + 2];
    fprintf(file, "%i %i %i\n", r, g, b);
  }

  return 0;
}
