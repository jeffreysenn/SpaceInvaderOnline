// explosions.cc

#include "entity.h"

namespace uu {
   constexpr int64 explosion_duration_ms = 250;

   explosions::explosions()
   {
   }

   void explosions::update(const time &dt) {
      for (int index = 0; index < _countof(entity_); index++) {
         entity &e = entity_[index];
         if (!e.visible_) {
            continue;
         }

         lifetime_[index] = lifetime_[index] - dt;
         if (lifetime_[index].as_seconds() < 0.0f) {
            e.visible_ = false;
         }
      }
   }

   void explosions::render(render_system &rs) {
      for (auto &e : entity_) {
         if (!e.visible_) {
            continue;
         }

         e.sprite_.render(rs);
      }
   }

   void explosions::reset(sprite_sheet &sheet, texture &image) {
      for (auto &t : lifetime_) {
         t = time(0);
      }

      rectangle source;
      sheet.get(EXPLOSION, source);
      for (auto &e : entity_) {
         e.visible_ = false;
         e.position_ = {};
         
         e.sprite_.set_texture(image);
         e.sprite_.set_source(source);
         e.sprite_.set_size({ 32.0f, 52.0f });

         //e.collider_.set_size({ 32.0f, 52.0f });
         //e.collider_.set_center({ 16.0f, 26.0f});
      }
   }

   void explosions::spawn(const vector2 &position) {
      for (int index = 0; index < _countof(entity_); index++) {
         entity &e = entity_[index];
         if (!e.visible_) {
            lifetime_[index] = time(explosion_duration_ms);
            e.visible_ = true;
            e.position_ = position;
            e.sprite_.set_position(position);
            e.collider_.set_position(position);
            break;
         }
      }
   }
} // !uu
