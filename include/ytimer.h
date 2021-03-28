#pragma once
#include <functional>

namespace yapre {
namespace timer {
bool Init();
void Deinit();
void Update();
void AddTimer(int ms, std::function<void()> callback);
} // namespace timer
} // namespace yapre
