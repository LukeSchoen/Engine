#ifndef matrix_h__
#define matrix_h__

#include "maths.h"

template <typename T>
struct MAT4
{
  T a[16];

  MAT4<T> operator*(const MAT4<T> &o) const
  {
    MAT4 o =
    {
      a[0] * o.a[0] + a[4] * o.a[1] + a[8] * o.a[2] + a[12] * o.a[3],
      a[1] * o.a[0] + a[5] * o.a[1] + a[9] * o.a[2] + a[13] * o.a[3],
      a[2] * o.a[0] + a[6] * o.a[1] + a[10] * o.a[2] + a[14] * o.a[3],
      a[3] * o.a[0] + a[7] * o.a[1] + a[11] * o.a[2] + a[15] * o.a[3],
      a[0] * o.a[4] + a[4] * o.a[5] + a[8] * o.a[6] + a[12] * o.a[7],
      a[1] * o.a[4] + a[5] * o.a[5] + a[9] * o.a[6] + a[13] * o.a[7],
      a[2] * o.a[4] + a[6] * o.a[5] + a[10] * o.a[6] + a[14] * o.a[7],
      a[3] * o.a[4] + a[7] * o.a[5] + a[11] * o.a[6] + a[15] * o.a[7],
      a[0] * o.a[8] + a[4] * o.a[9] + a[8] * o.a[10] + a[12] * o.a[11],
      a[1] * o.a[8] + a[5] * o.a[9] + a[9] * o.a[10] + a[13] * o.a[11],
      a[2] * o.a[8] + a[6] * o.a[9] + a[10] * o.a[10] + a[14] * o.a[11],
      a[3] * o.a[8] + a[7] * o.a[9] + a[11] * o.a[10] + a[15] * o.a[11],
      a[0] * o.a[12] + a[4] * o.a[13] + a[8] * o.a[14] + a[12] * o.a[15],
      a[1] * o.a[12] + a[5] * o.a[13] + a[9] * o.a[14] + a[13] * o.a[15],
      a[2] * o.a[12] + a[6] * o.a[13] + a[10] * o.a[14] + a[14] * o.a[15],
      a[3] * o.a[12] + a[7] * o.a[13] + a[11] * o.a[14] + a[15] * o.a[15],
    };

    return o;
  }

  Vector4<T> operator*(const Vector4<T> &v) const
  {
    Vector4<T> o =
    {
      a[0] * v.x + a[4] * v.y + a[8] * v.z + a[12] * v.w,
      a[1] * v.x + a[5] * v.y + a[9] * v.z + a[13] * v.w,
      a[2] * v.x + a[6] * v.y + a[10] * v.z + a[14] * v.w,
      a[3] * v.x + a[7] * v.y + a[11] * v.z + a[15] * v.w,
    };

    return o;
  }

  inline MAT4<T> inverse() const
  {
    //Determinants of all 2x2 matrix combinations.
    T d1_6_5_2 = a[1] * a[6] - a[5] * a[2];
    T d1_7_5_3 = a[1] * a[7] - a[5] * a[3];
    T d1_10_9_2 = a[1] * a[10] - a[9] * a[2];
    T d1_11_9_3 = a[1] * a[11] - a[9] * a[3];
    T d1_14_13_2 = a[1] * a[14] - a[13] * a[2];
    T d1_15_13_3 = a[1] * a[15] - a[13] * a[3];

    T d2_7_6_3 = a[2] * a[7] - a[6] * a[3];
    T d2_11_10_3 = a[2] * a[11] - a[10] * a[3];
    T d2_15_14_3 = a[2] * a[15] - a[14] * a[3];

    T d5_10_9_6 = a[5] * a[10] - a[9] * a[6];
    T d5_11_9_7 = a[5] * a[11] - a[9] * a[7];
    T d5_14_13_6 = a[5] * a[14] - a[13] * a[6];
    T d5_15_13_7 = a[5] * a[15] - a[13] * a[7];

    T d6_11_10_7 = a[6] * a[11] - a[10] * a[7];
    T d6_15_14_7 = a[6] * a[15] - a[14] * a[7];

    T d9_14_13_10 = a[9] * a[14] - a[13] * a[10];
    T d9_15_13_11 = a[9] * a[15] - a[13] * a[11];

    T d10_15_14_11 = a[10] * a[15] - a[14] * a[11];

    MAT4<T> o;

    //Determinants of minor matrices made from excluding column i and row j.
    o.a[0] = a[5] * d10_15_14_11 - a[9] * d6_15_14_7 + a[13] * d6_11_10_7;
    o.a[1] = a[4] * d10_15_14_11 - a[8] * d6_15_14_7 + a[12] * d6_11_10_7;
    o.a[2] = a[4] * d9_15_13_11 - a[8] * d5_15_13_7 + a[12] * d5_11_9_7;
    o.a[3] = a[4] * d9_14_13_10 - a[8] * d5_14_13_6 + a[12] * d5_10_9_6;
    o.a[4] = a[1] * d10_15_14_11 - a[9] * d2_15_14_3 + a[13] * d2_11_10_3;
    o.a[5] = a[0] * d10_15_14_11 - a[8] * d2_15_14_3 + a[12] * d2_11_10_3;
    o.a[6] = a[0] * d9_15_13_11 - a[8] * d1_15_13_3 + a[12] * d1_11_9_3;
    o.a[7] = a[0] * d9_14_13_10 - a[8] * d1_14_13_2 + a[12] * d1_10_9_2;
    o.a[8] = a[1] * d6_15_14_7 - a[5] * d2_15_14_3 + a[13] * d2_7_6_3;
    o.a[9] = a[0] * d6_15_14_7 - a[4] * d2_15_14_3 + a[12] * d2_7_6_3;
    o.a[10] = a[0] * d5_15_13_7 - a[4] * d1_15_13_3 + a[12] * d1_7_5_3;
    o.a[11] = a[0] * d5_14_13_6 - a[4] * d1_14_13_2 + a[12] * d1_6_5_2;
    o.a[12] = a[1] * d6_11_10_7 - a[5] * d2_11_10_3 + a[9] * d2_7_6_3;
    o.a[13] = a[0] * d6_11_10_7 - a[4] * d2_11_10_3 + a[8] * d2_7_6_3;
    o.a[14] = a[0] * d5_11_9_7 - a[4] * d1_11_9_3 + a[8] * d1_7_5_3;
    o.a[15] = a[0] * d5_10_9_6 - a[4] * d1_10_9_2 + a[8] * d1_6_5_2;

    //Coefficients of previous matrix.
    o.a[1] = -o.a[1];
    o.a[3] = -o.a[3];
    o.a[4] = -o.a[4];
    o.a[6] = -o.a[6];
    o.a[9] = -o.a[9];
    o.a[11] = -o.a[11];
    o.a[12] = -o.a[12];
    o.a[14] = -o.a[14];

    //Determinant
    T d = a[0] * o.a[0] + a[4] * o.a[4] + a[8] * o.a[8] + a[12] * o.a[12];

    //Transposition of previous matrix.
    T c1 = o.a[1];
    T c2 = o.a[2];
    T c3 = o.a[3];
    T c6 = o.a[6];
    T c7 = o.a[7];
    T c11 = o.a[11];

    o.a[1] = o.a[4];
    o.a[2] = o.a[8];
    o.a[3] = o.a[12];
    o.a[6] = o.a[9];
    o.a[7] = o.a[13];
    o.a[11] = o.a[14];

    o.a[4] = c1;
    o.a[8] = c2;
    o.a[12] = c3;
    o.a[9] = c6;
    o.a[13] = c7;
    o.a[14] = c11;

    T dr = T(1) / d;

    o.a[0] *= dr;
    o.a[1] *= dr;
    o.a[2] *= dr;
    o.a[3] *= dr;
    o.a[4] *= dr;
    o.a[5] *= dr;
    o.a[6] *= dr;
    o.a[7] *= dr;
    o.a[8] *= dr;
    o.a[9] *= dr;
    o.a[10] *= dr;
    o.a[11] *= dr;
    o.a[12] *= dr;
    o.a[13] *= dr;
    o.a[14] *= dr;
    o.a[15] *= dr;

    return o;
  }

  template <typename U>
  static inline MAT4<T> create(const MAT4<U> &m)
  {
    MAT4 o =
    {
      T(m.a[0]), T(m.a[1]), T(m.a[2]), T(m.a[3]),
      T(m.a[4]), T(m.a[5]), T(m.a[6]), T(m.a[7]),
      T(m.a[8]), T(m.a[9]), T(m.a[10]), T(m.a[11]),
      T(m.a[12]), T(m.a[13]), T(m.a[14]), T(m.a[15]),
    };

    return o;
  }

  static inline MAT4<T> identity()
  {
    MAT4 o =
    {
      T(1), T(0), T(0), T(0),
      T(0), T(1), T(0), T(0),
      T(0), T(0), T(1), T(0),
      T(0), T(0), T(0), T(1),
    };

    return o;
  }

  static inline MAT4<T> translate(T x, T y, T z)
  {
    MAT4 o =
    {
      T(1), T(0), T(0), 0,
      T(0), T(1), T(0), 0,
      T(0), T(0), T(1), 0,
      T(x), T(y), T(z), T(1),
    };

    return o;
  }

  static inline MAT4<T> translate(Vector3<T> v)
  {
    return MAT4<T>::translate(v.x, v.y, v.z);
  }

  //TODO: This seems to be broken at weird angles; investigate.
  static inline MAT4<T> rotateQuaternion(const Quaternion<T> &q)
  {
    T xy = q.x * q.y;
    T xz = q.x * q.z;
    T yz = q.y * q.z;

    T wx = q.w * q.x;
    T wy = q.w * q.y;
    T wz = q.w * q.z;

    T xx = q.x * q.x;
    T yy = q.y * q.y;
    T zz = q.z * q.z;

    MAT4 o =
    {
      T(1) - T(2) * yy - T(2) * zz,	T(2) * xy + T(2) * wz,	T(2) * xz - T(2) * wy,			T(0),
      T(2) * xy - T(2) * wz,			T(1) - T(2) * xx - zz,	T(2) * yz + T(2) * wx,			T(0),
      T(2) * xz + T(2) * wy,			T(2) * yz - T(2) * wx,	T(1) - T(2) * xx - T(2) * yy,	T(0),
      T(0),							T(0),					T(0),							T(1),
    };

    return o;
  }

  static inline MAT4<T> rotateEuler(T x, T y, T z)
  {
    T sX = T(sin(x));
    T sY = T(sin(y));
    T sZ = T(sin(z));
    T cX = T(cos(x));
    T cY = T(cos(y));
    T cZ = T(cos(z));

    MAT4 o =
    {
      cZ * cY,					sZ,			cZ * -sY,					T(0),
      sZ * -cY * cX + sY * sX,	cZ * cX,	sZ * sY * cX + cY * sX,		T(0),
      sZ * cY * sX + sY * cX,		-cZ * sX,	sZ * -sY * sX + cY * cX,	T(0),
      T(0),						T(0),		T(0),						T(1),
    };

    return o;
  }

  static inline MAT4<T> rotateEuler(Vector3<T> r)
  {
    return MAT4<T>::rotateEuler(r.x, r.y, r.z);
  }

  static inline MAT4<T> scaleUniform(T s)
  {
    MAT4 o =
    {
      T(s), T(0), T(0), T(0),
      T(0), T(s), T(0), T(0),
      T(0), T(0), T(s), T(0),
      T(0), T(0), T(0), T(1),
    };

    return o;
  }

  static inline MAT4<T> scaleNonUniform(T x, T y, T z)
  {
    MAT4 o =
    {
      T(x), T(0), T(0), T(0),
      T(0), T(y), T(0), T(0),
      T(0), T(0), T(z), T(0),
      T(0), T(0), T(0), T(1),
    };

    return o;
  };

  static inline MAT4<T> perspective(T fov, T aspect, T zNear, T zFar)
  {
    T zRange = zNear - zFar;
    T tanHalfFov = tan(fov * T(0.5));

    MAT4 o =
    {
      T(1) / (aspect * tanHalfFov), T(0), T(0), T(0),
      T(0), T(1) / tanHalfFov, T(0), T(0),
      T(0), T(0), -zFar / zRange, T(1),
      T(0), T(0), zFar * zNear / zRange, T(0),
    };

    return o;
  }
};

typedef MAT4<float> FMAT;

#endif // matrix_h__