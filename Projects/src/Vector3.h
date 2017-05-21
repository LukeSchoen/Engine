#ifndef Vector3_h__
#define Vector3_h__
#include "stdint.h"
#include "Maths.h"
template <typename T>
struct Vector3
{
  // Data
  T x, y, z;

  // Creation Constructors
  Vector3() { x = 0; y = 0; z = 0; }
  Vector3(T _x) { x = _x; y = _x; z = _x; }
  Vector3(T _x, T _y) { x = _x; y = _y; z = 0; }
  Vector3(T _x, T _y, T _z) { x = _x; y = _y; z = _z; }

  template <typename S>
  Vector3(Vector3<S> o) { x = o.x; y = o.y; z = o.z; }

  //Vector3(vec4 create) { x = create.x; y = create.y; z = create.z; };
  Vector3(const vec4 &create) { x = create.x; y = create.y; z = create.z; };

  // Downgrade Constructor
  //Vector3(Vector4 create) { x = create.x; y = create.y; z = create.z; };
  //Vector3(const Vector4 &create) { x = create.x; y = create.y; z = create.z; };

  // Move Constructor
  Vector3(Vector3 &&move) { x = move.x; y = move.y; z = move.z; }

  // Copy Constructor
  Vector3(const Vector3 &copy) { x = copy.x; y = copy.y; z = copy.z; }

  // Move Operation
  Vector3& operator=(Vector3 &&move) { x = move.x; y = move.y; z = move.z; return (*this); }

  // Copy Operation
  Vector3& operator=(const Vector3 &copy) { x = copy.x; y = copy.y; z = copy.z; return (*this); }

  // Utility Operations
  T *Data() { return &x; }
  T Length() const { return sqrt(x * x + y * y + z * z); }
  T LengthSquared() const { return x * x + y * y + z * z; }
  Vector3 Normalized() const { return LengthSquared() > 0.0 ? Vector3(x, y, z) / Length() : *this; }
  T DotProduct(const Vector3 &o) const { return T(x * o.x + y * o.y + z * o.z); }
  Vector3 CrossProduct(const Vector3 &o) const { return Vector3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x); }
  uint32_t ConvertTo32BitRGBA() const { return int(x * 255) + (int(y * 255) << 8) + (int(z * 255) << 16) + (255 << 24); }

  // Scalar Algebra Operations
  Vector3<T>& operator +=(T o) const { x += o; y += o; z += o; return *this; }
  Vector3<T>& operator -=(T o) const { x -= o; y -= o; z -= o; return *this; }
  Vector3<T>& operator *=(T o) const { x *= o; y *= o; z *= o; return *this; }
  Vector3<T>& operator /=(T o) const { x /= o; y /= o; z /= o; return *this; }
  Vector3<T> operator +(T o) const { Vector3<T> n(x + o, y + o, z + o); return n; }
  Vector3<T> operator -(T o) const { Vector3<T> n(x - o, y - o, z - o); return n; }
  Vector3<T> operator *(T o) const { Vector3<T> n(x * o, y * o, z * o); return n; }
  Vector3<T> operator /(T o) const { Vector3<T> n(x / o, y / o, z / o); return n; }

  // Vector Comparison Operations
  bool operator == (const Vector3<T> &o) const { return x == o.x && y == o.y && z == o.z; }
  bool operator != (const Vector3<T> &o) const { return x != o.x || y != o.y || z != o.z; }

  // Vector Algebra Operations
  Vector3<T>& operator +=(const Vector3<T> &o) { x += o.x; y += o.y; z += o.z; return *this; }
  Vector3<T>& operator -=(const Vector3<T> &o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
  Vector3<T>& operator *=(const Vector3<T> &o) { x *= o.x; y *= o.y; z *= o.z; return *this; }
  Vector3<T>& operator /=(const Vector3<T> &o) { x /= o.x; y /= o.y; z /= o.z; return *this; }
  Vector3<T> operator +(const Vector3<T> &o) const { Vector3<T> n(x + o.x, y + o.y, z + o.z); return n; }
  Vector3<T> operator -(const Vector3<T> &o) const { Vector3<T> n(x - o.x, y - o.y, z - o.z); return n; }
  Vector3<T> operator *(const Vector3<T> &o) const { Vector3<T> n(x * o.x, y * o.y, z * o.z); return n; }
  Vector3<T> operator /(const Vector3<T> &o) const { Vector3<T> n(x / o.x, y / o.y, z / o.z); return n; }
};

typedef Vector3<float> vec3;
typedef Vector3<int> vec3i;

#endif // Vector3_h__
