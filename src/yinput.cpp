#include "yinput.h"
#include "ycore.h"
#include "yluabind.hpp"
#include "yrenderer.h"

#include <SDL.h>
#include <unordered_map>

namespace yapre {
namespace input {
std::unordered_map<std::string, KeyboardCallBackFunc> KeyboardCallBackFuncMap;
std::unordered_map<std::string, MouseCallBackFunc> MouseCallBackFuncMap;
std::unordered_map<std::string, TouchCallBackFunc> TouchCallBackFuncMap;

bool Init() {
  lua::GStateModule{"yapre"}
  .Define("BindKeyboardInputCallback", BindKeyboardInputCallback)
  .Define("UnbindKeyboardInputCallback", UnbindKeyboardInputCallback)
  .Define("BindMouseInputCallback", BindMouseInputCallback)
  .Define("UnbindMouseInputCallback", UnbindMouseInputCallback)
  .Define("BindTouchInputCallback", BindTouchInputCallback)
  .Define("UnbindTouchInputCallback", UnbindMouseInputCallback);

  return true;
}
void Deinit() {
    KeyboardCallBackFuncMap.clear();
    MouseCallBackFuncMap.clear();
    TouchCallBackFuncMap.clear();
}

void Update() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {

    if (event.type == SDL_QUIT) {
      yapre::core::SetToStop();
      return;
    }

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
      for (auto [_, func] : KeyboardCallBackFuncMap) {
        func(event.key.timestamp,
             event.type == SDL_KEYDOWN ? kKeyStatePressed : kKeyStateReleased,
             event.key.repeat, event.key.keysym.sym);
      }
      return;
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
        auto [x, y] = renderer::convertToViewport(event.motion.x, event.motion.y);
        func(event.motion.timestamp, kMouseStateMove, buttonState,
            x,y
            );
      }
      return;
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

        func(event.button.timestamp,
             event.type == SDL_MOUSEBUTTONDOWN ? kMouseStatePressed
                                               : kMouseStateReleased,
             buttonState, event.button.x, event.button.y);
      }
      return;
    }
    if (event.type == SDL_MOUSEWHEEL) {
      for (auto [_, func] : MouseCallBackFuncMap) {
        func(event.wheel.timestamp, kMouseStateWheel, 0, event.wheel.x,
             event.wheel.y);
      }
      return;
    }
  }
}

void BindKeyboardInputCallback(KeyboardCallBackFunc callback,
                               const std::string &key) {
  KeyboardCallBackFuncMap[key] = callback;
}

void UnbindKeyboardInputCallback(const std::string &key) {
  KeyboardCallBackFuncMap.erase(key);
}

void BindMouseInputCallback(MouseCallBackFunc callback,
                            const std::string &key) {

  MouseCallBackFuncMap[key] = callback;
}
void UnbindMouseInputCallback(const std::string &key) {
  MouseCallBackFuncMap.erase(key);
}

void BindTouchInputCallback(TouchCallBackFunc callback,
                            const std::string &key) {

  TouchCallBackFuncMap[key] = callback;
}
void UnbindTouchInputCallback(const std::string &key) {
  TouchCallBackFuncMap.erase(key);
}
} // namespace input
} // namespace yapre
