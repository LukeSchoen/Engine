#include <xmmintrin.h>
#include <smmintrin.h>
#include "Maths.h"

static const __m128 MM_ONES = _mm_set1_ps(1.0f);

void MatVec(vec3* dst, mat4 matrix, const vec3* src, int num)
{
  const __m128 r0 = _mm_loadu_ps(matrix.m[0]);
  const __m128 r1 = _mm_loadu_ps(matrix.m[1]);
  const __m128 r2 = _mm_loadu_ps(matrix.m[2]);

  // unroll loop
  int i = 0;
  int maxPar = num - (num & 1);
  for (; i < maxPar; i += 2)
  {
    // Load [X0,Y0,Z0,1] into xyz1
    // Load [X1,Y1,Z1,1] into uvw1
    __m128 xyzu = _mm_loadu_ps(&src[i].x);  // X0 Y0 Z0 X1
    __m128 vw11 = _mm_loadl_pi(_mm_setzero_ps(), (const __m64*)&src[i + 1].y); // Y1 Z1 1 1    
    vw11 = _mm_loadh_pi(vw11, (const __m64*)&MM_ONES);
    // convert xyzuvw11 to xyz1 and uvw1
    __m128 zu1v = _mm_shuffle_ps(xyzu, vw11, _MM_SHUFFLE(0, 2, 3, 2)); // Z0 X1 1 Y1  
    __m128 xyz1 = _mm_shuffle_ps(xyzu, zu1v, _MM_SHUFFLE(2, 0, 1, 0)); // X0 Y0 Z0 1  
    __m128 uvw1 = _mm_shuffle_ps(zu1v, vw11, _MM_SHUFFLE(3, 1, 3, 1)); // X1 Y1 Z1 1

   // Perform matrix multiplication
    __m128 x = _mm_mul_ps(xyz1, r0);
    __m128 y = _mm_mul_ps(xyz1, r1);
    __m128 z = _mm_mul_ps(xyz1, r2);

    __m128 u = _mm_mul_ps(uvw1, r0);
    __m128 v = _mm_mul_ps(uvw1, r1);
    __m128 w = _mm_mul_ps(uvw1, r2);

    __m128 xy = _mm_hadd_ps(x, y);
    __m128 uv = _mm_hadd_ps(u, v);
    __m128 zw = _mm_hadd_ps(z, w);

    __m128 xyzw = _mm_hadd_ps(xy, zw);
    __m128 uvzw = _mm_hadd_ps(uv, zw);

    // Prepare the 6 output values (x'y'z'u'v'w')
    __m128 out_zuvw = _mm_shuffle_ps(uvzw, uvzw, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 out_xyzu = _mm_movelh_ps(xyzw, out_zuvw);

    _mm_storeu_ps(&dst[i].x, out_xyzu);           // write X,Y,Z,U
    _mm_storeh_pi((__m64*)&dst[i + 1].y, out_zuvw); // write V,W
  }
  // tail for loop unrolling
  for (; i < num; i++)
  {
    __m128 v = _mm_loadl_pi(MM_ONES, (__m64*)&src[i].x);
    v = _mm_insert_ps(v, _mm_load_ss(&src[i].z), 0x20);
    __m128 x = _mm_mul_ps(r0, v);
    __m128 y = _mm_mul_ps(r1, v);
    __m128 z = _mm_mul_ps(r2, v);
    __m128 xy = _mm_hadd_ps(x, y);
    __m128 zw = _mm_hadd_ps(z, y);
    __m128 out = _mm_hadd_ps(xy, zw); // x,y,z,?
    _mm_storel_pi((__m64*)&dst[i].x, out);
    _mm_store_ss(&dst[i].z, _mm_movehl_ps(out, out));
  }
}