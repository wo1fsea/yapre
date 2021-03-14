#pragma once

#include <tuple>

namespace yapre {
namespace window_manager {
bool Init();
std::tuple<int, int> GetDrawableSize();
std::tuple<int, int> GetDesignSize();
void SwapWinodw();
} // namespace window_manager
} // namespace yapre
