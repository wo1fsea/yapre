#include "yrenderer.h"

#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "stb_image_resize.h"

#include "yluabind.hpp"
#include "yshader.h"
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

unsigned int VBO = 0;
unsigned int draw_count = 0;
using DrawData =
    std::tuple<unsigned int, unsigned int, glm::mat4, glm::mat4, glm::vec3>;
using TextureData = std::tuple<unsigned int, int, int>;
std::unordered_map<std::string, TextureData> texture_map;
std::vector<DrawData> draw_list;
glm::fvec4 clean_color = glm::fvec4(0.2, 0.2, 0.2, 1);

const float vertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

Shader *shader = nullptr;
int viewport_x = 0;
int viewport_y = 0;
int viewport_w = render_width;
int viewport_h = render_height;

void PrintGlInfo() {
  std::cout << "OpenGL loaded" << std::endl;
  std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
}

bool Init() {

  gladLoadGLLoader(SDL_GL_GetProcAddress);
  PrintGlInfo();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  std::ifstream vertexShaderFile("data/shaders/sprite.vs");
  std::ifstream fragmentShaderFile("data/shaders/sprite.fs");
  std::stringstream vShaderStream, fShaderStream;
  vShaderStream << vertexShaderFile.rdbuf();
  fShaderStream << fragmentShaderFile.rdbuf();
  vertexShaderFile.close();
  fragmentShaderFile.close();
  vertexCode = vShaderStream.str();
  fragmentCode = fShaderStream.str();

  shader = new Shader();
  shader->Compile(vertexCode.c_str(), fragmentCode.c_str());

  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(0);

  lua::GStateModule("yapre")
      .Define<void (*)(const std::string &, int, int, int, int, int, float,
                       float, float, float)>("DrawSprite", DrawSprite)
      .Define("SetClearColor", SetClearColor)
      .Define("SetRenderSize", SetRenderSize);
  SetRenderSize(320, 240);
  return true;
}

void Deinit() { delete shader; }

std::tuple<int, int> GetRenderSize() {
  return std::make_tuple(render_width, render_height);
}

void SetRenderSize(int width, int height) {
  render_width = width;
  render_height = height;
  window::ResetWindowSize();

  lua::GStateModule("yapre")
      .Define("redner_width", width)
      .Define("redner_height", height);
}

std::tuple<unsigned int, int, int> GetTextureId(std::string texture_filename) {
  auto i = texture_map.find(texture_filename);
  if (i != texture_map.end()) {
    return i->second;
  }

  int n_channels;
  unsigned int texture_id = 0;
  int width, height;
  unsigned char *data =
      stbi_load(texture_filename.c_str(), &width, &height, &n_channels, 0);

  int len = width > height ? width : height;
  int n_len = 2;
  while (n_len < len) {
    n_len *= 2;
  }
  auto tmp = std::make_unique<unsigned char[]>(n_len * n_len * n_channels);
  stbir_resize_uint8(data, width, height, 0, tmp.get(), n_len, n_len, 0,
                     n_channels);

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, n_len, n_len, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, tmp.get());

  stbi_image_free(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBindTexture(GL_TEXTURE_2D, 0);

  auto texture_info = std::make_tuple(texture_id, width, height);
  texture_map[texture_filename] = texture_info;
  return texture_info;
}

void DrawSprite(const std::string &texture_filename, glm::vec3 position,
                glm::vec2 size, float rotate, glm::vec3 color) {

  auto [texture_id, texture_w, texture_h] = GetTextureId(texture_filename);

  if (size.x < 0 || size.y < 0) {
    size.x = texture_w;
    size.y = texture_h;
  }

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

  model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale
  auto [w, h] = GetRenderSize();
  glm::mat4 projection =
      glm::ortho(0.0f, 1.f * w, 1.f * h, 0.0f, -kMaxZ, kMaxZ);
  unsigned int draw_id = position.z * 1024 * 1024 + draw_count;
  draw_count++;

  draw_list.emplace_back(
      std::make_tuple(draw_id, texture_id, model, projection, color));
  return;
}

void DrawSprite(const std::string &texture_filename, int x, int y, int z,
                int width, int height, float rotate, float R, float G,
                float B) {
  DrawSprite(texture_filename, glm::vec3(x, y, z), glm::vec2(width, height),
             rotate, glm::vec3(R, G, B));
}

void DrawAll() {
  draw_count = 0;
  shader->Use();
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glActiveTexture(GL_TEXTURE0);
  shader->SetInteger("sprite", 0);

  std::sort(draw_list.begin(), draw_list.end(),
            [](const DrawData &a, const DrawData &b) {
              return std::get<0>(a) < std::get<0>(b);
            });

  for (auto [draw_id, texture_id, model, projection, color] : draw_list) {
    shader->SetMatrix4("projection", projection);
    shader->SetMatrix4("model", model);
    shader->SetVector3f("spriteColor", color);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  draw_list.clear();
}

void RefreshViewport() {
  auto [w, h] = window::GetDrawableSize();
  auto [dw, dh] = GetRenderSize();
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
  viewport_x = rx;
  viewport_y = ry;
  viewport_w = w;
  viewport_h = h;
}

void Update(int delta_ms) {
  RefreshViewport();
  glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
  glClearColor(clean_color.r, clean_color.g, clean_color.b, clean_color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  DrawAll();
  window::SwapWinodw();
}

std::tuple<int, int> ConvertToViewport(int x, int y) {
  auto [rw, rh] = GetRenderSize();
  return std::make_tuple((x - viewport_x) * rw / viewport_w,
                         (y - viewport_y) * rh / viewport_h);
}

void SetClearColor(float R, float G, float B, float A) {
  clean_color.r = R;
  clean_color.g = G;
  clean_color.b = B;
  clean_color.a = A;
}

} // namespace renderer
} // namespace yapre
