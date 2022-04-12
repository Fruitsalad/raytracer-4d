#pragma once


class iScreen {
public:
  virtual void init() = 0;
  virtual void render() = 0;
  virtual void update() = 0;
};



