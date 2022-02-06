#pragma once

#include <functional>
#include <string>

namespace yapre {
namespace input {
enum KeyState : uint8_t {
  kKeyStatePressed = 1,
  kKeyStateReleased = 2,
};

enum MouseState : uint8_t {
  kMouseStatePressed = 1,
  kMouseStateReleased = 2,
  kMouseStateWheel = 3,
  kMouseStateMove = 4,
};

enum MouseButtonType : uint8_t {
  kMouseButtonLeft = 1,
  kMouseButtonRight = 2,
  kMouseButtonMiddle = 4,
};

using KeyboardCallBackFunc = std::function<void(
    uint32_t timestamp, uint8_t state, uint8_t repeat, uint8_t keycode)>;
using MouseCallBackFunc = std::function<void(
    uint32_t timestamp, uint8_t state, uint8_t button, int32_t x, int32_t y)>;
using TouchCallBackFunc = std::function<void(
    uint32_t timestamp, uint8_t state, uint8_t touchId, int32_t x, int32_t y)>;

bool Init();
void Deinit();
void Update(int delta_ms);
void BindKeyboardInputCallback(const std::string &key,
                               const KeyboardCallBackFunc& callback);
void UnbindKeyboardInputCallback(const std::string &key);

void BindMouseInputCallback(const std::string &key, const MouseCallBackFunc &callback);
void UnbindMouseInputCallback(const std::string &key);

void BindTouchInputCallback(const std::string &key, const TouchCallBackFunc &callback);
void UnbindTouchInputCallback(const std::string &key);
} // namespace input
} // namespace yapre
