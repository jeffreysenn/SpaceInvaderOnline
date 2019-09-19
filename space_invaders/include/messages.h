// messages.h

#ifndef MESSAGES_H_INCLUDED
#define MESSAGES_H_INCLUDED

#include "input.h"

#include <gamma.h>

using namespace gamma;

namespace uu {
   enum message_type_id {
      MESSAGE_UNKNOWN,
      MESSAGE_CONNECTION_REQUEST,
      MESSAGE_CONNECTION_RESPONSE,
      MESSAGE_DISCONNECT,
      MESSAGE_INPUT,
      MESSAGE_COUNT,
   };

   static_assert(MESSAGE_COUNT < 255, "Message types cannot exceed 255");

   struct message_header {
      message_header();
      explicit message_header(message_type_id type);

      template <typename S>
      bool serialize(S &stream) {
         if (!stream.serialize(type_)) {
            return false;
         }
         return true;
      }

      uint8 type_;
   };

   struct message_connection_request : message_header {
      message_connection_request();
      explicit message_connection_request(uint32 random);

      template <typename S>
      bool serialize(S &stream) {
         if (!message_header::serialize(stream)) {
            return false;
         }
         if (!stream.serialize(random_)) {
            return false;
         }
         return true;
      }

	  bool is_valid() const
	  {
		  return type_ == MESSAGE_CONNECTION_REQUEST;
	  }

      uint32 random_;
   };

   struct message_connection_response : message_header {
      message_connection_response();

      template <typename S>
      bool serialize(S &stream) {
         if (!message_header::serialize<S>(stream)) {
            return false;
         }
         if (!stream.serialize(random_)) {
            return false;
         }
         return true;
      }

      uint32 random_;
   };

   struct message_disconnect : message_header {
      message_disconnect();

      template <typename S>
      bool serialize(S &stream) {
         if (!message_header::serialize<S>(stream)) {
            return false;
         }
         return true;
      }
   };

   struct message_input : message_header {
      message_input();
      explicit message_input(bool up, bool down, bool space);

      template <typename S>
      bool serialize(S &stream) {
         if (!message_header::serialize<S>(stream)) {
            return false;
         }
         if (!stream.serialize(input_)) {
            return false;
         }
         return true;
      }

	  bool is_valid() const
	  {
		  return type_ == MESSAGE_INPUT;
	  }

      bool has_up() const;
      bool has_down() const;
      bool has_space() const;

      uint8 input_;
   };

   constexpr size_t buffer_size = 12;
   struct message_input_buffer : message_header
   {
	   message_input_buffer();
	   explicit message_input_buffer(const std::vector<input>& input_buffer);

	   template <typename S>
	   bool serialize(S& serializer)
	   {
		   if (!message_header::serialize(serializer))
		   {
			   return false;
		   }
		   
		   for (int i = 0; i < buffer_size; ++i)
		   {
			   if(!(serializer.serialize(input_buffer_[i].input_) 
				  && serializer.serialize(input_buffer_[i].dt_)))
				  return false;
		   }

		   return true;
	   }

	   input input_buffer_[buffer_size];
	   size_t size_;
   };


#if 0
   /* USAGE EXAMPLE */
   void usage() {
      // note: we should write as many messages into a stream as possible for each send

      // note: example of writing messages to a stream
      {
         // note: we don't have to dynamically allocate anything
         //       just create an array on the stack and use it.
         //       MTU ethernet/wifi is ~1400 bytes before fragmentation
         uint8 array[1400] = {};
         byte_stream stream(sizeof(array), array);

         byte_stream_writer writer(stream);
         message_connection_request message(123);
         if (!message.serialize(writer)) {
            // note: faulure in writing message to stream
         }
         // note: otherwise just continue writing more messages to stream

         // note: when all messages are written to the stream send to remote
         if (!socket_.send_to(remote, stream)) {
            auto errcode = network::error::get_error();
            if (!network::error::is_non_critical(errcode)) {
               // note: handle error
            }
         }
         else {
            // note: all is well!
         }
      }

      // note: reading messages from a stream
      {
         // note: we don't have to dynamically allocate anything
         //       just create an array on the stack and use it.
         //       MTU ethernet/wifi is ~1400 bytes before fragmentation
         uint8 array[1400] = {};
         byte_stream stream(sizeof(array), array);

         ip_address remote;
         if (socket_.recv_from(remote, stream)) {
            if (remote == other_computer_ip) {
               byte_stream_reader reader(stream);
               message_connection_request message;
               if (!message.serialize(reader)) {
                  // note: failure reading message from stream
                  //       error, bail!
               }
               if (message.type_ != MESSSAGE_CONNECTION_REQUEST) {
                  // note: error! bail!
               }
               // note: otherwise just continue writing more messages to stream
            }
            else {
               // note: we got a udp packet from somewhere else, ignore it!
            }
         }
         else {
            auto errcode = network::error::get_error();
            // note: we got an error when we received
            //       check if it is serious or not
            if (!network::error::is_non_critical(errcode)) {
               // note: handle error
            }
         }
      }
   }


   void serialize_example()
   {
	   uint8 array[1400];
	   gamma::byte_stream stream(sizeof(array), array);
	   gamma::byte_stream_writer writer(steam);
	   uu::message_connection_request message;
	   if (!message.serialize(writer))
	   {

	   }

	   uu::message_input input_msg(true, false, false);
	   if (!input_msg.serialize(writer))
	   {

	   }

	   if (!socket_.send_to(remote_, stream))
	   {

	   }
   }

   void serialize_example2()
   {
	   uint8 array[1400];
	   gamma::byte_stream stream(sizeof(array), array);
	   gamma::byte_stream_reader reader(steam);

	   if (socket_.recv_from(remote, stream))
	   {
		   uu::message_connection_request req_msg;
		   if (!req_msg.serialize(reader))
		   {

		   }
		   if (!req_msg.is_valid())
		   {

		   }
		   uu::message_input input_msg;
		   if (!input_msg.serialize(reader))
		   {

		   }
	   }
   }

#endif
} // !uu

#endif // !MESSAGES_H_INCLUDED
