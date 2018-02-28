#ifndef Maths_h__
#define Maths_h__

// Vectors
#include "Vector2.h"
#include "Vector4.h"
#include "Vector3.h"

// Matrix
#include "Matrix4x4.h"

// Quaternion
#include "Quaternion.h"

// Algebra
#include <math.h>
#include "PI.h"
#include "Clamp.h"
#include "Exponent.h"
#include "Round.h"

// Geometry
#include "Ray.h"
#include "Box.h"
#include "Line.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "PolyMesh.h"

vec3 s_unpackRGB(uint32_t c);
uint32_t s_packRGB(vec3 c);

float IntersectRaySphere(vec3 rayStart, vec3 rayDir, vec3 sphereCentre, float radius);
bool RayWillHitTriangle(const vec3 V1, const vec3 V2, const vec3 V3, const vec3 RayPos, const vec3 RayDir, float *pCollisionT);
bool RayWillHitQuad(const vec3 V1, const vec3 V2, const vec3 V3, const vec3 V4, const vec3 RayPos, const vec3 RayDir, float *pCollisionT);

#endif // Maths_h__
