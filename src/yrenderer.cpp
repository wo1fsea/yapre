#include "yrenderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include "window_manager.h"
#include "yshader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <SDL.h>

namespace yapre {
namespace renderer {

unsigned int VBO;
unsigned int TEXTURE_ID;

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

  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("data/logo.png", &width, &height, &nrChannels, 0);
  glGenTextures(1, &TEXTURE_ID);
  glBindTexture(GL_TEXTURE_2D, TEXTURE_ID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);
  return true;
}

void Deinit() { delete shader; }

void DrawSprite(glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f),
                float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f)) {

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(
      model,
      glm::vec3(position, 0.0f)); // first translate (transformations are: scale
                                  // happens first, then rotation, and then
                                  // final translation happens; reversed order)

  model = glm::translate(
      model, glm::vec3(0.5f * size.x, 0.5f * size.y,
                       0.0f)); // move origin of rotation to center of quad
  model = glm::rotate(model, glm::radians(rotate),
                      glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y,
                                          0.0f)); // move origin back

  model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

  auto [w, h] = window_manager::GetDesignSize();
  glm::mat4 projection = glm::ortho(0.0f, 1.f * w, 1.f * h, 0.0f, -1.0f, 1.0f);
  shader->SetInteger("sprite", 0);
  shader->SetMatrix4("projection", projection);
  shader->SetMatrix4("model", model);
  shader->SetVector3f("spriteColor", color);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, TEXTURE_ID);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  shader->Use();
  glEnableVertexAttribArray(0);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Update() {

  auto [w, h] = window_manager::GetDrawableSize();
  glViewport(0, 0, w, h);
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto [dw, dh] = window_manager::GetDesignSize();
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

  DrawSprite(glm::vec2(100, 0), glm::vec2(600, 600), 0.0f,
             glm::vec3(1.0f, 0.0f, 1.0f));

  window_manager::SwapWinodw();
}

} // namespace renderer
} // namespace yapre
