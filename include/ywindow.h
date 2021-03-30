#pragma once

#include <tuple>

namespace yapre {
namespace window{
bool Init();
void Deinit();
std::tuple<int, int> GetDrawableSize();
void SwapWinodw();
} // namespace window_manager
} // namespace yapre
