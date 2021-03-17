#include "ywindow.h"

#include <iostream>
#include <tuple>

#include <SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace yapre {
namespace window {

const bool kFullScreen = 0;
const int kDefaultViewWidth = 800;
const int kDefaultViewHeight = 600;
const char *kWindowCaption = "yapre";
SDL_Window *mainWindow = nullptr;
SDL_GLContext mainContext;

std::tuple<int, int> GetDrawableSize() {
  int w = kDefaultViewWidth;
  int h = kDefaultViewHeight;

#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context =
      emscripten_webgl_get_current_context();
  emscripten_webgl_get_drawing_buffer_size(context, &w, &h);
#else
  if (mainWindow) {
    SDL_GL_GetDrawableSize(mainWindow, &w, &h);
  }
#endif
  return std::make_tuple(w, h);
}

std::tuple<int, int> GetDesignSize() {
  return std::make_tuple(kDefaultViewWidth, kDefaultViewHeight);
}

void PrintSdlError() {
  auto error_message = SDL_GetError();
  std::cout << error_message << std::endl;
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

bool Init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    PrintSdlError();
    return false;
  }
  atexit(SDL_Quit);

  SetupSdlGl();

  if (kFullScreen) {
    mainWindow = SDL_CreateWindow(kWindowCaption, SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, 0, 0,
                                  SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
  } else {
    mainWindow = SDL_CreateWindow(kWindowCaption, SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, kDefaultViewWidth,
                                  kDefaultViewHeight,
                                  SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    // SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  }

  if (mainWindow == NULL) {
    PrintSdlError();
    return false;
  }

  mainContext = SDL_GL_CreateContext(mainWindow);
  if (mainContext == NULL) {
    PrintSdlError();
    return false;
  }

  SDL_GL_SetSwapInterval(1);

  return true;
}

void SwapWinodw() {
  if (mainWindow) {
    SDL_GL_SwapWindow(mainWindow);
  }
}

} // namespace window
} // namespace yapre
