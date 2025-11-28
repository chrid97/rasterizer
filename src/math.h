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

// float Vector3Length(Vector3 v1) { return sqrtf(Vector3DotProduct(v1, v1)); }

// Vector3 Vector3Normalize(Vector3 v) {
//   float len = Vector3Length(v);
//   return (Vector3){v.x / len, v.y / len, v.z / len};
// }

Vector3 Vector3Negate(Vector3 v) { return (Vector3){-v.x, -v.y, -v.z}; }

// Why does inling get rid of the ODR violation warning?
inline int abs(int value) {
  if (value < 0) {
    return -value;
  }

  return value;
}

static inline int max(int num1, int num2) {
  if (num1 > num2) {
    return num1;
  }

  return num2;
}
