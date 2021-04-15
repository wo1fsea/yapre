#pragma once
#include "glm/glm.hpp"
#include <string>
#include <tuple>

namespace yapre {
namespace renderer {
const int kDefaultViewWidth = 320;
const int kDefaultViewHeight = 240;

bool Init();
void Deinit();
void Update(int delta_ms);
std::tuple<int, int> GetRenderSize();
std::tuple<int, int> convertToViewport(int x, int y);
void DrawSprite(const std::string &texture_filename,
                glm::vec3 position = glm::vec3(0.0f),
                glm::vec2 size = glm::vec2(-1.0f), float rotate = 0.0f,
                glm::vec3 color = glm::vec3(1.0f));
void DrawSprite(const std::string &texture_filename, int x, int y, int z,
                int width, int height, float rotate, float R, float G, float B);
} // namespace renderer
}; // namespace yapre
