#ifndef FRUIT_VEC3_HPP
#define FRUIT_VEC3_HPP


#include <util.hpp>
#include <cmath>


// A three-dimensional vector
template <typename Scalar_ = double>
struct Vec3x {
private:
  using Vec3 = Vec3x<Scalar_>;
public:
  using Scalar = Scalar_; // make Scalar public (template params are private)
  
  
  //#### DATA ####
  
  // You can refer to the members either as a[1], a[2] etc or a.y, a.z etc
  union {
    Scalar a[3];
    struct { Scalar x, y, z; };
  };
  
  
  //### CONSTRUCTORS ###
  
  Vec3x() : x(0), y(0), z(0) {}
  Vec3x(Scalar x, Scalar y, Scalar z) : x(x), y(y), z(z) {}
  Vec3x(const Vec3&) = default;
  
  
  //### OPERATORS ###
  
  // Vec[i] returns the i-th dimension of this vector
  Scalar& operator [] (int i) { return a[i]; }
  Scalar operator [] (int i) const { return a[i]; }
  
  
  //# vector operators #
  
  Vec3 operator + (const Vec3& b) const {
    return {
        a[0] + b[0],
        a[1] + b[1],
        a[2] + b[2]
    };
  }
  
  Vec3 operator - (const Vec3& b) const {
    return {
        a[0] - b[0],
        a[1] - b[1],
        a[2] - b[2]
    };
  }
  
  Vec3& operator += (const Vec3& b) {
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
    return *this;
  }
  
  Vec3& operator -= (const Vec3& b) {
    a[0] -= b[0];
    a[1] -= b[1];
    a[2] -= b[2];
    return *this;
  }
  
  bool operator == (const Vec3& b) const {
    return a[0] == b[0]
           && a[1] == b[1]
           && a[2] == b[2];
  }
  
  bool operator != (const Vec3& b) const {
    return a[0] != b[0]
           || a[1] != b[1]
           || a[2] != b[2];
  }
  
  Scalar dot(const Vec3& b) const {
    return a[0] * b[0]
           + a[1] * b[1]
           + a[2] * b[2];
  }
  
  
  //# scalar operators #
  
  Vec3 operator * (Scalar factor) const {
    return {
        a[0] * factor,
        a[1] * factor,
        a[2] * factor
    };
  }
  
  Vec3 operator / (Scalar factor) const {
    return {
        a[0] / factor,
        a[1] / factor,
        a[2] / factor
    };
  }
  
  Vec3& operator *= (Scalar factor) {
    a[0] *= factor;
    a[1] *= factor;
    a[2] *= factor;
    return *this;
  }
  
  Vec3& operator /= (Scalar factor) {
    a[0] /= factor;
    a[1] /= factor;
    a[2] /= factor;
    return *this;
  }
  
  
  //### FUNCTIONS ###
  
  Scalar calcLength() const {
    return std::sqrt(x*x + y*y + z*z);
  }
  
  // Returns a Vec3 with the same direction but a different length.
  Vec3 withLength(Scalar newLength) const {
    Scalar growthFactor = newLength / calcLength();
    return (*this) * growthFactor;
  }
  
  void normalize() {
    (*this) /= calcLength();
  }
  
  bool isNormalized() {
    static_assert(!Limits<Scalar>::is_exact,
                  "Vec3::isNormalized needs to be reimplemented for integer types.");
    Scalar length = calcLength();
    return 0.999 < length && length < 1.001;
  }
  
  // The cross product.
  Vec3 cross(const Vec3& b) {
    return {
        y*b.z - z*b.y,
        z*b.x - x*b.z,
        x*b.y - y*b.x
    };
  }
  
  
  // Textify
  std::ostream& operator << (std::ostream& stream) const {
    return stream << ("{x:"+$(x)+", y:"+$(y)+", z:"+$(z)+"}");
  }
};

// Textify
template <typename Scalar>
inline std::ostream& operator << (std::ostream& stream, Vec3x<Scalar> v) {
  return stream << ("{x:"+$(v.x)+", y:"+$(v.y)+", z:"+$(v.z)+"}");
}

template<class Scalar>
Vec3x<Scalar> normalize(const Vec3x<Scalar>& vector) {
  return vector / vector.calcLength();
}

template<class Scalar>
Vec3x<Scalar> operator * (Scalar factor, const Vec3x<Scalar>& vector) {
  return vector * factor;
}



using Vec3 = Vec3x<double>;
using Vec3f = Vec3x<float>;
using Vec3i = Vec3x<int>;
using Vec3u = Vec3x<u32>;


static_assert(
    sizeof(Vec3) == 3 * sizeof(double),
    "The members of type Vec3 aren't being aligned correctly."
);


#endif //FRUIT_VEC3_HPP
