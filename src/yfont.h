#pragma once

#include "ft2build.h"
#include "glm/glm.hpp"
#include "utf8.h"

#include <unordered_map>

#include FT_FREETYPE_H
namespace yapre {
namespace font {

const int kFontSize = 12;
struct CharData {
  unsigned int TextureID; // ID handle of the glyph texture
  glm::ivec2 Size;        // Size of glyph
  glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Horizontal offset to advance to next glyph
};
bool Init();
CharData GetCharData(uint32_t c);

}; // namespace font
}; // namespace yapre