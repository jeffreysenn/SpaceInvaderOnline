// entity.h

#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include <gamma.h>

using namespace gamma;

namespace uu {
   enum sprite_id {
      BULLET,
      EXPLOSION,
      LEFT_BASE_DMG0,
      LEFT_BASE_DMG1,
      LEFT_BASE_DMG2,
      LEFT_BASE_DMG3,
      LEFT_BASE_DMG4,
      LEFT_ENEMY_1,
      LEFT_ENEMY_2,
      LEFT_ENEMY_3,
      LEFT_PLAYER,
      RIGHT_BASE_DMG0,
      RIGHT_BASE_DMG1,
      RIGHT_BASE_DMG2,
      RIGHT_BASE_DMG3,
      RIGHT_BASE_DMG4,
      RIGHT_ENEMY_1,
      RIGHT_ENEMY_2,
      RIGHT_ENEMY_3,
      RIGHT_PLAYER,
   };

   struct sprite_sheet {
      sprite_sheet(texture &image);

      void add(const uint32 index, const rectangle &source);
      bool get(uint32 index, rectangle &source);

      texture &image_;
      hashmap<uint32, rectangle> sprites_;
   };

   struct entity {
      bool visible_;
      vector2 position_;
      sprite sprite_;
      collider collider_;
   };

   struct invaders {
      invaders(const vector2 &offset, const vector2 &direction);

      void update(const time &dt);
      void render(render_system &rs);

      bool are_all_dead() const;
      void reset(sprite_sheet &sheet, texture &image, bool left);
      void calculate_area();
      void remove_random();

      rectangle sources_[5];
      vector2 offset_;
      vector2 direction_;
      rectangle area_;
      int entity_count_;
      entity entity_[35];
   };

   struct bullets {
      bullets();

      void update(const time &dt);
      void render(render_system &rs);

      void reset(sprite_sheet &sheet, texture &image);
      void spawn(const vector2 &position, const vector2 &direction);
      
      vector2 direction_[32];
      entity entity_[32];
   };

   struct explosions {
      explosions();

      void update(const time &dt);
      void render(render_system &rs);

      void reset(sprite_sheet &sheet, texture &image);
      void spawn(const vector2 &position);

      time lifetime_[32];
      entity entity_[32];
   };

   struct spaceship {
      spaceship(const vector2 &position, const vector2 &offset);

      void update(const time &dt);
      void render(render_system &rs);

      void reset(sprite_sheet &sheet, texture &image, bool left);

      vector2 direction_;
      vector2 origin_;
      vector2 offset_;
      entity entity_;
   };

   struct blocks {
      blocks(const vector2 &size);

      void update(sprite_sheet &sheet);
      void render(render_system &rs);

      void reset(sprite_sheet &sheet, texture &image, const int sprite_begin);

      const vector2 size_;
      rectangle source_[5];
      int sprite_begin_;
      int health_[3];
      entity entity_[3];
   };
} // !uu

#endif // !ENTITY_H_INCLUDED
