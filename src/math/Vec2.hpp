#ifndef FRUIT_VEC2_HPP
#define FRUIT_VEC2_HPP


#include <cmath>

#include "util.hpp"
#include "util/debug.hpp"


// A two-dimensional vector
template <typename Scalar_ = double>
struct Vec2x {
private:
  using Vec2 = Vec2x<Scalar_>;
public:
  using Scalar = Scalar_; // make Scalar public (template params are private)
  
  
  //#### DATA ####
  
  // You can refer to the members either as a[1], a[2] etc or a.y, a.z etc
  union {
    Scalar a[2];
    struct { Scalar x, y; };
  };
  
  
  //### CONSTRUCTORS ###
  
  Vec2x() : x(0), y(0) {}
  Vec2x(Scalar x, Scalar y) : x(x), y(y) {}
  Vec2x(const Vec2&) = default;
  
  
  //### OPERATORS ###
  
  // Vec[i] returns the i-th dimension of this vector
  Scalar& operator [] (int i) { return a[i]; }
  Scalar operator [] (int i) const { return a[i]; }
  
  
  //# vector operators #
  
  Vec2 operator + (const Vec2& b) const {
    return {
        a[0] + b[0],
        a[1] + b[1]
    };
  }
  
  Vec2 operator - (const Vec2& b) const {
    return {
        a[0] - b[0],
        a[1] - b[1]
    };
  }
  
  Vec2& operator += (const Vec2& b) {
    a[0] += b[0];
    a[1] += b[1];
    return *this;
  }
  
  Vec2& operator -= (const Vec2& b) {
    a[0] -= b[0];
    a[1] -= b[1];
    return *this;
  }
  
  bool operator == (const Vec2& b) const {
    return a[0] == b[0]
           && a[1] == b[1];
  }
  
  bool operator != (const Vec2& b) const {
    return a[0] != b[0]
           || a[1] != b[1];
  }
  
  Scalar dot(const Vec2& b) const {
    return a[0] * b[0]
           + a[1] * b[1];
  }
  
  
  //# scalar operators #
  
  Vec2 operator * (Scalar factor) const {
    return {
        a[0] * factor,
        a[1] * factor
    };
  }
  
  Vec2 operator / (Scalar factor) const {
    return {
        a[0] / factor,
        a[1] / factor
    };
  }
  
  Vec2& operator *= (Scalar factor) {
    a[0] *= factor;
    a[1] *= factor;
    return *this;
  }
  
  Vec2& operator /= (Scalar factor) {
    a[0] /= factor;
    a[1] /= factor;
    return *this;
  }
  
  
  //### FUNCTIONS ###
  
  Scalar calcLength() const {
    return std::sqrt(x*x + y*y);
  }
  
  // Returns a Vec2 with the same direction but a different length.
  Vec2 withLength(Scalar newLength) const {
    assert_(calcLength() != 0);
    Scalar growthFactor = newLength / calcLength();
    return (*this) * growthFactor;
  }
  
  Vec2 normalized() const {
    assert_(calcLength() != 0);
    return (*this) / calcLength();
  }
};



// Textify
template <typename Scalar>
inline std::ostream& operator << (std::ostream& stream, Vec2x<Scalar> v) {
  return stream << ("{x:"+$(v.x)+", y:"+$(v.y)+"}");
}


template<class Scalar>
Vec2x<Scalar> normalize(const Vec2x<Scalar>& vector) {
  assert_(vector.calcLength() != 0);
  return vector / vector.calcLength();
}


template<class Scalar>
Vec2x<Scalar> maybe_normalize(const Vec2x<Scalar>& vector) {
  if (vector.calcLength() == 0)
    return vector;
  return vector / vector.calcLength();
}


template<class Scalar>
Vec2x<Scalar> operator * (Scalar factor, const Vec2x<Scalar>& vector) {
  return vector * factor;
}



using Vec2 = Vec2x<double>;
using Vec2f = Vec2x<float>;
using Vec2i = Vec2x<int>;
using Vec2u = Vec2x<u32>;


static_assert(
    sizeof(Vec2) == 2 * sizeof(double),
    "The members of type Vec2 aren't being aligned correctly."
);


#endif //FRUIT_VEC2_HPP
