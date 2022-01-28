#include "yrenderer.h"

#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "stb_image_resize.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "yluabind.hpp"
#include "yshader.h"
#include "ytexture.h"
#include "ywindow.h"

#include <SDL.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace yapre {

namespace renderer {

const float kMaxZ = 1024 * 1024;

int render_width = 320;
int render_height = 240;
bool keep_aspect = true;

unsigned int VBO = 0;
unsigned int draw_count = 0;

enum DrawType : unsigned char {
  sprite = 0,
  text,
};

using DrawData = std::tuple<unsigned int, DrawType, unsigned int, glm::mat4,
                            glm::mat4, glm::vec3, std::shared_ptr<float *>>;
std::unordered_map<std::string, std::shared_ptr<Texture>> texture_map;
std::vector<DrawData> draw_list;
glm::fvec4 clean_color = glm::fvec4(0.2, 0.2, 0.2, 1);

const float default_vertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

Shader *shader_sprite = nullptr;
Shader *shader_text = nullptr;
int viewport_x = 0;
int viewport_y = 0;
int viewport_w = render_width;
int viewport_h = render_height;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
  unsigned int TextureID; // ID handle of the glyph texture
  glm::ivec2 Size;        // Size of glyph
  glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::unordered_map<GLchar, Character> Characters;

void PrintGlInfo() {
  std::cout << "OpenGL loaded" << std::endl;
  std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
}

bool Init() {

  gladLoadGLLoader(SDL_GL_GetProcAddress);
  PrintGlInfo();
  // glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  shader_sprite = new Shader();
  shader_sprite->CompileFile("./shader/sprite.vs", "./shader/sprite.fs");

  shader_text = new Shader();
  shader_text->CompileFile("./shader/text.vs", "./shader/text.fs");

  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(default_vertices), default_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(0);

  // FreeType
  // --------
  FT_Library ft;
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;
    return false;
  }

  // load font as face
  FT_Face face;
  if (FT_New_Face(ft, "./font/zpix.ttf", 0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    return -1;
  }

  // set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, 12);

  // disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // load first 128 characters of ASCII set
  for (unsigned char c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
      continue;
    }
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // now store character for later use
    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<unsigned int>(face->glyph->advance.x)};
    Characters.insert(std::pair<char, Character>(c, character));
  }

  lua::GStateModule("yapre")
      .Define<void (*)(
          const std::string &, std::tuple<int, int, int>, std::tuple<int, int>,
          float, std::tuple<float, float, float>)>("DrawSprite", DrawSprite)
      .Define<void (*)(Texture *, std::tuple<int, int, int>,
                       std::tuple<int, int>, float,
                       std::tuple<float, float, float>)>(
          "DrawSpriteWithImageData", DrawSprite)
      .Define("SetClearColor", SetClearColor)
      .Define("SetRenderSize", SetRenderSize)
      .Define("SetKeepAspect", SetKeepAspect);

  lua::GLuaClass<Texture>("yapre", "Texture")
      .Ctor<unsigned int, unsigned int>("new")
      .Member("SetPixel", &Texture::SetPixel);
  SetRenderSize(320, 240);
  return true;
}

void Deinit() { delete shader_sprite; }

std::tuple<int, int> GetPreferRenderSize() {
  return std::make_tuple(render_width, render_height);
}

std::tuple<int, int> GetRealRenderSize() {
  auto [w, h] = window::GetDrawableSize();
  int dw = render_width;
  int dh = render_height;

  if (!keep_aspect) {
    if (1.0 * dw / dh > 1.0 * w / h) {
      dh = (int)(1.0 * dw * h / w + 0.5);
    } else {
      dw = (int)(1.0 * dh * w / h + 0.5);
    }
  }
  return std::make_tuple(dw, dh);
}

void _UpdateRenderSize(int width, int height) {
  lua::GStateModule("yapre")
      .Define("render_width", width)
      .Define("render_height", height);
}

void SetRenderSize(int width, int height) {
  render_width = width;
  render_height = height;
  window::ResetWindowSize();
  _UpdateRenderSize(width, height);
}

void DrawSprite(const std::string &texture_filename, glm::vec3 position,
                glm::vec2 size, float rotate, glm::vec3 color) {
  std::shared_ptr<Texture> texture_ptr;
  auto i = texture_map.find(texture_filename);
  if (i != texture_map.end()) {
    texture_ptr = i->second;
  } else {
    texture_ptr = std::make_shared<Texture>(texture_filename);
    texture_map[texture_filename] = texture_ptr;
  }
  DrawSprite(texture_ptr.get(), position, size, rotate, color);
}

void DrawSprite(const std::string &texture_filename, int x, int y, int z,
                int width, int height, float rotate, float R, float G,
                float B) {
  DrawSprite(texture_filename, glm::vec3(x, y, z), glm::vec2(width, height),
             rotate, glm::vec3(R, G, B));
}

void DrawSprite(const std::string &texture_filename,
                std::tuple<int, int, int> position, std::tuple<int, int> size,
                float rotate, std::tuple<float, float, float> color) {
  auto [x, y, z] = position;
  auto [width, height] = size;
  auto [R, G, B] = color;
  DrawSprite(texture_filename, glm::vec3(x, y, z), glm::vec2(width, height),
             rotate, glm::vec3(R, G, B));
}

void DrawSprite(Texture *texture, glm::vec3 position, glm::vec2 size,
                float rotate, glm::vec3 color) {

  texture->UpdateData();

  if (size.x < 0 || size.y < 0) {
    size.x = texture->Width();
    size.y = texture->Height();
  }
  int real_size = texture->RealSize();

  position.z = position.z;
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::translate(
      model, glm::vec3(0.5f * size.x, 0.5f * size.y,
                       0.0f)); // move origin of rotation to center of quad
  model = glm::rotate(model, glm::radians(rotate),
                      glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y,
                                          0.0f)); // move origin back

  model = glm::scale(model, glm::vec3(real_size * size.x / texture->Width(),
                                      real_size * size.y / texture->Height(),
                                      1.0f)); // last scale

  auto [w, h] = GetRealRenderSize();
  glm::mat4 projection =
      glm::ortho(0.0f, 1.f * w, 1.f * h, 0.0f, -kMaxZ, kMaxZ);
  unsigned int draw_id = position.z * 1024 * 1024 + draw_count;
  draw_count++;

  draw_list.emplace_back(
      std::make_tuple(draw_id, DrawType::sprite, texture->TextureID(), model,
                      projection, color, std::shared_ptr<float *>(nullptr)));
  return;
}

void DrawSprite(Texture *image_data, int x, int y, int z, int width, int height,
                float rotate, float R, float G, float B) {
  DrawSprite(image_data, glm::vec3(x, y, z), glm::vec2(width, height), rotate,
             glm::vec3(R, G, B));
}

void DrawSprite(Texture *image_data, std::tuple<int, int, int> position,
                std::tuple<int, int> size, float rotate,
                std::tuple<float, float, float> color) {
  auto [x, y, z] = position;
  auto [width, height] = size;
  auto [R, G, B] = color;
  DrawSprite(image_data, glm::vec3(x, y, z), glm::vec2(width, height), rotate,
             glm::vec3(R, G, B));
}

void DrawText(const std::string &text, float x, float y, float scale,
              glm::vec3 color) {
  // iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    Character ch = Characters[*c];

    float xpos = x + ch.Bearing.x * scale;
    float ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;
    float zpos = 1;

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;

    // update VBO for each character
    auto vertices = std::make_shared<float *>(new float[24]{
        xpos,     ypos + h, 0.0f, 0.0f, xpos,     ypos,     0.0f, 1.0f,
        xpos + w, ypos,     1.0f, 1.0f, xpos,     ypos + h, 0.0f, 0.0f,
        xpos + w, ypos,     1.0f, 1.0f, xpos + w, ypos + h, 1.0f, 0.0f});

    x += (ch.Advance >> 6) *
         scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount
                // of 1/64th pixels by 64 to get amount of pixels))
    
    glm::vec3 position(xpos, ypos, zpos);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::translate(model, glm::vec3(0.5f * w, 0.5f * h,
            0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(0.f),
        glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * w, -0.5f * h,
        0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(w,
        h,
        1.0f)); // last scale

    auto [rw, rh] = GetRealRenderSize();
    glm::mat4 projection =
        glm::ortho(0.0f, 1.f * rw, 1.f * rh, 0.0f, -kMaxZ, kMaxZ);
    unsigned int draw_id = position.z * 1024 * 1024 + draw_count;
    draw_count++;

    draw_list.emplace_back(
        std::make_tuple(draw_id, DrawType::text, ch.TextureID, model,
            projection, color, std::shared_ptr<float*>(nullptr)));
  }
}

void DrawAll() {
  draw_count = 0;

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glActiveTexture(GL_TEXTURE0);

  std::sort(draw_list.begin(), draw_list.end(),
            [](const DrawData &a, const DrawData &b) {
              return std::get<0>(a) < std::get<0>(b);
            });

  for (auto [draw_id, draw_type, texture_id, model, projection, color,
             vertices] : draw_list) {
    if (draw_type == DrawType::sprite) {
      shader_sprite->Use();
      shader_sprite->SetInteger("sprite", 0);
      shader_sprite->SetMatrix4("projection", projection);
      shader_sprite->SetMatrix4("model", model);
      shader_sprite->SetVector3f("spriteColor", color);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(default_vertices),
                      default_vertices);

      glBindTexture(GL_TEXTURE_2D, texture_id);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    } else if (draw_type == DrawType::text) {
      shader_text->Use();
      shader_sprite->SetInteger("sprite", 0);
      shader_sprite->SetMatrix4("projection", projection);
      shader_sprite->SetMatrix4("model", model);
      shader_sprite->SetVector3f("spriteColor", color);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(default_vertices),
          default_vertices);

      glBindTexture(GL_TEXTURE_2D, texture_id);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      std::cout << "FF" << std::endl;
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  draw_list.clear();
}

void RefreshViewport() {
  auto [w, h] = window::GetDrawableSize();
  auto [dw, dh] = GetRealRenderSize();
  int rx = 0;
  int ry = 0;

  if (1.0 * dw / dh > 1.0 * w / h) {
    int rh = w * dh / dw;
    ry = (h - rh) / 2;
    h = rh;
  } else {
    int rw = h * dw / dh;
    rx = (w - rw) / 2;
    w = rw;
  }

  _UpdateRenderSize(dw, dh);

  viewport_x = rx;
  viewport_y = ry;
  viewport_w = w;
  viewport_h = h;
}

void Update(int delta_ms) {
  DrawText("‰∆abcdefghijklmn", 10, 2, 1, glm::vec3(1, 1, 1));
  auto [w, h] = window::GetDrawableSize();
  lua::GStateModule("yapre")
      .Define("drawable_width", w)
      .Define("drawable_height", h);

  RefreshViewport();
  glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
  glClearColor(clean_color.r, clean_color.g, clean_color.b, clean_color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  DrawAll();
  window::SwapWinodw();
}

std::tuple<int, int> ConvertToViewport(int x, int y) {
  auto [rw, rh] = GetRealRenderSize();
  return std::make_tuple((x - viewport_x) * rw / viewport_w,
                         (y - viewport_y) * rh / viewport_h);
}

void SetClearColor(float R, float G, float B, float A) {
  clean_color.r = R;
  clean_color.g = G;
  clean_color.b = B;
  clean_color.a = A;
}

void SetKeepAspect(bool keey_aspect_) { keep_aspect = keey_aspect_; }

} // namespace renderer
} // namespace yapre
