#include "ycore.h"
#include "ytimer.h"

#include <SDL.h>
#include <chrono>
#include <iostream>

namespace yapre {
namespace core {

bool to_stop = false;
std::chrono::time_point<std::chrono::system_clock> last_time;

bool Init() {
  for (bool (*fptr)(void) : kInitFPtrs) {
    if (!fptr())
      return false;
  }
  last_time = std::chrono::system_clock::now();
  return true;
}

void Deinit() {
  for (void (*fptr)(void) : kDeinitFPtrs) {
    fptr();
  }
}

void Update() {
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = now - last_time;
  int delta_ms = elapsed_seconds.count() * 1000;
  last_time = now;

  for (void (*fptr)(int) : kUpdateFPtrs) {
    fptr(delta_ms);
  }
}

bool ToStop() { return to_stop; }
void SetToStop() { to_stop = true; }

} // namespace core
} // namespace yapre
