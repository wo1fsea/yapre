#include "ycore.h"
#include "ytimer.h"

#include <SDL.h>
#include <iostream>
namespace yapre {
namespace core {

bool to_stop = false;
bool Init() {
  for (bool (*fptr)(void) : kInitFPtrs) {
    if (!fptr())
      return false;
  }
  return true;
}

void Deinit() {
  for (void (*fptr)(void) : kDeinitFPtrs) {
    fptr();
  }
}

void Update() {
  input::Update();
  for (void (*fptr)(void) : kUpdateFPtrs) {
    fptr();
  }
}

bool ToStop() { return to_stop; }
void SetToStop() { to_stop = true; }

} // namespace core
} // namespace yapre
