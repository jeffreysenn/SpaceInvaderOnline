// messages.cc

#include "messages.h"

namespace uu {
   message_header::message_header()
      : type_((uint8)MESSAGE_UNKNOWN)
   {
   }

   message_header::message_header(message_type_id type)
      : type_((uint8)type)
   {
   }

   message_connection_request::message_connection_request()
      : message_header(MESSAGE_CONNECTION_REQUEST)
      , random_(0)
   {
   }

   message_connection_request::message_connection_request(uint32 random)
      : message_header(MESSAGE_CONNECTION_REQUEST)
      , random_(random)
   {
   }

   message_connection_response::message_connection_response()
      : message_header(MESSAGE_CONNECTION_RESPONSE)
	   , random_(0)
   {
   }

   message_disconnect::message_disconnect()
      : message_header(MESSAGE_DISCONNECT)
   {
   }

   message_input::message_input()
      : message_header(MESSAGE_INPUT)
      , input_(0)
   {
   }

   message_input::message_input(bool up, bool down, bool space)
      : message_header(MESSAGE_INPUT)
      , input_(0)
   {
      input_ |= up    ? (1 << 0) : 0;
      input_ |= down  ? (1 << 1) : 0;
      input_ |= space ? (1 << 2) : 0;
   }

   bool message_input::has_up() const {
      return (input_ & (1 << 0)) > 0;
   }

   bool message_input::has_down() const {
      return (input_ & (1 << 1)) > 0;
   }

   bool message_input::has_space() const {
      return (input_ & (1 << 2)) > 0;
   }
   message_input_buffer::message_input_buffer()
	   : input_buffer_{}
	   , size_(buffer_size)
   {
   }
   message_input_buffer::message_input_buffer(const std::vector<input>& input_buffer)
	   : input_buffer_{}
	   , size_(buffer_size)
   {
	   auto it = buffer_size < input_buffer.size() ? 
		   input_buffer.end() - buffer_size
		   : input_buffer.begin();
	   for (it; it != input_buffer.end(); ++it)
	   {
		   int64_t index = it - input_buffer.begin();
		   input_buffer_[index] = *it;
	   }
   }
} // !uu
