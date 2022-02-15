#pragma once

#include <tuple>
#include <SDL.h>

namespace yapre {
namespace window{
extern SDL_Window *mainWindow;
bool Init();
void Deinit();
std::tuple<int, int> GetDrawableSize();
void SwapWinodw();
void ResetWindowSize();
} // namespace window_manager
} // namespace yapre
