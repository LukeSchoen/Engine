#ifndef Matrix4x4_h__
#define Matrix4x4_h__

#include "Maths.h"

struct Matrix4x4
{
  Matrix4x4();
  Matrix4x4(float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33);

  void LoadIdentity();
  void Zero();
  void Ortho(float left, float right, float bottom, float top, float near, float far);
  void Ortho(float left, float right, float bottom, float top);
  void Perspective(float FOV, float aspectRatio, float near, float far);

  void Translate(float x, float y, float z);
  void Translate(const vec3 &vec);
  void Scale(float x, float y, float z);
  void Scale(float num);
  void Scale(const vec3 &vec);
  void Rotate(float x, float y, float z);
  void Rotate(const vec3 &rot);
  void RotateDegrees(float x, float y, float z);
  void RotateX(float radians);
  void RotateY(float radians);
  void RotateZ(float radians);

  float determinant() const;
  Matrix4x4& inverse();
  Matrix4x4& Transpose();

  float& Get(int i, int j);

  vec4 operator*(const vec4& vec) const;
  Matrix4x4 operator*(const Matrix4x4 &mat2) const;
  void operator*=(const Matrix4x4 &mat2);
  void operator=(const Matrix4x4 &mat2);

  float m[4][4];
};

typedef Matrix4x4 mat4;

#endif // Matrix4x4_h__
