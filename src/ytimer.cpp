#include "ytimer.h"

#include "yluabind.hpp"

#include <SDL.h>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace yapre {
namespace timer {

void *callback_index = 0;
std::unordered_map<void *, std::function<void()>> callback_map;
std::vector<void *> current_timeout_keys;
std::mutex current_timeout_keys_mutex;

bool Init() {
  if (SDL_Init(SDL_INIT_TIMER)) {
    return false;
  }
  lua::GStateModule{"yapre"}.Define("AddTimer", AddTimer);
  return true;
}

void Deinit() {
  callback_map.clear();
  current_timeout_keys.clear();
}

void Update() {
  std::lock_guard<std::mutex> guard(current_timeout_keys_mutex);
  for (auto key : current_timeout_keys) {
    auto i = callback_map.find(key);
    if (i != callback_map.end()) {
      i->second();
      callback_map.erase(key);
    }
  }
  current_timeout_keys.clear();
}

uint32_t TimerCallback(uint32_t interval, void *key) {
  std::lock_guard<std::mutex> guard(current_timeout_keys_mutex);
  current_timeout_keys.emplace_back(key);
  return 0;
}

void AddTimer(int ms, const std::function<void()> &callback) {
  auto key = callback_index;
  callback_index = reinterpret_cast<char *>(callback_index) + 1;
  callback_map.emplace(key, callback);
  SDL_AddTimer(ms, TimerCallback, key);
}
} // namespace timer
} // namespace yapre
