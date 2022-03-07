#include "ywindow.h"
#include "yrenderer.h"

#include <iostream>
#include <tuple>

#include "SDL.h"
#include "SDL_syswm.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace yapre {
namespace window {

const bool kFullScreen = 0;
const char *kWindowCaption = "yapre";
SDL_Window *mainWindow = nullptr;

void PrintSdlError() {
  auto error_message = SDL_GetError();
  std::cout << error_message << std::endl;
}

bool Init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    PrintSdlError();
    return false;
  }
  atexit(SDL_Quit);

  SDL_SetHint(SDL_HINT_VIDEO_EXTERNAL_CONTEXT, "1");

  auto [w, h] = renderer::GetPreferredResolution();
  if (kFullScreen) {
    mainWindow =
        SDL_CreateWindow(kWindowCaption, SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN);
  } else {
    mainWindow =
        SDL_CreateWindow(kWindowCaption, SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_RESIZABLE);
    // SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  }

  if (mainWindow == NULL) {
    PrintSdlError();
    return false;
  }

  return true;
}

void Deinit() {
  SDL_DestroyWindow(mainWindow);
  SDL_Quit();
}

std::tuple<int, int> GetDrawableSize() {
  auto [w, h] = renderer::GetPreferredResolution();

#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context =
      emscripten_webgl_get_current_context();
  emscripten_webgl_get_drawing_buffer_size(context, &w, &h);
#else
  if (mainWindow) {
    SDL_GetWindowSize(mainWindow, &w, &h);
  }
#endif
  return std::make_tuple(w, h);
}

void ResetWindowSize() {
#if defined(YPARE_WINDOWS) || defined(YPARE_MAC) || defined(YPARE_LINUX)
  if (mainWindow) {
    auto [w, h] = renderer::GetPreferredResolution();
    SDL_SetWindowSize(mainWindow, w, h);
  }
#endif
}

int OnWindowEvent(void *data, SDL_Event *event) {
  if (event->type == SDL_WINDOWEVENT &&
      event->window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_Window *win = SDL_GetWindowFromID(event->window.windowID);
    if (win == (SDL_Window *)data) {
      printf("resizing.....\n");
    }
  }
  return 0;
}

std::tuple<void *, void *> GetPlatformData() {
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);

  if (!SDL_GetWindowWMInfo(yapre::window::mainWindow, &wmi)) {
    PrintSdlError();
  }

  void *nwh = 0;
  void *ndt = 0;

#if defined(YAPRE_WINDOWS)
  nwh = wmi.info.win.window;
#elif defined(YAPRE_MAC)
  nwh = wmi.info.cocoa.window;
#elif defined(YAPRE_LIUNX)
  nwh = (void *)(uintptr_t)wmi.info.x11.window;
  ndt = wmi.info.x11.display;
#elif defined(YAPRE_ANDROID)
  nwh = wmi.info.android.window;
#elif defined(YAPRE_IOS)
  nwh = YapreSDLGetNwh(wmi, yapre::window::mainWindow);
#elif defined(YAPRE_EMSCRIPTEN)
  nwh = (void *)"#canvas";
#endif

  return std::make_tuple(nwh, ndt);
}

} // namespace window
} // namespace yapre
