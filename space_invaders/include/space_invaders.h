// space_invaders.h

#ifndef SPACE_INVADERS_H_INCLUDED
#define SPACE_INVADERS_H_INCLUDED

#include <gamma.h>

using namespace gamma;

#include "entity.h"
#include "messages.h"
#include "input.h"

#include <vector>

namespace uu {
   struct space_invaders : game_base {
      enum state {
         GAME_STATE_INIT,
         GAME_STATE_CONNECTING,
         GAME_STATE_PLAY,
      };

      space_invaders();      
      ~space_invaders();

      bool enter();
      void exit();
      bool update(const time &dt, const keyboard &kb);
      void render(render_system &rs);

	  bool send_connection_request();
	  bool send_connection_response();
	  bool receive_connection_request(gamma::byte_stream& stream);
	  bool receive_connection_response(gamma::byte_stream& stream);
	  bool send_input(uu::message_input& inputMessage);
	  bool receive_input(uu::message_input& inputMessage);
	  bool send_input_buffer(uu::message_input_buffer& input_buffer_message);
	  bool receive_input_buffer(uu::message_input_buffer& input_buffer_message);


      ip_address remote_;
      udp_socket socket_;

      texture sprites_;
      sprite_sheet sprite_sheet_;

      state state_;
      time firetimer_;
      bullets bullets_;
      explosions explosions_;
      invaders invaders_left_;
      invaders invaders_right_;
      spaceship ship_left_;
      spaceship ship_right_;
      blocks blocks_left_;
      blocks blocks_right_;

	  std::pair<bool, bool> connection_pair_;
	  bool is_host_;
	  time send_timer_;
	  std::vector<input> input_buffer_;
   };
} // !uu

#endif // !SPACE_INVADERS_H_INCLUDED
