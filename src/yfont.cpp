#include "yfont.h"

#include "ft2build.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "utf8.h"

#include <iostream>
#include <unordered_map>

#include FT_FREETYPE_H
namespace yapre {
namespace font {

FT_Library ft;
FT_Face face;
std::unordered_map<uint32_t, CharData> char_data_map;

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

CharData GetCharData(uint32_t c) {
  if (char_data_map.find(c) != char_data_map.end()) {
    return char_data_map[c];
  }

  // load first 128 characters of ASCII set
  // Load character glyph
  if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
  }
  // generate texture
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
               face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
               face->glyph->bitmap.buffer);
  // set texture options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // now store character for later use
  CharData character = {
      texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
      glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
      static_cast<unsigned int>(face->glyph->advance.x)};
      
  char_data_map.insert(std::pair<uint32_t, CharData>(c, character));
  return character;
}

}; // namespace font
}; // namespace yapre