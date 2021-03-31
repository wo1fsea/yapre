#include "ytimer.h"

#include "yluabind.hpp"

#include <SDL.h>
#include <functional>
#include <unordered_map>

namespace yapre {
namespace timer {

void *callback_index = 0;
std::unordered_map<void *, std::function<void()>> callback_map;

bool Init() {
  if (SDL_Init(SDL_INIT_TIMER)) {
    return false;
  }
  lua::GStateModule{"yapre"}.Define("AddTimer", AddTimer);
  return true;
}

void Deinit() {
 callback_map.clear(); 
}
void Update() {}

uint32_t TimerCallback(uint32_t interval, void *key) {
  auto i = callback_map.find(key);
  if (i != callback_map.end()) {
    i->second();
    callback_map.erase(key);
  }
  return 0;
}

void AddTimer(int ms, const std::function<void()>& callback) {
  std::cout << ms << std::endl;
  auto key = callback_index;
  callback_index = reinterpret_cast<char *>(callback_index) + 1;
  callback_map.emplace(key, callback);
  SDL_AddTimer(ms, TimerCallback, key);
}
} // namespace timer
} // namespace yapre
