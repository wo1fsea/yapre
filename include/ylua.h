#pragma once

namespace yapre {
namespace lua {
bool Init();
void Deinit();
void Update(int delta_ms);
}; // namespace core
}; // namespace yapre
