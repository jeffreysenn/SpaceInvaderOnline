// bullets.cc

#include "entity.h"

namespace uu {
   constexpr float bullet_speed = 300.0f;

   bullets::bullets() 
   {
   }

   void bullets::update(const time &dt) {
      for(int index = 0; index < _countof(entity_); index++) {
         entity &e = entity_[index];
         if (!e.visible_) {
            continue;
         }

         e.position_ = e.position_ + direction_[index] * bullet_speed * dt.as_seconds();
         e.sprite_.set_position(e.position_);
         e.collider_.set_position(e.position_);
         if (e.position_.x_ < -50.0f) {
            e.visible_ = false;
         }
         else if (e.position_.x_ > 1100.0f) {
            e.visible_ = false;
         }
      }
   }

   void bullets::render(render_system &rs) {
      for (auto &e : entity_) {
         if (!e.visible_) {
            continue;
         }

         e.sprite_.render(rs);
      }
   }

   void bullets::reset(sprite_sheet &sheet, texture &image) {
      rectangle source;
      sheet.get(BULLET, source);

      for (auto &e : entity_) {
         e.visible_ = false;
         
         e.sprite_.set_texture(image);
         e.sprite_.set_source(source);
         e.sprite_.set_size({ 24.0f, 4.0f });

         e.collider_.set_size({ 24.0f, 4.0f });
         e.collider_.set_center({ 12.0f, 2.0f });
      }
   }

   void bullets::spawn(const vector2 &position, const vector2 &direction) {
      for (int index = 0; index < _countof(entity_); index++) {
         entity &e = entity_[index];
         if (!e.visible_) {
            direction_[index] = direction;
            e.visible_ = true;
            e.position_ = position;
            e.sprite_.set_position(position);
            e.collider_.set_position(position);
            break;
         }
      }
   }
} // !uu
