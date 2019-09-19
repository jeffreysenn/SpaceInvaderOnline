// entity.cc

#include "pong.h"

#pragma warning(disable: 4244) // conversion from 'float' to 'int', possible loss of data

namespace uu {
   paddle::paddle(const vector2 position, const vector2 size)
      : origin_(position)
      , position_(position)
      , direction_(0.0f, 0.0f)
      , speed_(150.0f)
   {
      sprite_.set_size(size);
      sprite_.set_position(position);
      collider_.set_size(size);
      collider_.set_position(position + size * 0.5f);
   }

   void paddle::update(float dt) {
      position_ = position_ + direction_ * speed_ * dt;
      collider_.set_position(position_);
      sprite_.set_position(position_);
   }

   void paddle::render(render_system &rs) {
      sprite_.render(rs);
   }

   void paddle::reset() {
      position_ = origin_;
   }

   ball::ball(const vector2 position, const vector2 size)
      : origin_(position)
      , position_(position)
      , direction_(0.0f, 0.0f)
      , speed_(200.0f)
   {
      sprite_.set_size(size);
      sprite_.set_position(position);
      collider_.set_size(size);
      collider_.set_position(position + size * 0.5f);
   }

   void ball::update(float dt) {
      position_ = position_ + direction_ * speed_ * dt;
      collider_.set_position(position_);
      sprite_.set_position(position_);
   }

   void ball::render(render_system &rs) {
      sprite_.render(rs);
   }

   void ball::reset() {
      position_ = origin_;
      direction_ = {};
   }

   void ball::bounce_x() {
      direction_.x_ = -direction_.x_;
   }

   void ball::bounce_y() {
      direction_.y_ = -direction_.y_;
   }

   score::score(const vector2 pos)
      : position_(pos)
      , value_(0)
   {
   }

   void score::render(render_system &rs) {
      rs.draw_text(position_.x_, position_.y_, 0xffffffff, 4, "%d", value_);
   }

   void score::reset() {
      value_ = 0;
   }

   void score::increment() {
      value_++;
   }

   wall::wall(const vector2 size, const vector2 position)
   {
      sprite_.set_size(size);
      sprite_.set_position(position);
      collider_.set_size(size);
      collider_.set_center(position + size * 0.5f);
   }

   void wall::render(render_system &rs) {
      sprite_.render(rs);
   }
} // !uu
