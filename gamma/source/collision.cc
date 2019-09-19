// collision.cc

#include "gamma.h"

namespace gamma {
   // static
   bool collider::overlap(const collider &lhs, const collider &rhs) {
      const vector2 cc = lhs.center_ - rhs.center_;
      const vector2 hh = lhs.extend_ + rhs.extend_;

      const float px = hh.x_ - fabsf(cc.x_);
      if (px < 0.0f) {
         return false;
      }

      const float py = hh.y_ - fabsf(cc.y_);
      if (py < 0.0f) {
         return false;
      }

      return true;
   }

   collider::collider()
   {
   }

   collider::collider(const vector2 center, const vector2 extend)
      : center_(center)
      , extend_(extend)
   {
   }

   void collider::set_size(const vector2 size) {
      extend_ = size * 0.5f;
   }

   void collider::set_center(const vector2 center) {
      center_ = center;
   }

   void collider::set_position(const vector2 position) {
      center_ = position + extend_;
   }

   vector2 collider::min() const {
      return center_ - extend_;
   }

   vector2 collider::max() const {
      return center_ + extend_;
   }

   float collider::width() const {
      return extend_.x_ * 2.0f;
   }

   float collider::height() const {
      return extend_.y_ * 2.0f;
   }
} // !gamma
