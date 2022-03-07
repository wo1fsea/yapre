#pragma once
#include "ytexture.h"
#include <string>
#include <tuple>

namespace yapre {
namespace renderer {
bool Init();
void Deinit();
void Update(int delta_ms);
std::tuple<int, int> GetPreferredResolution();
void SetPreferredResolution(int width, int height);
std::tuple<int, int> ConvertToViewport(int x, int y);
void DrawSprite(const std::string &texture_filename,
                std::tuple<int, int, int> position, std::tuple<int, int> size,
                float rotate, std::tuple<float, float, float> color);
void DrawSprite(Texture *texture, std::tuple<int, int, int> position,
                std::tuple<int, int> size, float rotate,
                std::tuple<float, float, float> color);
void DrawText(const std::string &text, float scale = 1.0f,
              std::tuple<int, int, int> position = {0, 0, 0},
              std::tuple<int, int> area = {-1, -1},
              std::tuple<float, float, float> color = {1.0f, 1.0f, 1.0f});
std::tuple<int, int> CalculateTextRenderSize(const std::string &, float,
                                             std::tuple<int, int>);
void SetClearColor(float R, float G, float B, float A);
void SetKeepAspect(bool keey_aspect_);
void ResetWindowSize();
} // namespace renderer
}; // namespace yapre
