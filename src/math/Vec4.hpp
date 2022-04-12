#ifndef FRUIT_VEC4_HPP
#define FRUIT_VEC4_HPP


#include <util.hpp>
#include <cmath>


// A four-dimensional vector
template <typename Scalar_ = double>
struct Vec4x {
private:
  using Vec4 = Vec4x<Scalar_>;
public:
  using Scalar = Scalar_; // make Scalar public (template params are private)
  
  
  //#### DATA ####
  
  // You can refer to the members either as a[1], a[2] etc or a.y, a.z etc
  union {
    Scalar a[4];
    struct { Scalar x, y, z, w; };
  };
  
  
  //### CONSTRUCTORS ###
  
  Vec4x() : x(0), y(0), z(0), w(0) {}
  Vec4x(Scalar x, Scalar y, Scalar z, Scalar w) : x(x), y(y), z(z), w(w) {}
  Vec4x(const Vec4&) = default;
  
  
  //### OPERATORS ###
  
  // Vec[i] returns the i-th dimension of this vector
  Scalar& operator [] (int i) { return a[i]; }
  Scalar operator [] (int i) const { return a[i]; }
  
  
  //# vector operators #
  
  Vec4 operator + (const Vec4& b) const {
    return {
        a[0] + b[0],
        a[1] + b[1],
        a[2] + b[2],
        a[3] + b[3]
    };
  }
  
  Vec4 operator - (const Vec4& b) const {
    return {
        a[0] - b[0],
        a[1] - b[1],
        a[2] - b[2],
        a[3] - b[3]
    };
  }
  
  Vec4& operator += (const Vec4& b) {
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
    a[3] += b[3];
    return *this;
  }
  
  Vec4& operator -= (const Vec4& b) {
    a[0] -= b[0];
    a[1] -= b[1];
    a[2] -= b[2];
    a[3] -= b[3];
    return *this;
  }
  
  bool operator == (const Vec4& b) const {
    return a[0] == b[0]
           && a[1] == b[1]
           && a[2] == b[2]
           && a[3] == b[3];
  }
  
  bool operator != (const Vec4& b) const {
    return a[0] != b[0]
           || a[1] != b[1]
           || a[2] != b[2]
           || a[3] != b[3];
  }
  
  Scalar dot(const Vec4& b) const {
    return a[0] * b[0]
           + a[1] * b[1]
           + a[2] * b[2]
           + a[3] * b[3];
  }
  
  Vec4 elemMult(const Vec4& b) const {
    return {
        a[0] * b[0],
        a[1] * b[1],
        a[2] * b[2],
        a[3] * b[3]
    };
  }
  
  
  //# scalar operators #
  
  Vec4 operator * (Scalar factor) const {
    return {
        a[0] * factor,
        a[1] * factor,
        a[2] * factor,
        a[3] * factor
    };
  }
  
  Vec4 operator / (Scalar factor) const {
    return {
        a[0] / factor,
        a[1] / factor,
        a[2] / factor,
        a[3] / factor
    };
  }
  
  Vec4& operator *= (Scalar factor) {
    a[0] *= factor;
    a[1] *= factor;
    a[2] *= factor;
    a[3] *= factor;
    return *this;
  }
  
  Vec4& operator /= (Scalar factor) {
    a[0] /= factor;
    a[1] /= factor;
    a[2] /= factor;
    a[3] /= factor;
    return *this;
  }
  
  
  //### FUNCTIONS ###
  
  Scalar calcLength() const {
    return std::sqrt(x*x + y*y + z*z + w*w);
  }
  
  // Returns a Vec4 with the same direction but a different length.
  Vec4 withLength(Scalar newLength) const {
    Scalar growthFactor = newLength / calcLength();
    return (*this) * growthFactor;
  }
  
  void normalize() {
    (*this) /= calcLength();
  }
  
  
  // Textify
  std::ostream& operator << (std::ostream& stream) const {
    return stream << ("{x:"+$(x)+", y:"+$(y)+", z:"+$(z)+", w:"+$(w)+"}");
  }
};




// Textify
template <typename Scalar>
inline std::ostream& operator << (std::ostream& stream, Vec4x<Scalar> v) {
  return stream << ("{x:"+$(v.x)+", y:"+$(v.y)+", z:"+$(v.z)+", w:"+$(v.w)+"}");
}


template<class Scalar>
Vec4x<Scalar> normalize(const Vec4x<Scalar>& vector) {
  return vector / vector.calcLength();
}


template<class Scalar>
Vec4x<Scalar> maybe_normalize(const Vec4x<Scalar>& vector) {
  if (vector.calcLength() == 0)
    return vector;
  return vector / vector.calcLength();
}


template<class Scalar>
bool isNormalized(Vec4x<Scalar> vector) {
  static_assert(!Limits<Scalar>::is_exact,
                "isNormalized needs to be reimplemented for integer types.");
  Scalar length = vector.calcLength();
  return 0.999 < length && length < 1.001;
}


template<class Scalar>
Vec4x<Scalar> operator * (Scalar factor, const Vec4x<Scalar>& vector) {
  return vector * factor;
}




using Vec4 = Vec4x<double>;
using Vec4f = Vec4x<float>;
using Vec4i = Vec4x<int>;
using Vec4u = Vec4x<u32>;


static_assert(
    sizeof(Vec4) == 4 * sizeof(double),
    "The members of type Vec4 aren't being aligned correctly."
);




#ifdef ENABLE_DOCTEST
#include <doctest/doctest.h>
TEST_CASE("Vec4 element access") {
  Vec4 v = {1, 2, 3, 4};
  
  CHECK(v.x == 1);
  CHECK(v.y == 2);
  CHECK(v.z == 3);
  CHECK(v.w == 4);
  
  CHECK(v[0] == v.x);
  CHECK(v[1] == v.y);
  CHECK(v[2] == v.z);
  CHECK(v[3] == v.w);
}

TEST_CASE("Vec4 normalization") {
  Vec4 v = {1, 2, 3, 4};
  Vec4 v2 = {0, 20, 35, 0};
  
  CHECK(isNormalized(normalize(v)));
  CHECK(isNormalized(normalize(v2)));
}
#endif //ENABLE_DOCTEST


#endif //FRUIT_VEC4_HPP
