#pragma once

#include <string>

namespace yapre {
namespace repl {
bool Init();
void Deinit();
void Update(int delta_ms);
std::string DebugRead();
void DebugWrite(std::string data);
}; // namespace repl
}; // namespace yapre
