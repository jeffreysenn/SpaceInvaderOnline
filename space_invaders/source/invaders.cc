// invaders.cc

#include "entity.h"

namespace uu {
   constexpr float invader_scale = 4.0f;
   constexpr float invader_width = 8.0f * invader_scale;
   constexpr float invader_height = 12.0f * invader_scale;
   constexpr float invader_spacing = 8.0f;
   constexpr float invader_speed = 20.0f;

   invaders::invaders(const vector2 &offset, const vector2 &direction)
      : offset_(offset)
      , direction_(direction)
   {
   }

   void invaders::update(const time &dt) {
      if (entity_count_ == 0) {
         return;
      }

      vector2 move(0.0f, invader_speed * direction_.y_ * dt.as_seconds());
      for (auto &e : entity_) {
         e.position_ = e.position_ + move;
         e.sprite_.set_position(e.position_);
         e.collider_.set_position(e.position_);
      }

      calculate_area();
   }

   void invaders::render(render_system &rs) {
      if (entity_count_ == 0) {
         return;
      }

      //rs.draw(0xffff00ff, area_);
      for (auto &e : entity_) {
         if (!e.visible_) {
            continue;
         }

         e.sprite_.render(rs);
      }
   }

   bool invaders::are_all_dead() const {
      return entity_count_ == 0;
   }

   void invaders::reset(sprite_sheet &sheet, texture &image, bool left) {
      entity_count_ = _countof(entity_);

      const int id = left ? 0 : 1;
      const int sprites[2][5] =
      {
         { LEFT_ENEMY_3, LEFT_ENEMY_3, LEFT_ENEMY_2, LEFT_ENEMY_2, LEFT_ENEMY_1 },
         { RIGHT_ENEMY_1, RIGHT_ENEMY_2, RIGHT_ENEMY_2, RIGHT_ENEMY_3, RIGHT_ENEMY_3 },
      };

      const int col_count = 5;
      const int row_count = 7;
      int index = 0;
      for (auto &e : entity_) {
         const int row = (index % row_count);
         const float y = row * invader_height;
         const float y_offset = invader_spacing * row;

         const int col = (index / row_count);
         const float x = col * invader_width;
         const float x_offset = invader_spacing * col;

         e.position_ = vector2(x + x_offset, y + y_offset);
         e.position_ = e.position_ + offset_;

         rectangle source;
         sheet.get(sprites[id][col], source);
         e.sprite_.set_texture(image);
         e.sprite_.set_source(source);
         e.sprite_.set_size({ invader_width, invader_height });
         e.sprite_.set_position(e.position_);
         
         e.collider_.set_size({ invader_width, invader_height });
         e.collider_.set_center(e.position_ + vector2(invader_width * 0.5f, invader_height * 0.5f));
         e.collider_.set_position(e.position_);

         index++;
      }

      calculate_area();
   }

   void invaders::calculate_area() {
      vector2 min(9999.0f, 9999.0f);
      vector2 max;
      for (auto &e : entity_) {
         if (!e.visible_) { 
            continue; 
         }

         if (min.x_ > e.collider_.min().x_) {
            min.x_ = e.collider_.min().x_;
         }
         if (min.y_ > e.collider_.min().y_) {
            min.y_ = e.collider_.min().y_;
         }

         if (max.x_ < e.collider_.max().x_) {
            max.x_ = e.collider_.max().x_;
         }
         if (max.y_ < e.collider_.max().y_) {
            max.y_ = e.collider_.max().y_;
         }
      }

      area_.x_ = min.x_;
      area_.y_ = min.y_;
      area_.width_ = max.x_ - min.x_;
      area_.height_ = max.y_ - min.y_;

      if (area_.y_ < 10.0f && direction_.y_ < 0.0f) {
         direction_.y_ = -direction_.y_;
      }
      else if ((area_.y_ + area_.height_) > 502.0f && direction_.y_ > 0.0f) {
         direction_.y_ = -direction_.y_;
      }
   }

   void invaders::remove_random() {
#if 0
      int index = (int)random::range(0, _countof(entity_));
      entity_[index].visible_ = false;
#else
      for (int counter = 0; counter < _countof(entity_); counter++) {
         int index = (int)random::range(0, _countof(entity_));
         if (entity_[index].visible_) {
            entity_[index].visible_ = false;
            entity_count_--;
            break;
         }
      }
#endif
   }
} // !uu
