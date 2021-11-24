#pragma once
#include "glm/glm.hpp"
#include "ytexture.h"
#include <string>
#include <tuple>

namespace yapre {
namespace renderer {
bool Init();
void Deinit();
void Update(int delta_ms);
std::tuple<int, int> GetRenderSize();
void SetRenderSize(int width, int height);
std::tuple<int, int> ConvertToViewport(int x, int y);
void DrawSprite(const std::string &texture_filename,
                glm::vec3 position = glm::vec3(0.0f),
                glm::vec2 size = glm::vec2(-1.0f), float rotate = 0.0f,
                glm::vec3 color = glm::vec3(1.0f));
void DrawSprite(const std::string &texture_filename, int x, int y, int z,
                int width, int height, float rotate, float R, float G, float B);
void DrawSprite(Texture *texture, glm::vec3 position = glm::vec3(0.0f),
                glm::vec2 size = glm::vec2(-1.0f), float rotate = 0.0f,
                glm::vec3 color = glm::vec3(1.0f));
void DrawSprite(Texture *texture, int x, int y, int z, int width, int height,
                float rotate, float R, float G, float B);
void SetClearColor(float R, float G, float B, float A);
void ResetWindowSize();
} // namespace renderer
}; // namespace yapre
