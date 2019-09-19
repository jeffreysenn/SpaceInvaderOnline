// spaceship.cc

#include "entity.h"

namespace uu {
   constexpr float ship_speed = 150.0f;

   spaceship::spaceship(const vector2& position, const vector2 &offset)
      : origin_(position)
      , offset_(offset)
   {
   }

   void spaceship::update(const time &dt) {
      entity_.position_ = entity_.position_ + direction_ * ship_speed * dt.as_seconds();
      if (entity_.position_.y_ < 10.0f) {
         entity_.position_.y_ = 10.0f;
      }
      else if (entity_.position_.y_ > 450.0f) {
         entity_.position_.y_ = 450.0f;
      }

      entity_.sprite_.set_position(entity_.position_);
      entity_.collider_.set_position(entity_.position_);
   }

   void spaceship::render(render_system &rs) {
      entity_.sprite_.render(rs);
   }

   void spaceship::reset(sprite_sheet &sheet, texture &image, bool left) {
      entity_.visible_ = true;
      entity_.position_ = origin_;

      rectangle source;
      sheet.get(left ? LEFT_PLAYER : RIGHT_PLAYER, source);
      entity_.sprite_.set_texture(image);
      entity_.sprite_.set_source(source);
      entity_.sprite_.set_size({ 32.0f, 52.0f });
      entity_.sprite_.set_position(origin_);

      entity_.collider_.set_size({ 32.0f, 52.0f });
      entity_.collider_.set_center({ 16.0f, 26.0f });
      entity_.collider_.set_position(origin_);
   }
} // !uu
