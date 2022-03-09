#pragma once

#include "SDL.h"
#include <tuple>

struct SDL_Window;

namespace yapre {
namespace window {
extern SDL_Window *mainWindow;

bool Init();
void Deinit();
std::tuple<int, int> GetDrawableSize();
void ResetWindowSize();

std::tuple<void *, void *> GetPlatformData();
} // namespace window
} // namespace yapre
