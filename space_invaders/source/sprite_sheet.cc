// sprite_sheet.cc

#include "entity.h"

namespace uu {
   sprite_sheet::sprite_sheet(texture &image)
      : image_(image)
   {
   }

   void sprite_sheet::add(const uint32 index, const rectangle &source) {
      sprites_.insert(std::pair<uint32, rectangle>(index, source));
   }

   bool sprite_sheet::get(const uint32 index, rectangle &source) {
      if (sprites_.find(index) == sprites_.end()) {
         return false;
      }

      source = sprites_[index];
      return true;
   }
} // !uu
