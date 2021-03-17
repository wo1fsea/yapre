#include "yrenderer.h"

#include "SDL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "stb_image_resize.h"

#include "yshader.h"
#include "ywindow.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace yapre {
namespace renderer {

unsigned int VBO;
std::unordered_map<std::string, std::tuple<unsigned int, int, int>> texture_map;
std::vector<std::tuple<unsigned int, glm::mat4, glm::mat4, glm::vec3>>
    draw_list;

const float vertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

Shader *shader = nullptr;

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

  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  std::ifstream vertexShaderFile("data/shaders/shader.vs");
  std::ifstream fragmentShaderFile("data/shaders/shader.fs");
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

  return true;
}

void Deinit() { delete shader; }

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

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  auto texture_info = std::make_tuple(texture_id, width, height);
  texture_map[texture_filename] = texture_info;
  return texture_info;
}

void DrawSprite(std::string texture_filename, glm::vec3 position,
                glm::vec2 size, float rotate, glm::vec3 color) {

  auto [texture_id, texture_w, texture_h] = GetTextureId(texture_filename);

  if (size.x < 0 || size.y < 0) {
    size.x = texture_w;
    size.y = texture_h;
  }

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
  auto [w, h] = window::GetDesignSize();
  glm::mat4 projection = glm::ortho(0.0f, 1.f * w, 1.f * h, 0.0f, -1.0f, 1.0f);

  draw_list.emplace_back(std::make_tuple(texture_id, model, projection, color));
  return;
}

void DrawAll() {
  shader->Use();
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glActiveTexture(GL_TEXTURE0);
  shader->SetInteger("sprite", 0);

  for (auto [texture_id, model, projection, color] : draw_list) {
    shader->SetMatrix4("projection", projection);
    shader->SetMatrix4("model", model);
    shader->SetVector3f("spriteColor", color);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  draw_list.clear();
}

void Update() {

  auto [w, h] = window::GetDrawableSize();
  glViewport(0, 0, w, h);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto [dw, dh] = window::GetDesignSize();
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

  glViewport(rx, ry, w, h);
  glClearColor(.2f, .2f, .2f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  DrawAll();
  window::SwapWinodw();
}

} // namespace renderer
} // namespace yapre
