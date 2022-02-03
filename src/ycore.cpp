#include "ycore.h"

#include <SDL.h>
#include <chrono>
#include <iostream>

#ifdef YAPRE_WINDOWS
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

#include "yluabind.hpp"
#include "yscheduler.h"

namespace yapre {
namespace core {

bool to_stop = false;
std::chrono::time_point<std::chrono::system_clock> last_time;

#if defined(YAPRE_WINDOWS)
const std::string platform = "windows";
#elif defined(YAPRE_MAC)
const std::string platform = "mac";
#elif defined(YAPRE_LIUNX)
const std::string platform = "linux";
#elif defined(YAPRE_ANDROID)
const std::string platform = "android";
#elif defined(YAPRE_IOS)
const std::string platform = "ios";
#elif defined(YAPRE_EMSCRIPTEN)
const std::string platform = "emscripten";
#else
const std::string platform = "unknown";
#endif

void _Update() {
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = now - last_time;
  int delta_ms = elapsed_seconds.count() * 1000;
  if (delta_ms < kMinUpdateDeltaMs) {
    return;
  }

  last_time = now;
  for (void (*fptr)(int) : kUpdateFPtrs) {
    fptr(delta_ms);
  }
}

bool Init() {
  scheduler::Init();
  scheduler::SetupUpdateFunctionOnWorker(_Update,
                                         std::chrono::milliseconds(10));

// setup data path
#ifdef YAPRE_ANDROID
  std::string data_path = SDL_AndroidGetExternalStoragePath();
  data_path += "/data";
#else
  std::string data_path = "./data";
#endif
  chdir(data_path.c_str());

  for (bool (*fptr)(void) : kInitFPtrs) {
    if (!fptr())
      return false;
  }
  last_time = std::chrono::system_clock::now();

  lua::GStateModule{"yapre"}.Define("Exit", SetToStop);
  lua::GStateModule{"yapre"}.Define("platform", platform);

  return true;
}

void Deinit() {
  scheduler::Deinit();
  for (void (*fptr)(void) : kDeinitFPtrs) {
    fptr();
  }
}

void Update() { scheduler::Update(); }

bool ToStop() { return to_stop; }
void SetToStop() { to_stop = true; }

} // namespace core
} // namespace yapre
