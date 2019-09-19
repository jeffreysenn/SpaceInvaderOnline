// rendering.cc

#pragma warning(disable: 4244) // conversion from 'int' to 'float', possible loss of data

#include "gamma.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>

#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gamma {
   namespace {
      template <typename Fn>
      struct scope_guard {
         scope_guard(Fn f) : fn(f) { }
         ~scope_guard() { fn(); }
         Fn fn;
      };

      template <typename Fn>
      scope_guard<Fn> make_scope_guard(Fn f) {
         return scope_guard<Fn>(f);
      }

      bool load_file_content(const char *filename, dynamic_array<uint8> &data) {
         HANDLE handle = CreateFileA(filename,
                                     GENERIC_READ,
                                     FILE_SHARE_READ,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL);
         if (handle == INVALID_HANDLE_VALUE) {
            return false;
         }

         auto defer = make_scope_guard(([&]() {
            CloseHandle(handle);
         }));

         LARGE_INTEGER size = {};
         if (!GetFileSizeEx(handle, &size)) {
            return false;
         }

         data.resize(size.QuadPart);
         if (!ReadFile(handle, data.data(), size.LowPart, NULL, NULL)) {
            return false;
         }

         if (size.HighPart > 0) {
            if (!ReadFile(handle, data.data() + size.LowPart, size.HighPart, NULL, NULL)) {
               return false;
            }
         }

         return true;
      }

      GLuint opengl_create_texture(int width, int height, void *bitmap) {
         GLuint id = 0;
         glGenTextures(1, &id);
         glBindTexture(GL_TEXTURE_2D, id);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
         glBindTexture(GL_TEXTURE_2D, 0);
         GLenum err = glGetError();
         if (err != GL_NO_ERROR) {
            assert(!"opengl texture create error");
         }
         return id;
      }

      void set_pixel(int width, int height, uint32 *dst, int x, int y, uint32 src) {
         if (x < 0 || x >= width) return;
         if (y < 0 || y >= height) return;
         dst[y * width + x] = src;
      }

      void blit_glyphs(int width, int height, uint32 *bitmap) {
         // Source:
         // - https://github.com/dhepper/font8x8
         //
         // license: 
         // - Public Domain
         //
         const unsigned char font8x8_basic[][8] = {
            { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},   // U+0021 (!)
            { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0022 (")
            { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00},   // U+0023 (#)
            { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},   // U+0024 ($)
            { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},   // U+0025 (%)
            { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},   // U+0026 (&)
            { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0027 (')
            { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},   // U+0028 (()
            { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},   // U+0029 ())
            { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},   // U+002A (*)
            { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00},   // U+002B (+)
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+002C (,)
            { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00},   // U+002D (-)
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+002E (.)
            { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},   // U+002F (/)
            { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},   // U+0030 (0)
            { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},   // U+0031 (1)
            { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},   // U+0032 (2)
            { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},   // U+0033 (3)
            { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},   // U+0034 (4)
            { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},   // U+0035 (5)
            { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},   // U+0036 (6)
            { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},   // U+0037 (7)
            { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},   // U+0038 (8)
            { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},   // U+0039 (9)
            { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},   // U+003A (:)
            { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06},   // U+003B (//)
            { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},   // U+003C (<)
            { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00},   // U+003D (=)
            { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},   // U+003E (>)
            { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},   // U+003F (?)
            { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},   // U+0040 (@)
            { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},   // U+0041 (A)
            { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},   // U+0042 (B)
            { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},   // U+0043 (C)
            { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},   // U+0044 (D)
            { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},   // U+0045 (E)
            { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00},   // U+0046 (F)
            { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},   // U+0047 (G)
            { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},   // U+0048 (H)
            { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0049 (I)
            { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},   // U+004A (J)
            { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},   // U+004B (K)
            { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00},   // U+004C (L)
            { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},   // U+004D (M)
            { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+004E (N)
            { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+004F (O)
            { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+0050 (P)
            { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},   // U+0051 (Q)
            { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},   // U+0052 (R)
            { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},   // U+0053 (S)
            { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0054 (T)
            { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00},   // U+0055 (U)
            { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0056 (V)
            { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},   // U+0057 (W)
            { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},   // U+0058 (X)
            { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},   // U+0059 (Y)
            { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},   // U+005A (Z)
            { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00},   // U+005B ([)
            { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00},   // U+005C (\)
            { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00},   // U+005D (])
            { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00},   // U+005E (^)
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF},   // U+005F (_)
            { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0060 (`)
            { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00},   // U+0061 (a)
            { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00},   // U+0062 (b)
            { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00},   // U+0063 (c)
            { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00},   // U+0064 (d)
            { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00},   // U+0065 (e)
            { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00},   // U+0066 (f)
            { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0067 (g)
            { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00},   // U+0068 (h)
            { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0069 (i)
            { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E},   // U+006A (j)
            { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00},   // U+006B (k)
            { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+006C (l)
            { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00},   // U+006D (m)
            { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00},   // U+006E (n)
            { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+006F (o)
            { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F},   // U+0070 (p)
            { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78},   // U+0071 (q)
            { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00},   // U+0072 (r)
            { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00},   // U+0073 (s)
            { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+0074 (t)
            { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00},   // U+0075 (u)
            { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+0076 (v)
            { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00},   // U+0077 (w)
            { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00},   // U+0078 (x)
            { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F},   // U+0079 (y)
            { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00},   // U+007A (z)
            { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00},   // U+007B ({)
            { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+007C (|)
            { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00},   // U+007D (})
            { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007E (~)
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007F
         };

         const int glyph_atlas_columns = 16;
         const int glyph_atlas_rows = 8;
         const int glyph_count = (sizeof(font8x8_basic) / sizeof(font8x8_basic[0]));
         for (int glyph = 0; glyph < glyph_count; glyph++) {
            const int glyph_x = glyph % glyph_atlas_columns * 8;
            const int glyph_y = glyph / glyph_atlas_columns * 8;

            for (int y = 0; y < 8; y++) {
               for (int x = 0; x < 8; x++) {
                  if (font8x8_basic[glyph][y] & 1 << x)
                     set_pixel(width, height, bitmap, glyph_x + x, glyph_y + y, 0xffffffff);
                  else
                     set_pixel(width, height, bitmap, glyph_x + x, glyph_y + y, 0x00000000);
               }
            }
         }

         set_pixel(width, height, bitmap, width - 1, height - 1, 0xffffffff);
      }
   } // !anon

   namespace {
      struct vertex {
         vector2 pos_;
         vector2 tex_;
      };

      void opengl_render(int start, int count, const vertex *vertices) {
         const uint8 *base = (const uint8 *)vertices;
         glVertexPointer(2, GL_FLOAT, sizeof(vertex), (const GLvoid *)(base + offsetof(vertex, pos_)));
         glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (const GLvoid *)(base + offsetof(vertex, tex_)));
         glDrawArrays(GL_QUADS, start, count);
      }
   } // !anon

   render_system::render_system()
      : texture_(0)
   {
      uint32 *bitmap = new uint32[16384];
      blit_glyphs(128, 128, bitmap);
      texture_ = opengl_create_texture(128, 128, bitmap);
      delete[] bitmap;
   }

   render_system::~render_system()
   {
   }

   void render_system::clear(uint32 color) {
      float r = ((color >> 16) & 0xff) / 255.0f;
      float g = ((color >> 8) & 0xff) / 255.0f;
      float b = ((color >> 0) & 0xff) / 255.0f;
      float a = ((color >> 24) & 0xff) / 255.0f;
      glClearColor(r, g, b, a);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   void render_system::draw_text(int x, int y, uint32 color, int scale, const char *format, ...) {
      char text[2048] = { 0 };
      va_list args;
      va_start(args, format);
      size_t len = vsprintf_s(text, sizeof(text) - 1, format, args);
      va_end(args);

      const float uvst = 1.0f / 16.0f;
      const int character_width = 8 * scale;
      const int line_feed_height = 10;
      const int first_valid_character = (int)'!';
      const int last_valid_character = (int)'~';
      const int invalid_character = (int)'?' - first_valid_character;

      glColor4ubv((const GLubyte *)& color);
      glBindTexture(GL_TEXTURE_2D, texture_);

      int ox = x;
      for (size_t index = 0; index < len; index++) {
         int character = (int)text[index];
         if (character == (int)(' ')) {
            x += character_width;
            continue;
         }
         else if (character == (int)('\n')) {
            x = ox;
            y += line_feed_height;
            continue;
         }

         int character_index = character - first_valid_character;
         if (character_index < 0 || character_index >= last_valid_character) {
            character_index = invalid_character;
         }

         const float x0 = x;
         const float y0 = y;
         const float x1 = x + character_width;
         const float y1 = y + character_width;

         const float u0 = (character_index % 16) * uvst;
         const float v0 = (character_index / 16) * uvst;
         const float u1 = u0 + uvst;
         const float v1 = v0 + uvst;

         const vertex vertices[] =
         {
            { { x0, y0 }, { u0, v0 } },
            { { x1, y0 }, { u1, v0 } },
            { { x1, y1 }, { u1, v1 } },
            { { x0, y1 }, { u0, v1 } },
         };
         opengl_render(0, 4, vertices);

         x += character_width;
      }

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   void render_system::draw(const uint32 color, const rectangle &dst) {
      glBindTexture(GL_TEXTURE_2D, 0);
      glColor4ubv((const GLubyte *)&color);
    
      const float x0 = dst.x_;
      const float y0 = dst.y_;
      const float x1 = dst.x_ + dst.width_;
      const float y1 = dst.y_ + dst.height_;

      const vertex vertices[] =
      {
         { { x0, y0 }, { 0.0f, 0.0f } },
         { { x1, y0 }, { 0.0f, 0.0f } },
         { { x1, y1 }, { 0.0f, 0.0f } },
         { { x0, y1 }, { 0.0f, 0.0f } },
      };
      opengl_render(0, 4, vertices);
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   }

   void render_system::draw(const texture &image, const rectangle &src, const rectangle &dst) {
      glBindTexture(GL_TEXTURE_2D, image.handle_);

      const float x0 = dst.x_;
      const float y0 = dst.y_;
      const float x1 = dst.x_ + dst.width_;
      const float y1 = dst.y_ + dst.height_;

      const float u0 = src.x_;
      const float v0 = src.y_;
      const float u1 = src.x_ + src.width_;
      const float v1 = src.y_ + src.height_;

      const vertex vertices[] =
      {
         { { x0, y0 }, { u0, v0 } },
         { { x1, y0 }, { u1, v0 } },
         { { x1, y1 }, { u1, v1 } },
         { { x0, y1 }, { u0, v1 } },
      };
      opengl_render(0, 4, vertices);
   }

   texture::texture()
      : handle_(0)
      , width_(0)
      , height_(0)
   {
   }

   bool texture::is_valid() const {
      return handle_ != 0;
   }

   bool texture::create_from_file(const char *filename) {
      if (is_valid()) {
         destroy();
      }

      dynamic_array<uint8> file_content;
      if (!load_file_content(filename, file_content)) {
         return false;
      }

      int width = 0, height = 0, comp = 0;
      auto bitmap = stbi_load_from_memory(file_content.data(), (int)file_content.size(),
                                          &width, &height, &comp, STBI_rgb_alpha);
      if (!bitmap) {
         return false;
      }

      handle_ = opengl_create_texture(width, height, bitmap);
      width_ = width;
      height_ = height;

      stbi_image_free(bitmap);

      return is_valid();
   }

   bool texture::create_from_memory(int width, int height, void *bitmap) {
      if (is_valid()) {
         destroy();
      }

      handle_ = opengl_create_texture(width, height, bitmap);
      width_ = width;
      height_ = height;

      return is_valid();
   }

   void texture::destroy() {
      if (is_valid()) {
         glDeleteTextures(1, &handle_);
      }

      handle_ = 0;
      width_ = 0;
      height_ = 0;
   }

   sprite::sprite()
      : image_(nullptr)
   {
   }

   void sprite::set_texture(const texture &image) {
      image_ = &image;
   }

   void sprite::set_source(const rectangle &rect) {
      float u0 = 0.0f;
      float v0 = 0.0f;
      float u1 = 0.0f;
      float v1 = 0.0f;

      if (image_) {
         const float iw = 1.0f / image_->width_;
         const float ih = 1.0f / image_->height_;
         u0 = rect.x_ * iw;
         v0 = rect.y_ * ih;
         u1 = rect.width_ * iw;
         v1 = rect.height_ * ih;
      }

      source_ = { u0, v0, u1, v1 };
   }

   void sprite::set_size(const vector2 &size) {
      destination_.width_ = size.x_;
      destination_.height_ = size.y_;
   }

   void sprite::set_position(const vector2 &position) {
      destination_.x_ = position.x_;
      destination_.y_ = position.y_;
   }

   void sprite::render(render_system &rc) {
      if (!image_) {
         rc.draw(0xffffffff, destination_);
      }
      else {
         rc.draw(*image_, source_, destination_);
      }
   }
} // !uu
