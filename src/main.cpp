#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL.h>
#include <SDL_main.h>
#include <glad/glad.h>
#include <iostream>

#include "yaudio.h"
#include "ycore.h"
#include "yrenderer.h"

static const int SCREEN_FULLSCREEN = 0;
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static const char *WINDOW_CAPTION = "yapre";

static SDL_Window *window = NULL;
static SDL_GLContext maincontext;

void GetDrawableSize(SDL_Window *window, int *w, int *h) {
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context =
      emscripten_webgl_get_current_context();
  emscripten_webgl_get_drawing_buffer_size(context, w, h);
#else
  SDL_GL_GetDrawableSize(window, w, h);
#endif
}

void PrintSdlError() {
  auto error_message = SDL_GetError();
  std::cout << error_message << std::endl;
}

void PrintSdlInfo() {
  std::cout << "OpenGL loaded" << std::endl;
  std::cout << "Vendor:" << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer:" << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version:" << glGetString(GL_VERSION) << std::endl;
}

void SetupSdlGl() {
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

  /*
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  */

  // Also request a depth buffer
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Default OpenGL is fine.
  SDL_GL_LoadLibrary(NULL);
}

int main(int argc, char *args[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    PrintSdlError();
    return 0;
  }
  atexit(SDL_Quit);

  SetupSdlGl();

  if (SCREEN_FULLSCREEN) {
    window = SDL_CreateWindow(WINDOW_CAPTION, SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, 0, 0,
                              SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
  } else {
    window = SDL_CreateWindow(
        WINDOW_CAPTION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    // SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  }

  if (window == NULL) {
    PrintSdlError();
    return 0;
  }

  maincontext = SDL_GL_CreateContext(window);
  if (maincontext == NULL) {
    PrintSdlError();
    return 0;
  }

  gladLoadGLLoader(SDL_GL_GetProcAddress);

  PrintSdlInfo();

  SDL_GL_SetSwapInterval(1);

  glEnable(GL_DEPTH_TEST);

  if (!yapre::core::Init()) {
    return 0;
  }

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(
      []() {
        int w, h;
        GetDrawableSize(window, &w, &h);
        glViewport(0, 0, w, h);

        yapre::core::Update();
        SDL_GL_SwapWindow(window);
      },
      0, 1);
#else
  while (!yapre::core::ToStop()) {
    int w, h;
    GetDrawableSize(window, &w, &h);
    glViewport(0, 0, w, h);
    yapre::core::Update();
    SDL_GL_SwapWindow(window);
  }
#endif

  yapre::core::Deinit();
  return 0;
}
