// pong.cc

#pragma warning(disable: 4244)

#include "pong.h"

namespace gamma {
   game_base *create_game(string &caption, video_mode &mode) {
      caption = "PONG";
      mode = video_mode(1280, 720);
      return new uu::pong(1280.0f, 720.0f);
   }
}

namespace uu {
   namespace {
      static long next = 1;
      void seed(long s) {
         next = s;
      }

      float random() {
         next = next * 1103515245 + 12345;
         return ((unsigned int)(next / 65536) % 32768) / 32768.0f;
      }

      float random_range(float min, float max) {
         return min + (max - min) * random();
      }
   } // !anon

   pong::pong(float width, float height)
      : state_(GAME_STATE_INIT)
      , screen_(width, height)
      , server_(true)
      , ball_({ (width - 10.0f) * 0.5f, (height - 10.0f) * 0.5f }, { 20.0f, 20.0f })
      , local_({ 40.0f, height * 0.5f - 40.0f }, { 20.0f, 80.0f })
      , remote_({ width - 60.0f, height * 0.5f - 40.0f }, { 20.0f, 80.0f })
      , local_score_({ width * 0.5f - 100.0f, 10.0f })
      , remote_score_({ width * 0.5f + 80.0f, 10.0f })
      , east_( { 10.0f, height }, { 0.0f, 0.0f } )
      , west_( { 10.0f, height }, { width - 10.0f, 0.0f } )
      , north_( { width, 10.0f }, { 0.0f, 0.0f } )
       , south_( { width, 10.0f }, { 0.0f, height - 10.0f } )
   {
      seed((long)time::now().tick_);
   }

   pong::~pong()
   {
   }

   void pong::collision() {
      if (collider::overlap(local_.collider_, north_.collider_)) {
         local_.position_.y_ = north_.collider_.max().y_;
         local_.update(0.0f);
      }
      if (collider::overlap(local_.collider_, south_.collider_)) {
         local_.position_.y_ = south_.collider_.min().y_;
         local_.position_.y_ -= local_.collider_.height();
         local_.update(0.0f);
      }

      if (collider::overlap(remote_.collider_, north_.collider_)) {
         remote_.position_.y_ = north_.collider_.max().y_;
         remote_.update(0.0f);
      }
      if (collider::overlap(remote_.collider_, south_.collider_)) {
         remote_.position_.y_ = south_.collider_.min().y_;
         remote_.position_.y_ -= remote_.collider_.height();
         remote_.update(0.0f);
      }

      if (collider::overlap(ball_.collider_, north_.collider_)) {
         ball_.bounce_y();
      }
      else if (collider::overlap(ball_.collider_, south_.collider_)) {
         ball_.bounce_y();
      }
      if (collider::overlap(ball_.collider_, east_.collider_)) {
         ball_.bounce_x();
         remote_score_.increment();
      }
      else if (collider::overlap(ball_.collider_, west_.collider_)) {
         ball_.bounce_x();
         local_score_.increment();
      }

      if (collider::overlap(ball_.collider_, local_.collider_)) {
         ball_.bounce_x();
      }
      if (collider::overlap(ball_.collider_, remote_.collider_)) {
         ball_.bounce_x();
      }
   }

   bool pong::update(const time &dt, const keyboard &kb) {
      if (kb.is_released(KEYCODE_ESCAPE)) {
         return false;
      }

      if (state_ == GAME_STATE_INIT) {
         state_ = GAME_STATE_PLAY;
      }
      else if (state_ == GAME_STATE_PLAY) {
         // note: local player
         local_.direction_ = {};
         if (kb.is_down(KEYCODE_W)) {
            local_.direction_.y_ -= 1.0f;
         }
         if (kb.is_down(KEYCODE_S)) {
            local_.direction_.y_ += 1.0f;
         }
         local_.update(dt.as_seconds());

         // note: remote player
         remote_.direction_ = {};
         if (kb.is_down(KEYCODE_O)) {
            remote_.direction_.y_ -= 1.0f;
         }
         if (kb.is_down(KEYCODE_L)) {
            remote_.direction_.y_ += 1.0f;
         }
         remote_.update(dt.as_seconds());

         // note: local/remote ball
         if (server_ && kb.is_released(KEYCODE_SPACE)) {
            server_ = false;
            ball_.direction_.x_ = random_range(-2.0f, 2.0f);
            ball_.direction_.y_ = random_range(-1.0f, 1.0f);
            ball_.direction_.normalize();
         }
         ball_.update(dt.as_seconds());

         collision();
      }

      return true;
   }

   void pong::render(render_system &rs) {
      rs.clear();
      ball_.render(rs);
      local_.render(rs);
      remote_.render(rs);
      local_score_.render(rs);
      remote_score_.render(rs);
   }
} // !uu
