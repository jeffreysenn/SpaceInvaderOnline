// video_mode.cc

#include "gamma.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

extern HWND win32_get_window_handle();
extern void opengl_projection(int width, int height);

namespace gamma {
   // static
   bool video_mode::get_desktop(video_mode &mode) {
      DEVMODE dm = {};
      EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
      mode = video_mode((int)dm.dmPelsWidth, (int)dm.dmPelsHeight);
      return true;
   }

   bool video_mode::get_supported(dynamic_array<video_mode> &modes) {
      for (int index = 0; ; index++) {
         DEVMODE mode = {};
         if (!EnumDisplaySettings(NULL, index, &mode)) {
            break;
         }

         if (mode.dmBitsPerPel != 32) {
            continue;
         }

         if (mode.dmPelsWidth < 1024) {
            continue;
         }

         if (mode.dmPelsHeight > mode.dmPelsWidth) {
            continue;
         }

         modes.push_back(video_mode((int)mode.dmPelsWidth, (int)mode.dmPelsHeight));
      }

      return !modes.empty();
   }

   void video_mode::set_mode(const video_mode &mode) {
      HWND window = win32_get_window_handle();
      RECT window_rect = { 0 };
      GetWindowRect(window, &window_rect);

      int window_x = window_rect.left;
      int window_y = window_rect.top;

      window_rect = {};
      window_rect.right = mode.width_;
      window_rect.bottom = mode.height_;
      DWORD style = GetWindowStyle(window);
      AdjustWindowRect(&window_rect, style, 0);

      int window_width = window_rect.right - window_rect.left;
      int window_height = window_rect.bottom - window_rect.top;
      SetWindowPos(window,
                   NULL,
                   window_x,
                   window_y,
                   window_width,
                   window_height,
                   SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

      opengl_projection(mode.width_, mode.height_);
   }

   video_mode::video_mode()
      : width_(0)
      , height_(0)
   {
   }

   video_mode::video_mode(int width, int height)
      : width_(width)
      , height_(height)
   {
   }
} // !gamma
