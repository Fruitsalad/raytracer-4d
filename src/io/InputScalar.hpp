#pragma once


class InputScalar {
public:
  double value; // This is just a placeholder.
  // Hopefully someday I will make this thing work better.
  // Note from the future: I never did.
  
  
  double get() const {
    return value;
  }
  
  operator double() const { return get(); }
};
