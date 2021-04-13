#pragma once

#include <string>

namespace yapre {
namespace lua {
bool Init();
void Deinit();
void Update(int delta_ms);
std::string DoString(const std::string& input);
}; // namespace lua
}; // namespace yapre
