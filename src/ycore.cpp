#include "ycore.h"

#include <iostream>


namespace yapre {
namespace core {

void testK(uint32_t timestamp, uint8_t state, uint8_t repeat, uint8_t keycode)
{
    std::cout << timestamp << ", " << state << ", " << keycode << std::endl;
}
void testM(uint32_t timestamp, uint8_t state, uint8_t button, int32_t x, int32_t y)
{
    std::cout << timestamp << ", " << (int)state << ", " << (int)button << "{" << x<<", "<<y<< "}" << std::endl;
}

bool to_stop = false;
bool Init() {
  for (auto fptr : kInitFPtrs) {
    if (!fptr())
      return false;
  }
  input::BindKeyboardInputCallback(testK, "testK");
  input::BindMouseInputCallback(testM, "testM");
  return true;
}

void Deinit() {
  for (auto fptr : kDeinitFPtrs) {
    fptr();
  }
}

void Update() {
  for (auto fptr : kUpdateFPtrs) {
    fptr();
  }

}

bool ToStop() { return to_stop; }
void SetToStop()
{
    to_stop = true;
}

} // namespace core
} // namespace yapre
