#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "ycore.h"
#include "yinput.h"
#include "yluabind.hpp"
#include "yrenderer.h"

#include <SDL.h>
#include <unordered_map>

namespace yapre {
namespace input {
std::unordered_map<std::string, KeyboardCallBackFunc> KeyboardCallBackFuncMap;
std::unordered_map<std::string, MouseCallBackFunc> MouseCallBackFuncMap;
std::unordered_map<std::string, TouchCallBackFunc> TouchCallBackFuncMap;

bool Init() { return true; }
void Deinit() {
  KeyboardCallBackFuncMap.clear();
  MouseCallBackFuncMap.clear();
  TouchCallBackFuncMap.clear();
}

void Update(int delta_ms) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      yapre::core::SetToStop();
      return;
    }

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      for (auto [_, func] : KeyboardCallBackFuncMap) {
        uint32_t timestamp = 0;
#ifndef __EMSCRIPTEN__
        // fuck emscritpen! no event.key.timestamp
        timestamp = event.key.timestamp;
#endif

        func(timestamp,
             event.type == SDL_KEYDOWN ? kKeyStatePressed : kKeyStateReleased,
             event.key.repeat, event.key.keysym.sym);
      }
      continue;
    }

    if (event.type == SDL_MOUSEMOTION) {
      for (auto [_, func] : MouseCallBackFuncMap) {
        uint8_t buttonState = 0;
        if (event.motion.state & SDL_BUTTON_LMASK) {
          buttonState += kMouseButtonLeft;
        }
        if (event.motion.state & SDL_BUTTON_RMASK) {
          buttonState += kMouseButtonRight;
        }
        if (event.motion.state & SDL_BUTTON_MMASK) {
          buttonState += kMouseButtonMiddle;
        }
        auto [x, y] =
            renderer::ConvertToViewport(event.motion.x, event.motion.y);
        func(event.motion.timestamp, kMouseStateMove, buttonState, x, y);
      }
      continue;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
      for (auto [_, func] : MouseCallBackFuncMap) {
        uint8_t buttonState = 0;
        if (event.button.button == SDL_BUTTON_LEFT) {
          buttonState = kMouseButtonLeft;
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
          buttonState = kMouseButtonRight;
        } else if (event.button.button == SDL_BUTTON_MIDDLE) {
          buttonState = kMouseButtonMiddle;
        }
        auto [x, y] =
            renderer::ConvertToViewport(event.motion.x, event.motion.y);
        func(event.button.timestamp,
             event.type == SDL_MOUSEBUTTONDOWN ? kMouseStatePressed
                                               : kMouseStateReleased,
             buttonState, x, y);
      }
      continue;
    }
    if (event.type == SDL_MOUSEWHEEL) {
      for (auto [_, func] : MouseCallBackFuncMap) {
        func(event.wheel.timestamp, kMouseStateWheel, 0, event.wheel.x,
             event.wheel.y);
      }
      continue;
    }
  }
}

void BindKeyboardInputCallback(const std::string &key,
                               const KeyboardCallBackFunc &callback) {
  KeyboardCallBackFuncMap[key] = callback;
}

void UnbindKeyboardInputCallback(const std::string &key) {
  KeyboardCallBackFuncMap.erase(key);
}

void BindMouseInputCallback(const std::string &key,
                            const MouseCallBackFunc &callback) {
  MouseCallBackFuncMap[key] = callback;
}
void UnbindMouseInputCallback(const std::string &key) {
  MouseCallBackFuncMap.erase(key);
}

void BindTouchInputCallback(const std::string &key,
                            const TouchCallBackFunc &callback) {
  TouchCallBackFuncMap[key] = callback;
}
void UnbindTouchInputCallback(const std::string &key) {
  TouchCallBackFuncMap.erase(key);
}
} // namespace input
} // namespace yapre
