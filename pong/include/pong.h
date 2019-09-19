// pong.h

#ifndef PONG_H_INCLUDED
#define PONG_H_INCLUDED

#include <gamma.h>

using namespace gamma;

namespace uu {
   struct paddle  {
      paddle(const vector2 origin, const vector2 size);

      void update(float dt);
      void render(render_system &rs);
      void reset();

      vector2 origin_;
      vector2 position_;
      vector2 direction_;
      float speed_;
      sprite sprite_;
      collider collider_;
   };

   struct ball {
      ball(const vector2 origin, const vector2 size);

      void update(float dt);
      void render(render_system &rs);
      void reset();

      void bounce_x();
      void bounce_y();

      vector2 origin_;
      vector2 position_;
      vector2 direction_;
      float speed_;
      sprite sprite_;
      collider collider_;
   };

   struct wall {
      wall(const vector2 size, const vector2 position);

      void render(render_system &rs);

      sprite sprite_;
      collider collider_;
   };

   struct score {
      score(const vector2 pos);

      void render(render_system &rs);
      void reset();
      void increment();

      vector2 position_;
      int value_;
   };

   struct pong : game_base {
      enum state {
         GAME_STATE_INIT,
         GAME_STATE_PLAY,
      };

      pong(float width, float height);
      ~pong();

      void collision();
      bool enter() { return true; }
      void exit() {}
      bool update(const time &dt, const keyboard &kb);
      void render(render_system &rs);

      state state_;
      udp_socket socket_;
      vector2 screen_;
      bool server_;
      ball ball_;
      paddle local_;
      paddle remote_;
      score local_score_;
      score remote_score_;
      wall east_;
      wall west_;
      wall north_;
      wall south_;

      texture sprites_;
   };
} // !uu

#endif // PONG_H_INCLUDED
