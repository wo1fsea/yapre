#include "yfont.h"
#include "ytexture.h"

#include "ft2build.h"
#include "glm/glm.hpp"
#include "utf8.h"

#include <iostream>
#include <memory>
#include <unordered_map>

#include FT_FREETYPE_H
namespace yapre {
namespace font {

FT_Library ft;
FT_Face face;
std::unordered_map<uint32_t, std::shared_ptr<CharData>> char_data_map;

bool Init() {
  // FreeType
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;
    return false;
  }

  // load font as face
  if (FT_New_Face(ft, "./font/zpix.ttf", 0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    return false;
  }

  // set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, kFontSize);

  return true;
}

std::shared_ptr<CharData> GetCharData(uint32_t c) {
  static bool inited = false;
  if (!inited) {
    Init();
    inited = true;
  }

  if (char_data_map.find(c) != char_data_map.end()) {
    return char_data_map[c];
  }

  // load first 128 characters of ASCII set
  // Load character glyph
  if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
  }

  int w = face->glyph->bitmap.width;
  int h = face->glyph->bitmap.rows;
  int l = face->glyph->bitmap_left;
  int t = face->glyph->bitmap_top;
  int a = static_cast<unsigned int>(face->glyph->advance.x) >> 6;

  unsigned char *buffer = face->glyph->bitmap.buffer;

  auto char_texture = std::make_shared<Texture>(w, h);

  for (int x = 0; x < w; ++x) {
    for (int y = 0; y < h; ++y) {
      char_texture->SetPixel(x, y, 255, 255, 255, buffer[x + w * y]);
    }
  }

  char_data_map.emplace(
      c, std::shared_ptr<CharData>(new CharData{char_texture, w, h, l, t, a}));
  return char_data_map[c];
}

}; // namespace font
}; // namespace yapre