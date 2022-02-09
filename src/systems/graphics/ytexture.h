#pragma once

#include <memory>
#include <string>
#include <vector>

#include "glad/glad.h"

namespace yapre {
class Texture {
private:
  std::vector<unsigned char> data_ptr;
  int width = 0;
  int height = 0;
  int channel = 4;
  int real_size = 0;
  unsigned int texture_id = 0;
  bool changed = true;

public:
  Texture(const std::string &file_path);
  Texture(unsigned int width_, unsigned int height_);
  ~Texture();
  void UpdateData();

  inline void SetPixel(unsigned int x, unsigned int y, unsigned char R,
                       unsigned char G, unsigned char B, unsigned char A) {
    unsigned int p = (x + y * real_size) * channel;
    data_ptr[p + 0] = R;
    data_ptr[p + 1] = G;
    data_ptr[p + 2] = B;
    data_ptr[p + 3] = A;
    changed = true;
  }
  inline int Width() { return width; }
  inline int Height() { return height; }
  inline int Channel() { return channel; }
  inline int RealSize() { return real_size; }
  inline unsigned int TextureID() { return texture_id; }
  inline unsigned char *Data() { return data_ptr.data(); }
  inline void ClearChanged() { changed = false; };
};
} // namespace yapre
