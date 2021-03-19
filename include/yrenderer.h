#pragma once
#include "glm/glm.hpp"
#include <string>

namespace yapre {
namespace renderer {
bool Init();
void Deinit();
void Update();
void DrawSprite(const std::string &texture_filename,
                glm::vec3 position = glm::vec3(0.0f),
                glm::vec2 size = glm::vec2(-1.0f), float rotate = 0.0f,
                glm::vec3 color = glm::vec3(1.0f));
void DrawSprite(const std::string &texture_filename, int x, int y, int z,
                int width, int height, float rotate, float R, float G,
                float B);
}// namespace renderer
};                          // namespace yapre
