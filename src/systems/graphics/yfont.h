#pragma once

#include "ytexture.h"

#include "ft2build.h"
#include "glm/glm.hpp"
#include "utf8.h"

#include <unordered_map>

#include FT_FREETYPE_H
namespace yapre {
namespace font {
struct CharData {
  std::shared_ptr<Texture> texture; // ID handle of the glyph texture
  int width;
  int height;
  int bearing_x;
  int bearing_y;
  int advance; // Horizontal offset to advance to next glyph
};
const int kFontSize = 12;
std::shared_ptr<CharData> GetCharData(uint32_t c);

}; // namespace font
}; // namespace yapre