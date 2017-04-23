#ifndef Vector4_h__
#define Vector4_h__
#include "MathsForward.h"
template <typename T>
struct Vector4
{
  // Data
  T x, y, z, w;

  // Creation Constructor
  Vector4(T _x = 0, T _y = 0, T _z = 0, T _w = 0) : x(_x), y(_y), z(_z), w(_w) { }

  // Move Constructor
  Vector4(Vector4 &&move) { x = move.x; y = move.y; z = move.z; w = move.w; }

  // Copy Constructor
  Vector4(const Vector4 &copy) { x = copy.x; y = copy.y; z = copy.z; w = copy.w; }

  // Move Operation
  Vector4& operator=(Vector4 &&move) { x = move.x; y = move.y; z = move.z; w = move.w; return (*this); }

  // Copy Operation
  Vector4& operator=(const Vector4 &copy) { x = copy.x; y = copy.y; z = copy.z; w = copy.w; return (*this); }

  // Utility Operations
  const T *Data() const { return &x; }
  T Length() const { return sqrt(x * x + y * y + z * z + w * w); }
  T LengthSquared() const { return x * x + y * y + z * z + w * w; }
  Vector4 Normalized() const { return Vector4(x, y, z, w) / Length(); }
  T DotProduct(const Vector4 &o) const { return T(x * o.x + y * o.y + z * o.z + w * o.w); }
  //uint32_t ConvertTo32BitRGBA() const { return int(x * 255) + (int(y * 255) << 8) + (int(z * 255) << 16) + (int(w * 255) << 24); }

  Vector4 operator*(const mat4 &rhs) { return rhs * Vector4(x, y, z, w); }

  // Scalar Algebra Operations
  Vector4<T>& operator +=(T o) const { x += o; y += o; z += o; w += o; return *this; }
  Vector4<T>& operator -=(T o) const { x -= o; y -= o; z -= o; w -= o; return *this; }
  Vector4<T>& operator *=(T o) const { x *= o; y *= o; z *= o; w *= o; return *this; }
  Vector4<T>& operator /=(T o) const { x /= o; y /= o; z /= o; w /= o; return *this; }
  Vector4<T> operator +(T o) const { Vector4<T> n(x + o, y + o, z + o, w + o); return n; }
  Vector4<T> operator -(T o) const { Vector4<T> n(x - o, y - o, z - o, w - o); return n; }
  Vector4<T> operator *(T o) const { Vector4<T> n(x * o, y * o, z * o, w * o); return n; }
  Vector4<T> operator /(T o) const { Vector4<T> n(x / o, y / o, z / o, w / o); return n; }

  // Vector Comparison Operations
  bool operator == (const Vector4<T> &o) const { return x == o.x && y == o.y && z == o.z && w == o.w; }
  bool operator != (const Vector4<T> &o) const { return x != o.x || y != o.y || z != o.z || w != o.w; }

  // Vector Algebra Operations
  Vector4<T>& operator +=(const Vector4<T> &o) const { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
  Vector4<T>& operator -=(const Vector4<T> &o) const { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
  Vector4<T>& operator *=(const Vector4<T> &o) const { x *= o.x; y *= o.y; z *= o.z; w *= o.w; return *this; }
  Vector4<T>& operator /=(const Vector4<T> &o) const { x /= o.x; y /= o.y; z /= o.z; w /= o.w; return *this; }
  Vector4<T> operator +(const Vector4<T> &o) const { Vector4<T> n(x + o.x, y + o.y, z + o.z, w + o.w); return n; }
  Vector4<T> operator -(const Vector4<T> &o) const { Vector4<T> n(x - o.x, y - o.y, z - o.z, w - o.w); return n; }
  Vector4<T> operator *(const Vector4<T> &o) const { Vector4<T> n(x * o.x, y * o.y, z * o.z, w * o.w); return n; }
  Vector4<T> operator /(const Vector4<T> &o) const { Vector4<T> n(x / o.x, y / o.y, z / o.z, w / o.w); return n; }
};

typedef Vector4<float> vec4;
typedef Vector4<int> vec4i;

#endif // Vector4_h__
