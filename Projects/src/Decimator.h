#ifndef Decimator_h__
#define Decimator_h__

#include "Maths.h"

struct MeshDecimator
{
  class SymetricMatrix {

  public:

    // Constructor
    SymetricMatrix(double c = 0)
    {
      for (int i = 0; i < 10; i++)
        m[i] = c;
    }

    SymetricMatrix(double m11, double m12, double m13, double m14,
      double m22, double m23, double m24,
      double m33, double m34,
      double m44)
    {
      m[0] = m11;  m[1] = m12;  m[2] = m13;  m[3] = m14;
      m[4] = m22;  m[5] = m23;  m[6] = m24;
      m[7] = m33;  m[8] = m34;
      m[9] = m44;
    }

    // Make plane
    SymetricMatrix(double a, double b, double c, double d)
    {
      m[0] = a*a;  m[1] = a*b;  m[2] = a*c;  m[3] = a*d;
      m[4] = b*b;  m[5] = b*c;  m[6] = b*d;
      m[7] = c*c; m[8] = c*d;
      m[9] = d*d;
    }

    double operator[](int c) const { return m[c]; }

    // Determinant
    double det(int a11, int a12, int a13,
      int a21, int a22, int a23,
      int a31, int a32, int a33)
    {
      double det = m[a11] * m[a22] * m[a33] + m[a13] * m[a21] * m[a32] + m[a12] * m[a23] * m[a31]
        - m[a13] * m[a22] * m[a31] - m[a11] * m[a23] * m[a32] - m[a12] * m[a21] * m[a33];
      return det;
    }

    const SymetricMatrix operator+(const SymetricMatrix& n) const
    {
      return SymetricMatrix(m[0] + n[0], m[1] + n[1], m[2] + n[2], m[3] + n[3],
        m[4] + n[4], m[5] + n[5], m[6] + n[6],
        m[7] + n[7], m[8] + n[8],
        m[9] + n[9]);
    }

    SymetricMatrix& operator+=(const SymetricMatrix& n)
    {
      m[0] += n[0];   m[1] += n[1];   m[2] += n[2];   m[3] += n[3];
      m[4] += n[4];   m[5] += n[5];   m[6] += n[6];   m[7] += n[7];
      m[8] += n[8];   m[9] += n[9];
      return *this;
    }

    double m[10];
  };

  struct Triangle
  {
    int v[3];
    double err[4];
    int deleted, dirty;
    vec3 n;
  };

  struct Vertex
  {
    vec3 p, c; int tstart, tcount; SymetricMatrix q; int border;
    Vertex(vec3 pos = vec3(), vec3 col = vec3())
    {
      p = pos;
      c = col;
    }
  };

  struct Ref { int tid, tvertex; };
  std::vector<Triangle> triangles;
  std::vector<Vertex> vertices;
  std::vector<Ref> refs;

  // To begin add each triangle
  void AddTriangle(vec3 p1, vec3 p2, vec3 p3, vec3 c1 = vec3(1, 1, 1), vec3 c2 = vec3(1, 1, 1), vec3 c3 = vec3(1, 1, 1));

  // Target_count  : target nr. of triangles, agressiveness : sharpness to increase the threshold, 5..8 are good numbers, more iterations = more quality
  void simplify_mesh(int target_count, double agressiveness = 7);

private:

  // Helper functions
  double vertex_error(SymetricMatrix q, double x, double y, double z);
  double calculate_error(int id_v1, int id_v2, vec3 &p_result);
  bool flipped(vec3 p, int i0, int i1, Vertex &v0, Vertex &v1, std::vector<int> &deleted);
  void update_triangles(int i0, Vertex &v, std::vector<int> &deleted, int &deleted_triangles);
  void update_mesh(int iteration);
  void compact_mesh();
};
#endif // Decimator_h__