// main.cc

#include "gamma.h"

#include <assert.h>
#include <Windows.h>
#include <gl/GL.h>

extern void random_seed(int s);

struct input_state {
   struct {
      bool current_;
      bool previous_;
   } keys_[gamma::keycode::KEYCODE_COUNT]{};
};

static void
input_state_process(input_state &is, gamma::keyboard &kb) {
   for (int index = 0; index < gamma::keycode::KEYCODE_COUNT; index++) {
      kb.keys_[index].pressed_ = false;
      kb.keys_[index].released_ = false;

      if (is.keys_[index].current_ != is.keys_[index].previous_) {
         if (is.keys_[index].current_) {
            kb.keys_[index].down_ = true;
            kb.keys_[index].pressed_ = true;
         }
         else {
            kb.keys_[index].down_ = false;
            kb.keys_[index].released_= true;
         }

         is.keys_[index].previous_ = is.keys_[index].current_;
      }
   }
}

void opengl_projection(int width, int height) {
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, (GLdouble)width, (GLdouble)height, 0.0, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

static HWND g_window = nullptr;
HWND win32_get_window_handle() {
   return g_window;
}

static LRESULT CALLBACK
win32_main_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
   input_state *is = (input_state *)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
   if (!is) { return DefWindowProcA(hWnd, uMsg, wParam, lParam); }

   switch (uMsg)
   {
      case WM_KEYDOWN:
      case WM_KEYUP:
      {
         int index = (int)wParam;
         bool down = uMsg == WM_KEYDOWN;
         if (index <= 0xff) {
            is->keys_[index].current_ = down;
         }
      } break;

      case WM_CLOSE:
      {
         PostQuitMessage(0);
      } break;
      
      default:
      {
         return DefWindowProcA(hWnd, uMsg, wParam, lParam);
      } break;
   }

   return 0;
}

static bool
win32_register_class(const char *ident) {
   WNDCLASSA wc = { 0 };
   wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc = win32_main_proc;
   wc.hInstance = GetModuleHandle(NULL);
   wc.lpszClassName = ident;
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = CreateSolidBrush(0x00000000);
   wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(101));
   if (!RegisterClassA(&wc))
      return false;

   return true;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
   const int width = 1024;
   const int height = 576;

   win32_register_class("spinningClassName");

   DWORD ws = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
   RECT wr = { 0, 0, width, height };
   if (!AdjustWindowRect(&wr, ws, 0)) {
      return -1;
   }

   int x = CW_USEDEFAULT;
   int y = CW_USEDEFAULT;
   HWND window = CreateWindowA("spinningClassName",
                               "gamma",
                               ws,
                               x,
                               y,
                               wr.right - wr.left,
                               wr.bottom - wr.top,
                               0, 0,
                               instance,
                               0);
   if (!window) {
      return -1;
   }

   g_window = window;
   HDC device = GetDC(window);

   PIXELFORMATDESCRIPTOR pfd = { 0 };
   pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
   pfd.nVersion = 1;
   pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 32;
   pfd.iLayerType = PFD_MAIN_PLANE;

   int pixel_format_index = ChoosePixelFormat(device, &pfd);
   SetPixelFormat(device, pixel_format_index, &pfd);

   HGLRC context = wglCreateContext(device);
   if (!wglMakeCurrent(device, context)) {
      return -1;
   }

#if 0
   bool vsync_available = false;
   typedef BOOL WINAPI wglSwapIntervalEXT_t(int interval);
   wglSwapIntervalEXT_t *wglSwapIntervalEXT = (wglSwapIntervalEXT_t *)wglGetProcAddress("wglSwapIntervalEXT");
   if (wglSwapIntervalEXT) {
      vsync_available = true;
      wglSwapIntervalEXT(1);
   }
#endif

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_TEXTURE_2D);
   glFrontFace(GL_CW);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   opengl_projection(width, height);

   input_state is;
   SetWindowLongPtrA(window, GWLP_USERDATA, (LONG_PTR)&is);
   ShowWindow(window, SW_NORMAL);

   LARGE_INTEGER s = {};
   QueryPerformanceCounter(&s);
   random_seed((int)s.LowPart);

   gamma::keyboard kb;
   gamma::render_system rs;
   gamma::time time = gamma::time::now();

   gamma::string caption = "gamma";
   gamma::video_mode mode(width, height);
   gamma::game_base *game = gamma::create_game(caption, mode);
   if (!game->enter()) {
      gamma::system::message_box("Could not create game instance!");
      return -1;
   }

   SetWindowTextA(window, caption.c_str());
   gamma::video_mode::set_mode(mode);

   bool running = true;
   while (running) {
      MSG msg = {};
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
         if (msg.message == WM_QUIT) {
            running = false;
         }
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
      input_state_process(is, kb);

      gamma::time current = gamma::time::now();
      gamma::time dt = (current - time);
      time = current;
      
      running = running && game->update(dt, kb);

      game->render(rs);
      SwapBuffers(device);
      Sleep(16);
   }

   delete game;

   return 0;
}
