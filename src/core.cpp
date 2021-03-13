#include "core.h"

#include <iostream>

#include <SDL.h>

namespace yapre {
namespace core {

bool to_stop = false;
bool Init() {
  for (auto fptr : kInitFPtrs) {
    if (!fptr())
      return false;
  }
  return true;
}

void Deinit() {
  for (auto fptr : kDeinitFPtrs) {
    fptr();
  }
}

void Update() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    int l = 200;
    if (event.type == SDL_KEYDOWN) {
      std::cout << event.key.keysym.sym << "." << SDLK_KP_SPACE << std::endl;

#ifdef __EMSCRIPTEN__
      if (event.key.keysym.sym >= 30 && event.key.keysym.sym < 39) {
        event.key.keysym.sym += 19;
      } else if (event.key.keysym.sym == 39) {
        event.key.keysym.sym = 48;
      }
#endif

      switch (event.key.keysym.sym) {
      case SDLK_1:
        yapre::audio::Beep(audio::TONE::D, l);
        break;
      case SDLK_2:
        yapre::audio::Beep(audio::TONE::E, l);
        break;
      case SDLK_3:
        yapre::audio::Beep(audio::TONE::F, l);
        break;
      case SDLK_4:
        yapre::audio::Beep(audio::TONE::G, l);
        break;
      case SDLK_5:
        yapre::audio::Beep(audio::TONE::A, l);
        break;
      case SDLK_6:
        yapre::audio::Beep(audio::TONE::B, l);
        break;
      case SDLK_7:
        yapre::audio::Beep(audio::TONE::c, l);
        break;
      case SDLK_8:
        yapre::audio::Beep(audio::TONE::d, l);
        break;
      case SDLK_0:
        yapre::audio::PlayMario();
        break;
      }
    }
  }

  for (auto fptr : kUpdateFPtrs) {
    fptr();
  }
  to_stop = event.type == SDL_QUIT;
}

bool ToStop() { return to_stop; }

} // namespace core
} // namespace yapre
