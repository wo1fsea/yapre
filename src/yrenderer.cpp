#include "yrenderer.h"

#include "SDL.h"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "stb_image_resize.h"

#include "yluabind.hpp"
#include "yshader.h"
#include "ywindow.h"

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

GLuint FramebufferName = 0;
GLuint renderedTexture;

unsigned int screen_VBO = 0;
unsigned int VBO = 0;
unsigned int draw_count = 0;

using DrawData =
    std::tuple<unsigned int, unsigned int, glm::mat4, glm::mat4, glm::vec3>;
using TextureData = std::tuple<unsigned int, int, int>;
std::unordered_map<std::string, TextureData> texture_map;
std::vector<DrawData> draw_list;

const float vertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
};

const float screen_vertices[] = {
    // pos      // tex
    -1.0f, 1.0f, 0.0f, 1.0f, 
    1.0f, -1.0f, 1.0f, 0.0f, 
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f, 
    1.0f, 1.0f, 1.0f, 1.0f, 
    1.0f, -1.0f, 1.0f, 0.0f
};

Shader *shader = nullptr;
Shader *shader_screen = nullptr;

int viewport_x = 0;
int viewport_y = 0;
int viewport_w = kDefaultViewWidth;
int viewport_h = kDefaultViewHeight;

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

  std::string vertexCode1 = "";
  std::string fragmentCode1 = "";
  std::ifstream vertexShaderFile1("data/shaders/screen.vs");
  std::ifstream fragmentShaderFile1("data/shaders/screen.fs");
  std::stringstream vShaderStream1, fShaderStream1;
  vShaderStream1 << vertexShaderFile1.rdbuf();
  fShaderStream1 << fragmentShaderFile1.rdbuf();
  vertexShaderFile1.close();
  fragmentShaderFile1.close();
  vertexCode1 = vShaderStream1.str();
  fragmentCode1 = fShaderStream1.str();
  shader_screen = new Shader();
  shader_screen->Compile(vertexCode1.c_str(), fragmentCode1.c_str());

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &screen_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, screen_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // ==========

  // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
  glGenFramebuffers(1, &FramebufferName);
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  // The texture we're going to render to
  glGenTextures(1, &renderedTexture);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, renderedTexture);

  // Give an empty image to OpenGL ( the last "0" )
  auto [w, h] = GetRenderSize();
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, w, h, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // The depth buffer
  GLuint depthrenderbuffer;
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  // Set "renderedTexture" as our colour attachement #0
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

  // Set the list of draw buffers.
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
  
  glBindTexture(GL_TEXTURE_2D, 0);

  // Always check that our framebuffer is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false; 
  // ==========

  lua::GStateModule("yapre")
      .Define<void (*)(const std::string &, int, int, int, int, int, float,
                       float, float, float)>("DrawSprite", DrawSprite);
  return true;
}

void Deinit() { delete shader; }

std::tuple<int, int> GetRenderSize() {
  return std::make_tuple(kDefaultViewWidth, kDefaultViewHeight);
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
 
  position.z = position.z / (1024 * 1024);
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
  glm::mat4 projection = glm::ortho(0.0f, 1.f * w, 1.f * h, 0.0f, -1.0f, 1.0f);
  unsigned int draw_id = position.z * 1024 * 1024 * 1024 * 1024 + draw_count;
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
  glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
  glViewport(0, 0, 320, 240);
  glClearColor(.0f, .0f, .0f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_count = 0;

  shader->Use();
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glActiveTexture(GL_TEXTURE0);
  shader->SetInteger("sprite", 0);

  std::sort(draw_list.begin(), draw_list.end(), 
          [](const DrawData &a, const DrawData &b){
          return std::get<0>(a) < std::get<0>(b); 
          }
          );

  for (auto [draw_id, texture_id, model, projection, color] : draw_list) {
    shader->SetMatrix4("projection", projection);
    shader->SetMatrix4("model", model);
    shader->SetVector3f("spriteColor", color);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glDisableVertexAttribArray(0);
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
  DrawAll();
  
  RefreshViewport();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(viewport_x, viewport_y, viewport_w, viewport_h);
  glClearColor(.2f, .2f, .2f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  shader_screen->Use();
  glBindBuffer(GL_ARRAY_BUFFER, screen_VBO);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glActiveTexture(GL_TEXTURE0);
  shader_screen->SetInteger("sprite", 0);
  shader_screen->SetVector3f("spriteColor", glm::vec3(1.0f));

  glBindTexture(GL_TEXTURE_2D, renderedTexture);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  window::SwapWinodw();
}

std::tuple<int, int> convertToViewport(int x, int y) {
  auto [rw, rh] = GetRenderSize();
  return std::make_tuple((x - viewport_x) * rw / viewport_w,
                         (y - viewport_y) * rh / viewport_h);
}

} // namespace renderer
} // namespace yapre
