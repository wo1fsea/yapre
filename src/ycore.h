#pragma once

#include <string>

namespace yapre {
namespace core {
extern const std::string platform;

bool Init();
void Deinit();
void Update();
void Serialize();
void Unserialize();
bool ToStop();
void SetToStop();
}; // namespace core
}; // namespace yapre
