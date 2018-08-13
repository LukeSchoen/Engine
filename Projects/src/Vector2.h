#ifndef Vector2_h__
#define Vector2_h__
template <typename T>
struct Vector2
{
  // Data
  T x, y;

  // Creation Constructor
  Vector2(T _x = 0, T _y = 0) { x = _x; y = _y; }

  // Move Constructor
  Vector2(Vector2 &&move) { x = move.x; y = move.y; }

  // Copy Constructor
  Vector2(const Vector2 &copy) { x = copy.x; y = copy.y; }

  // Move Operation
  Vector2& operator=(Vector2 &&move) { x = move.x; y = move.y; return (*this); }

  // Copy Operation
  Vector2& operator=(const Vector2 &copy) { x = copy.x; y = copy.y; return (*this); }

  // Utility Operations
  T *Data() const { return &x; }
  T Length() const { return sqrt(x * x + y * y); }
  T LengthSquared() const { return x * x + y * y; }
  Vector2 Normalized() const { return Vector2(x, y) / Length(); }
  T DotProduct(const Vector2 &o) const { return T(x * o.x + y * o.y); }

  // Scalar Algebra Operations
  Vector2<T>& operator +=(T o) const { x += o; y += o; return *this; }
  Vector2<T>& operator -=(T o) const { x -= o; y -= o; return *this; }
  Vector2<T>& operator *=(T o) const { x *= o; y *= o; return *this; }
  Vector2<T>& operator /=(T o) const { x /= o; y /= o; return *this; }
  Vector2<T> operator +(T o) const { Vector2<T> n(x + o, y + o); return n; }
  Vector2<T> operator -(T o) const { Vector2<T> n(x - o, y - o); return n; }
  Vector2<T> operator *(T o) const { Vector2<T> n(x * o, y * o); return n; }
  Vector2<T> operator /(T o) const { Vector2<T> n(x / o, y / o); return n; }

  // Vector Comparison Operations
  bool operator == (const Vector2<T> &o) const { return x == o.x && y == o.y; }
  bool operator != (const Vector2<T> &o) const { return x != o.x || y != o.y; }

  // Vector Algebra Operations
  Vector2<T>& operator +=(const Vector2<T> &o) const { x += o.x; y += o.y; return *this; }
  Vector2<T>& operator -=(const Vector2<T> &o) const { x -= o.x; y -= o.y; return *this; }
  Vector2<T>& operator *=(const Vector2<T> &o) const { x *= o.x; y *= o.y; return *this; }
  Vector2<T>& operator /=(const Vector2<T> &o) const { x /= o.x; y /= o.y; return *this; }
  Vector2<T> operator +(const Vector2<T> &o) const { Vector2<T> n(x + o.x, y + o.y); return n; }
  Vector2<T> operator -(const Vector2<T> &o) const { Vector2<T> n(x - o.x, y - o.y); return n; }
  Vector2<T> operator *(const Vector2<T> &o) const { Vector2<T> n(x * o.x, y * o.y); return n; }
  Vector2<T> operator /(const Vector2<T> &o) const { Vector2<T> n(x / o.x, y / o.y); return n; }
};

typedef Vector2<float> vec2;
typedef Vector2<int> vec2i;
typedef Vector2<unsigned int> vec2ui;

#endif // Vector2_h__