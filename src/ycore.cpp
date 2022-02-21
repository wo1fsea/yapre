#include "ycore.h"

#include "ysystems.inl"

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

bool to_stop = false;

std::chrono::time_point<std::chrono::system_clock> last_time;

void _Update() {
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = now - last_time;
  int delta_ms = elapsed_seconds.count() * 1000;

  last_time = now;
  for (void (*fptr)(int) : kUpdateFPtrs) {
    fptr(delta_ms);
  }
}

inline void _SetupDataPath() {
// setup data path
#ifdef YAPRE_ANDROID
  std::string data_path = SDL_AndroidGetExternalStoragePath();
  data_path += "/data";
#else
  std::string data_path = "./data";
#endif
  chdir(data_path.c_str());
}

bool Init() {
  _SetupDataPath();

  bool result = InitSystems();
  if (!result) {
    return false;
  }

  scheduler::Init();
  scheduler::SetupUpdateFunctionOnMain(_Update, std::chrono::milliseconds(25));

  last_time = std::chrono::system_clock::now();
  return true;
}

void Deinit() {
  DeinitSystems();
  scheduler::Deinit();
}

void Update() { scheduler::Update(); }

bool ToStop() { return to_stop; }
void SetToStop() { to_stop = true; }

} // namespace core
} // namespace yapre
