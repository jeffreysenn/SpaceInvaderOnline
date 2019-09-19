// vector2.cc

#include "gamma.h"

#include <math.h>

namespace gamma {
   vector2::vector2()
      : x_(0.0f)
      , y_(0.0f)
   {
   }

   vector2::vector2(float x, float y) 
      : x_(x)
      , y_(y)
   {
   }

   vector2 vector2::operator-(const vector2 &rhs) const {
      return vector2(x_ - rhs.x_, y_ - rhs.y_);
   }

   vector2 vector2::operator+(const vector2 &rhs) const {
      return vector2(x_ + rhs.x_, y_ + rhs.y_);
   }

   vector2 vector2::operator*(const vector2 &rhs) const {
      return vector2(x_ * rhs.x_, y_ * rhs.y_);
   }

   vector2 vector2::operator*(const float rhs) const {
      return vector2(x_ * rhs, y_ * rhs);
   }

   vector2 vector2::operator/(const float rhs) const {
      return vector2(x_ / rhs, y_ / rhs);
   }

   float vector2::dot(const vector2 &rhs) const {
      return x_ * rhs.x_ + y_ * rhs.y_;
   }

   float vector2::length() const {
      return sqrtf(dot(*this));
   }

   void vector2::normalize() {
      float len = length();
      if (len > 0.0f) {
         x_ /= len;
         y_ /= len;
      }
   }
} // !uu
