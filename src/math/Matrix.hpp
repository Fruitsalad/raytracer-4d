#ifndef FRUIT_MATRIX_HPP
#define FRUIT_MATRIX_HPP

#include <util.hpp>
#include <iomanip>
#include <array>


/** Matrix<h,w> represents a two-dimensional matrix.
 * Following matrix conventions, this class uses (y,x) instead of (x,y) */
template<uint _height, uint _width, typename Scalar_ = double>
class Matrix {
private:
  using S = Scalar_;
  static constexpr uint w = _width;
  static constexpr uint h = _height;
public:
  // make the template parameters publicly accessible:
  using Scalar = Scalar_;
  static constexpr uint width = _width;
  static constexpr uint height = _height;
  
  
  std::array<Scalar, width*height> data;
  
  
  
  //### OPERATORS ###
  
  Scalar& operator () (uint y, uint x) {
    return get(y, x);
  }
  
  Scalar operator () (uint y, uint x) const {
    return get(y, x);
  }
  
  Scalar& get(uint y, uint x) {
    return data[y*width + x];
  }
  
  Scalar get(uint y, uint x) const {
    return data[y*width + x];
  }
  
  
  //# logical operators #
  
  bool operator == (const Matrix<h,w,S>& other) const {
    return data == other.data;
  }
  
  bool operator != (const Matrix<h,w,S>& other) const {
    return data != other.data;
  }
  
  
  //# matrix operators #
  
  Matrix<h,w,S>& operator += (const Matrix<h,w,S>& other) {
    for (uint i = 0; i < data.size(); i++)
      data[i] += other.data[i];
    return *this;
  }
  
  
  Matrix<h,w,S>& operator -= (const Matrix<h,w,S>& other) {
    for (uint i = 0; i < data.size(); i++)
      data[i] -= other.data[i];
    return *this;
  }
  
  
  Matrix<h,w,S> operator + (const Matrix<h,w,S>& other) const {
    Matrix<h,w,S> result;
    
    for (uint i = 0; i < data.size(); i++)
      result[i] = data[i] + other.data[i];
    
    return result;
  }
  
  
  Matrix<h,w,S> operator - (const Matrix<h,w,S>& other) const {
    Matrix<h,w,S> result;
    
    for (uint i = 0; i < data.size(); i++)
      result[i] = data[i] - other.data[i];
    
    return result;
  }
  
  
  template<uint h2, uint w2>
  Matrix<h,w2,S> operator * (const Matrix<h2,w2,S>& other) {
    static_assert(w == h2,
        "These matrices cannot be multiplied. "
        "The width of the first matrix must be the same as the height of the "
        "second matrix. Currently these sizes don't match.");
    
    Matrix<h,w2,S> result;
    
    // For each row of the first operand...
    for (uint y = 0; y < h; y++) {
      
      // For each column of the second operand...
      for (uint x2 = 0; x2 < w2; x2++) {
        
        // The usual matrix multiplication stuff
        Scalar& target = result(y, x2);
        target = 0;
        
        for (int i = 0; i < w; i++)
          target += get(y, i) * other(i, x2);
      }
    }
    
    return result;
  }
  
  
  //# scalar operators #
  
  Matrix<h,w,S> operator * (Scalar factor) const {
    Matrix<h,w,S> result;
    
    for (uint i = 0; i < data.size(); i++)
      result[i] = data[i] * factor;
    
    return result;
  }
  
  Matrix<h,w,S> operator / (Scalar factor) const {
    Matrix<h,w,S> result;
    
    for (uint i = 0; i < data.size(); i++)
      result[i] = data[i] / factor;
    
    return result;
  }
  
  Matrix<h,w,S>& operator *= (Scalar factor) const {
    for (uint i = 0; i < data.size(); i++)
      data[i] *= factor;
    return *this;
  }
  
  Matrix<h,w,S>& operator /= (Scalar factor) const {
    for (uint i = 0; i < data.size(); i++)
      data[i] /= factor;
    return *this;
  }
  
  
  //### FUNCTIONS ###
  
  Matrix<w,h> transposed() const {
    Matrix<w,h> result;
    
    for (uint y = 0; y < h; y++)
      for (uint x = 0; x < w; x++)
        result(x,y) = get(y,x);
    
    return result;
  }
};



// More operators
template<uint h, uint w, typename S>
Matrix<h,w,S> operator * (S scalar, Matrix<h,w,S> matrix) {
  return matrix * scalar;
}



// Shorthands for alternative types
template<uint w, uint h> using MatrixF = Matrix<w,h,float>;
template<uint w, uint h> using MatrixI = Matrix<w,h,int>;
template<uint w, uint h> using MatrixU = Matrix<w,h,uint>;




// Constant matrices

template<uint h, uint w, typename S>
Matrix<h,w,S> createZeroMatrix() {
  Matrix<h,w,S> result;
  result.data.fill(0);
  return result;
}

template<uint w, typename S>
Matrix<w,w,S> createUnitMatrix() {
  Matrix<w,w,S> result = createZeroMatrix<w,w,S>();
  for (uint i = 0; i < w; i++)
    result(i,i) = 1;
  return result;
}


template<uint h, uint w, typename S = double>
inline Matrix<h,w,S> zeroMatrix = createZeroMatrix<h,w,S>();

template<uint w, typename S = double>
inline Matrix<w,w,S> unitMatrix = createUnitMatrix<w,S>();



// Stringify (this is primarily for the debug output of unit tests)
template<uint h, uint w, typename S>
inline std::ostream& operator << (std::ostream& out, const Matrix<h,w,S>& m) {
  out << std::setprecision(4) << "[" << h << "x" << w << " matrix] [\n";
  
  for (uint y = 0; y < h; y++) {
    out << "    ";
    for (uint x = 0; x < w; x++)
      out << m(y, x) << " ";
    out << "\n";
  }
  
  return out << "]";
}



#ifdef ENABLE_DOCTEST
#include <doctest/doctest.h>
TEST_CASE("matrix multiplication") {
  Matrix<2,3> matrix_2x3 = {
      2, 5, 0,
      3, 1, -1
  };
  
  Matrix<3,1> vector_3 = {
      1,
      2,
      1
  };
  
  Matrix<2,1> expected = {12, 4};
  Matrix<2,1> not_expected = {0, 0};
  
  Matrix<2,1> result = matrix_2x3 * vector_3;
  
  CHECK(result == expected);
  CHECK(result != not_expected);
  
  
  // Test associativity
  
  Matrix<2,2> matrix_2x2 = {
      3, 1,
      0, -1
  };
  
  Matrix<2,1> expected_2 = {40, -4};
  
  Matrix<2,1> result_2 = (matrix_2x2 * matrix_2x3) * vector_3;
  Matrix<2,1> result_3 = matrix_2x2 * (matrix_2x3 * vector_3);
      
      CHECK(result_2 == expected_2);
      CHECK(result_3 == expected_2);
}


TEST_CASE("matrix transpose") {
  Matrix<2,3> matrix_2x3 = {
      2, 5, 0,
      3, 1, -1
  };
  
  Matrix<3,2> expected = {
      2, 3,
      5, 1,
      0, -1
  };
  
  CHECK(matrix_2x3.transposed() == expected);
}


TEST_CASE("zero matrix") {
  CHECK(zeroMatrix<2,3> == Matrix<2,3>{0,0,0, 0,0,0});
}


TEST_CASE("unit matrix") {
  CHECK(unitMatrix<3> == Matrix<3,3>{1,0,0, 0,1,0, 0,0,1});
}
#endif

#endif //FRUIT_MATRIX_HPP
