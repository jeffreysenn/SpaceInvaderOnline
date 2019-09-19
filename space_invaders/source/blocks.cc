// blocks.cc

#include "entity.h"

namespace uu {
   constexpr int block_max_health = 5;

   blocks::blocks(const vector2 &size)
      : size_(size)
   {
      for (auto &health : health_) {
         health = block_max_health;
      }
   }

   void blocks::update(sprite_sheet &sheet) {
      for (int index = 0; index < _countof(entity_); index++) {
         entity &e = entity_[index];

         e.sprite_.set_source(source_[block_max_health - health_[index]]);
         e.sprite_.set_position(e.position_);
         e.collider_.set_position(e.position_);
      }
   }

   void blocks::render(render_system &rs) {
      for (auto &e : entity_) {
         if (!e.visible_) {
            continue;
         }

         e.sprite_.render(rs);
      }
   }

   void blocks::reset(sprite_sheet &sheet, texture &image, const int sprite_begin) {
      for (int index = 0; index < 5; index++) {
         sheet.get(sprite_begin + index, source_[index]);
      }

      for (auto &health : health_) {
         health = block_max_health;
      }

      const float x = sprite_begin == LEFT_BASE_DMG0 ? 60.0f : size_.x_ - 148.0f;
      const float y = 10.0f;
      const float y_step = (size_.y_ - 64.0f - y * 2.0f) / 3.0f;
      const float y_begin = y_step - 64.0f;
      for (int index = 0; index < 3; index++) {
         entity &e = entity_[index];

         e.visible_ = true;
         e.position_ = { x, y_begin + y_step * index };

         e.sprite_.set_texture(image);
         e.sprite_.set_source(source_[block_max_health - health_[index]]);
         e.sprite_.set_size({ 64.0f, 88.0f });
         e.sprite_.set_position(e.position_);

         e.collider_.set_size({ 64.0f, 88.0f });
         e.collider_.set_center({ 32.0f, 44.0f });
         e.collider_.set_position(e.position_);
      }
   }
} // !uu
