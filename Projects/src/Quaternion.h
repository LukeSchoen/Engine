#ifndef Quaternion_h__
#define Quaternion_h__

#include "MathsForward.h"

template <typename T>
struct Quaternion
{
  // Data
  T x, y, z, w;

  // Creation Constructor
  Quaternion(T _x = 0, T _y = 0, T _z = 0, T _w = 0) : x(_x), y(_y), z(_z), w(_w) { }

  // Move Constructor
  Quaternion(Quaternion &&move) { x = move.x; y = move.y; z = move.z; w = move.w; }

  // Copy Constructor
  Quaternion(const Quaternion &copy) { x = copy.x; y = copy.y; z = copy.z; w = copy.w; }

  // Move Operation
  Quaternion& operator=(Quaternion &&move) { x = move.x; y = move.y; z = move.z; w = move.w; return (*this); }

  // Copy Operation
  Quaternion& operator=(const Quaternion &copy) { x = copy.x; y = copy.y; z = copy.z; w = copy.w; return (*this); }

  T LengthSquared() { return vec4(x, y, z, w).LengthSquared(); }
  T Length() { return vec4(x, y, z, w).Length(); }

//   // Quaternion Vector Operations
//   Vector3 <T> Apply(const Vector3<T> &o) const
//   {
//     Vector3<T> qv(x, y, z);
//     return T(2) * qv.DotProduct(o) * qv
//       + (w * w - qv.DotProduct(qv)) * o
//       + T(2) * w * qv.CrossProduct(o);
//   }

  // Quaternion Quaternion Operations
  Quaternion<T> operator*(const Quaternion<T> &q) const
  {
    Quaternion<T> o;
    o.x = x * q.x - y * q.y - z * q.z - w * q.w;
    o.y = x * q.y + y * q.x + z * q.w - w * q.z;
    o.z = x * q.z - y * q.w + z * q.x + w * q.y;
    o.w = x * q.w + y * q.z - z * q.y + w * q.x;
    return o;
  }

  void GetMatrix(mat4 *mat)
  {
    T qxx(x * x);
    T qyy(y * y);
    T qzz(z * z);
    T qxz(x * z);
    T qxy(x * y);
    T qyz(y * z);
    T qwx(w * x);
    T qwy(w * y);
    T qwz(w * z);
    (*mat).m[0][0] = 1 - 2 * (qyy + qzz);
    (*mat).m[0][1] = 2 * (qxy + qwz);
    (*mat).m[0][2] = 2 * (qxz - qwy);
    (*mat).m[1][0] = 2 * (qxy - qwz);
    (*mat).m[1][1] = 1 - 2 * (qxx + qzz);
    (*mat).m[1][2] = 2 * (qyz + qwx);
    (*mat).m[2][0] = 2 * (qxz + qwy);
    (*mat).m[2][1] = 2 * (qyz - qwx);
    (*mat).m[2][2] = 1 - 2 * (qxx + qyy);
   }
};

typedef Quaternion<float> Quat;
typedef Quaternion<int> Quati;

#endif // Quaternion_h__