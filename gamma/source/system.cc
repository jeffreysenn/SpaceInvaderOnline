// system.cc

#include "gamma.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdarg.h>

namespace gamma {
   namespace system {
      bool message_box(const char *format, ...) {
         char message[2048] = {};
         va_list args;
         va_start(args, format);
         vsprintf_s(message, sizeof(message), format, args);
         va_end(args);
         return MessageBoxA(NULL, message, "Info", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK;
      }
   } // !system
} // !uu
