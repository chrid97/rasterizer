#include <math.h>

typedef struct {
  float d[4][4];
} Matrix;

typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  float x;
  float y;
  float z;
} Vector3;

typedef struct {
  float x;
  float y;
  float z;
  float w;
} Vector4;

Vector4 vec4_new(float x, float y, float z) { return (Vector4){x, y, z, 1}; }

Vector4 vec_mult_matrix(Vector4 v, Matrix m) {
  Vector4 result;

  result.x =
      v.x * m.d[0][0] + v.y * m.d[0][1] + v.z * m.d[0][2] + v.w * m.d[0][3];

  result.y =
      v.x * m.d[1][0] + v.y * m.d[1][1] + v.z * m.d[1][2] + v.w * m.d[1][3];

  result.z =
      v.x * m.d[2][0] + v.y * m.d[2][1] + v.z * m.d[2][2] + v.w * m.d[2][3];

  result.w =
      v.x * m.d[3][0] + v.y * m.d[3][1] + v.z * m.d[3][2] + v.w * m.d[3][3];

  return result;
}

Vector3 Vector3Scale(Vector3 v1, float scalar) {
  return (Vector3){
      v1.x * scalar,
      v1.y * scalar,
      v1.z * scalar,
  };
}

Vector3 Vector3AddValue(Vector3 v1, float value) {
  return (Vector3){
      v1.x + value,
      v1.y + value,
      v1.z + value,
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

Vector4 vec4_cross(Vector4 v1, Vector4 v2) {
  return (Vector4){
      .x = (v1.y * v2.z) - (v1.z * v2.y),
      .y = (v1.z * v2.x) - (v1.x * v2.z),
      .z = (v1.x * v2.y) - (v1.y * v2.x),
      .w = 0.0f,
  };
}

float vec4_dot(Vector4 v1, Vector4 v2) {
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

Vector4 vec4_subtract(Vector4 v1, Vector4 v2) {
  return (Vector4){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, 0};
}

float vec4_length(Vector4 v) { return sqrtf(vec4_dot(v, v)); }

Vector4 vec4_normalize(Vector4 v) {
  float len = vec4_length(v);
  return (Vector4){v.x / len, v.y / len, v.z / len};
}

// float Vector3Length(Vector3 v1) { return sqrtf(Vector3DotProduct(v1, v1)); }

// Vector3 Vector3Normalize(Vector3 v) {
//   float len = Vector3Length(v);
//   return (Vector3){v.x / len, v.y / len, v.z / len};
// }

Vector3 Vector3Negate(Vector3 v) { return (Vector3){-v.x, -v.y, -v.z}; }

// Why does inling get rid of the ODR violation warning?
// static inline int abs(int value) {
//   if (value < 0) {
//     return -value;
//   }
//
//   return value;
// }
//
// static inline int absf(float value) {
//   if (value < 0) {
//     return -value;
//   }
//
//   return value;
// }

static inline int max(int num1, int num2) {
  if (num1 > num2) {
    return num1;
  }

  return num2;
}
