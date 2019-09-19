// gamma.h

#ifndef GAMMA_H_INCLUDED
#define GAMMA_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

namespace gamma {
   typedef unsigned long long uint64;
   typedef   signed long long int64;
   typedef unsigned int       uint32;
   typedef   signed int       int32;
   typedef unsigned short     uint16;
   typedef   signed short     int16;
   typedef unsigned char      uint8;
   typedef   signed char      int8;

   using string = std::string;

   template <typename T>
   using dynamic_array = std::vector<T>;

   template <typename K, typename V>
   using hashmap = std::unordered_map<K, V>;

   struct vector2 {
      vector2();
      vector2(float x, float y);

      vector2 operator-(const vector2 &rhs) const;
      vector2 operator+(const vector2 &rhs) const;
      vector2 operator*(const vector2 &rhs) const;
      vector2 operator*(const float rhs) const;
      vector2 operator/(const float rhs) const;

      float dot(const vector2 &rhs) const;
      float length() const;
      void normalize();

      float x_;
      float y_;
   };

   namespace random {
      float normalized();
      float range(float min, float max);
   } // !random

   struct rectangle {
      rectangle();
      rectangle(float x, float y, float width, float height);

      void set_size(const vector2 size);
      void set_position(const vector2 pos);

      float x_;
      float y_;
      float width_;
      float height_;
   };

   struct time {
      static time now();

      time();
      explicit time(int64 tick);

      time operator+(const time &rhs);
      time operator-(const time &rhs);
      bool operator<(const time &rhs);

      float as_seconds() const;
      float as_milliseconds() const;

      int64 tick_;
   };

   struct video_mode {
      static bool get_desktop(video_mode &mode);
      static bool get_supported(dynamic_array<video_mode> &modes);
      static void set_mode(const video_mode &mode);

      video_mode();
      explicit video_mode(int width, int height);

      int width_;
      int height_;
   };

   namespace system {
      bool message_box(const char *format, ...);
   } // !system

   enum keycode {
      KEYCODE_NONE = 0x00,        KEYCODE_BACK = 0x08,        KEYCODE_TAB = 0x09,         KEYCODE_CLEAR = 0x0C,
      KEYCODE_RETURN = 0x0D,      KEYCODE_SHIFT = 0x10,       KEYCODE_CONTROL = 0x11,     KEYCODE_MENU = 0x12,
      KEYCODE_PAUSE = 0x13,       KEYCODE_CAPITAL = 0x14,     KEYCODE_ESCAPE = 0x1B,      KEYCODE_SPACE = 0x20,
      KEYCODE_PAGEUP = 0x21,      KEYCODE_PAGEDOWN = 0x22,    KEYCODE_END = 0x23,         KEYCODE_HOME = 0x24,
      KEYCODE_LEFT = 0x25,        KEYCODE_UP = 0x26,          KEYCODE_RIGHT = 0x27,       KEYCODE_DOWN = 0x28,
      KEYCODE_KEYPADENTER = 0x2B, KEYCODE_PRINTSCREEN = 0x2C, KEYCODE_INSERT = 0x2D,      KEYCODE_DELETE = 0x2E,
      KEYCODE_0 = 0x30,           KEYCODE_1 = 0x31,           KEYCODE_2 = 0x32,           KEYCODE_3 = 0x33,
      KEYCODE_4 = 0x34,           KEYCODE_5 = 0x35,           KEYCODE_6 = 0x36,           KEYCODE_7 = 0x37,
      KEYCODE_8 = 0x38,           KEYCODE_9 = 0x39,           KEYCODE_A = 0x41,           KEYCODE_B = 0x42,
      KEYCODE_C = 0x43,           KEYCODE_D = 0x44,           KEYCODE_E = 0x45,           KEYCODE_F = 0x46,
      KEYCODE_G = 0x47,           KEYCODE_H = 0x48,           KEYCODE_I = 0x49,           KEYCODE_J = 0x4A,
      KEYCODE_K = 0x4B,           KEYCODE_L = 0x4C,           KEYCODE_M = 0x4D,           KEYCODE_N = 0x4E,
      KEYCODE_O = 0x4F,           KEYCODE_P = 0x50,           KEYCODE_Q = 0x51,           KEYCODE_R = 0x52,
      KEYCODE_S = 0x53,           KEYCODE_T = 0x54,           KEYCODE_U = 0x55,           KEYCODE_V = 0x56,
      KEYCODE_W = 0x57,           KEYCODE_X = 0x58,           KEYCODE_Y = 0x59,           KEYCODE_Z = 0x5A,
      KEYCODE_LWIN = 0x5B,        KEYCODE_RWIN = 0x5C,        KEYCODE_KEYPAD0 = 0x60,     KEYCODE_KEYPAD1 = 0x61,
      KEYCODE_KEYPAD2 = 0x62,     KEYCODE_KEYPAD3 = 0x63,     KEYCODE_KEYPAD4 = 0x64,     KEYCODE_KEYPAD5 = 0x65,
      KEYCODE_KEYPAD6 = 0x66,     KEYCODE_KEYPAD7 = 0x67,     KEYCODE_KEYPAD8 = 0x68,     KEYCODE_KEYPAD9 = 0x69,
      KEYCODE_MULTIPLY = 0x6A,    KEYCODE_ADD = 0x6B,         KEYCODE_SEPARATOR = 0x6C,   KEYCODE_SUBTRACT = 0x6D,
      KEYCODE_DECIMAL = 0x6E,     KEYCODE_DIVIDE = 0x6F,      KEYCODE_F1 = 0x70,          KEYCODE_F2 = 0x71,
      KEYCODE_F3 = 0x72,          KEYCODE_F4 = 0x73,          KEYCODE_F5 = 0x74,          KEYCODE_F6 = 0x75,
      KEYCODE_F7 = 0x76,          KEYCODE_F8 = 0x77,          KEYCODE_F9 = 0x78,          KEYCODE_F10 = 0x79,
      KEYCODE_F11 = 0x7A,         KEYCODE_F12 = 0x7B,         KEYCODE_NUMLOCK = 0x90,     KEYCODE_SCROLLOCK = 0x91,
      KEYCODE_LSHIFT = 0xA0,      KEYCODE_RSHIFT = 0xA1,      KEYCODE_LCONTROL = 0xA2,    KEYCODE_RCONTROL = 0xA3,
      KEYCODE_LMENU = 0xA4,       KEYCODE_RMENU = 0xA5,       KEYCODE_OEM_1 = 0xBA,       KEYCODE_OEM_PLUS = 0xBB,
      KEYCODE_OEM_COMMA = 0xBC,   KEYCODE_OEM_MINUS = 0xBD,   KEYCODE_OEM_PERIOD = 0xBE,  KEYCODE_OEM_2 = 0xBF, 
      KEYCODE_OEM_3 = 0xC0,       KEYCODE_OEM_4 = 0xDB,       KEYCODE_OEM_5 = 0xDC,       KEYCODE_OEM_6 = 0xDD, 
      KEYCODE_OEM_7 = 0xDE,       KEYCODE_OEM_8 = 0xDF,       KEYCODE_OEM_102 = 0xE2,     KEYCODE_COUNT = 256,
   };

   struct keyboard {
      keyboard();

      bool is_down(keycode index) const;
      bool is_pressed(keycode index) const;
      bool is_released(keycode index) const;

      struct keystate {
         bool down_;
         bool released_;
         bool pressed_;
      } keys_[KEYCODE_COUNT]{};
   };

   enum network_error_code {
      NETERR_NO_ERROR,
      NETERR_INVALID_HANDLE,
      NETERR_NOT_ENOUGH_MEMORY,
      NETERR_INVALID_PARAMETER,
      NETERR_OPERATION_ABORTED,
      NETERR_INTERUPTED_CALL,
      NETERR_BAD_FILE_HANDLE,
      NETERR_SOCKET_ACCESS_DENIED,
      NETERR_BAD_ADDRESS,
      NETERR_INVALID_ARGUMENT,
      NETERR_TOO_MANY_OPEN_FILES,
      NETERR_WOULD_BLOCK,
      NETERR_IN_PROGRESS,
      NETERR_ALREADY_IN_PROGRESS,
      NETERR_HANDLE_NON_SOCKET,
      NETERR_DESTINATION_ADDRESS_REQUIRED,
      NETERR_MESSAGE_TOO_LONG,
      NETERR_WRONG_PROTOTYPE,
      NETERR_BAD_PROTOCOL_OPTION,
      NETERR_PROTOCOL_NOT_SUPPORTED,
      NETERR_SOCKET_TYPE_NOT_SUPPORTED,
      NETERR_OPERATION_NOT_SUPPORTED,
      NETERR_PROTOCOL_FAMILY_NOT_SUPPORT,
      NETERR_ADDRESS_FAMILY_NOT_SUPPORT,
      NETERR_ADDRESS_IN_USE,
      NETERR_ADDRESS_NOT_AVAILABLE,
      NETERR_NETWORK_DOWN,
      NETERR_NETWORK_UNREACHABLE,
      NETERR_NETWORK_DROPPED_CONNECTION,
      NETERR_CONNECTION_RESET_BY_SOFTWARE,
      NETERR_CONNECTION_RESET_BY_PEER,
      NETERR_NO_BUFFER_SPACE_AVAIABLE,
      NETERR_ALREADY_CONNECTED,
      NETERR_NOT_CONNECTED,
      NETERR_SEND_SHUTDOWN,
      NETERR_TOO_MANY_REFS,
      NETERR_CONNECTION_TIMED_OUT,
      NETERR_CONNECTION_REFUSED,
      NETERR_TRANSLATE_NAME,
      NETERR_NAME_TOO_LONG,
      NETERR_HOST_DOWN,
      NETERR_HOST_UNREACHABLE,
      NETERR_PROCESS_LIMIT,
      NETERR_NETWORK_SYSTEM_NOT_READY,
      NETERR_VERSION_NOT_SUPPORTED,
      NETERR_NETWORK_NOT_INITIALISED,
      NETERR_DISCONNECT_IN_PROGRESS,
      NETERR_INVALIDPROCTABLE,
      NETERR_INVALIDPROVIDER,
      NETERR_PROVIDERFAILEDINIT,
      NETERR_SYSCALLFAILURE,
      NETERR_HOST_NOT_FOUND,
      NETERR_TRY_AGAIN,
      NETERR_NO_RECOVERY,
      NETERR_NO_DATA,
      NETERR_REMOTE_DISCONNECT,
      NETERR_SELECT_TIMEOUT,
      NETERR_UNKNOWN,
   };

   namespace network {
      bool init();
      void shut();

      namespace error {
         network_error_code get_error();
         const char *as_string(network_error_code error_code);
         bool is_non_critical(network_error_code error_code);
      } // !error
   } // !network

   struct ip_address {
      static bool local_addresses(dynamic_array<ip_address> &addresses);
      static bool lookup(const string &dns, dynamic_array<ip_address> &addresses);

      ip_address();
      explicit ip_address(uint32 host, uint16 port);
      explicit ip_address(uint8 a, uint8 b, uint8 c, uint8 d, uint16 port = 0);

      bool operator==(const ip_address &rhs) const;

      void set_host(uint8 a, uint8 b, uint8 c, uint8 d);
      void set_port(uint16 port);
      bool as_string(string &str) const;

      uint32 host_;
      uint16 port_;
   };

   struct byte_stream;
   struct udp_socket {
      udp_socket();
      ~udp_socket() = default;

      bool is_valid() const;
      void close();
      bool open(ip_address &addr);

      bool send_to(const ip_address &address, byte_stream &stream);
      bool recv_from(ip_address &address, byte_stream &stream);
      bool address_of(ip_address &address);

      uint32 handle_;
   };

   struct byte_stream {
      byte_stream(uint64 capacity, uint8 *base);
      ~byte_stream() = default;

      void reset();
      uint64 capacity() const;
      uint64 length() const;

      uint64 capacity_;
      uint8 *base_;
      uint8 *at_;
   };

   struct byte_stream_writer {
      byte_stream_writer(byte_stream &stream);
      ~byte_stream_writer() = default;

      bool serialize(float value);
      bool serialize(uint64 value);
      bool serialize(uint32 value);
      bool serialize(uint16 value);
      bool serialize(uint8 value);
      bool serialize(const uint32 count, const uint8 *values);

      byte_stream &stream_;
   };

   struct byte_stream_reader {
      byte_stream_reader(byte_stream &stream);
      ~byte_stream_reader() = default;

      bool serialize(float &value);
      bool serialize(uint64 &value);
      bool serialize(uint32 &value);
      bool serialize(uint16 &value);
      bool serialize(uint8 &value);
      bool serialize(const uint32 count, uint8 *values);

      byte_stream &stream_;
      uint8 *cursor_;
   };

   struct texture {
      texture();

      bool is_valid() const;
      bool create_from_file(const char *filename);
      bool create_from_memory(int width, int height, void *bitmap);
      void destroy();

      uint32 handle_;
      int32 width_;
      int32 height_;
   };

   struct render_system {
      render_system();
      ~render_system();

      void clear(uint32 color = 0xff000000);
      void draw(const uint32 color, const rectangle &dst);
      void draw(const texture &image, const rectangle &src, const rectangle &dst);
      void draw_text(int x, int y, uint32 color, int scale, const char *format, ...);

      uint32 texture_;
   };

   struct sprite {
      sprite();

      void set_texture(const texture &image);
      void set_source(const rectangle &rect);
      void set_size(const vector2 &size);
      void set_position(const vector2 &position);
      void render(render_system &rs);

      rectangle destination_;
      rectangle source_;
      const texture *image_;
   };

   struct collider {
      static bool overlap(const collider &lhs, const collider &rhs);

      collider();
      collider(const vector2 center, const vector2 extend);

      void set_size(const vector2 size);
      void set_center(const vector2 center);
      void set_position(const vector2 position);

      vector2 min() const;
      vector2 max() const;
      float width() const;
      float height() const;

      vector2 center_;
      vector2 extend_;
   };

   struct game_base {
      virtual ~game_base() = default;
      virtual bool enter() = 0;
      virtual void exit() = 0;
      virtual bool update(const time &dt, const keyboard &kb) = 0;
      virtual void render(render_system &rs) = 0;
   };

   game_base *create_game(string &caption, video_mode &mode);
} // !uu

#endif // GAMMA_H_INCLUDED
