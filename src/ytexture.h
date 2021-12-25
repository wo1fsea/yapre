#pragma once

#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "stb_image.h"
#include "stb_image_resize.h"

#include "glad/glad.h"

namespace yapre {
class Texture {
private:
  std::shared_ptr<unsigned char[]> data_ptr = nullptr;
  int width = 0;
  int height = 0;
  int channel = 4;
  int real_size = 0;
  unsigned int texture_id = 0;
  bool changed = true;

public:
  Texture(const std::string &file_path) {
    int n_channel;
    int n_width, n_height;
    unsigned char *file_data =
        stbi_load(file_path.c_str(), &n_width, &n_height, &n_channel, 0);

    assert(n_channel == channel);
    width = n_width;
    height = n_height;

    int size = width > height ? width : height;
    real_size = 1;
    while (real_size < size) {
      real_size *= 2;
    }

    data_ptr = std::shared_ptr<unsigned char[]>(
        new unsigned char[real_size * real_size * channel]);

    /* no need to resize
    stbir_resize_uint8(file_data, n_width, n_height, 0, data_ptr.get(),
                       real_size, real_size, 0, channel);
                       */
    for (int x = 0; x < real_size; ++x) {
      for (int y = 0; y < real_size; ++y) {
        int r_idx = (x + real_size * y) * channel;
        if (x < width && y < height) {
          int idx = (x + width * y) * channel;
          for (int c = 0; c < channel; ++c) {
            data_ptr[r_idx + c] = file_data[idx + c];
          }
        } else {
          for (int c = 0; c < channel; ++c) {
            data_ptr[r_idx + c] = 0;
          }
        }
      }
    }
    stbi_image_free(file_data);

    glGenTextures(1, &texture_id);
  }

  inline Texture(unsigned int width_, unsigned int height_)
      : width(width_), height(height_) {
    int size = width > height ? width : height;
    real_size = 1;
    while (real_size < size) {
      real_size *= 2;
    }
    data_ptr = std::shared_ptr<unsigned char[]>(
        new unsigned char[real_size * real_size * channel]);

    glGenTextures(1, &texture_id);
  }

  ~Texture() { glDeleteTextures(1, &texture_id); }

  inline void SetPixel(unsigned int x, unsigned int y, unsigned char R,
                       unsigned char G, unsigned char B, unsigned char A) {
    unsigned int p = (x + y * real_size) * channel;
    data_ptr[p + 0] = R;
    data_ptr[p + 1] = G;
    data_ptr[p + 2] = B;
    data_ptr[p + 3] = A;
    changed = true;
  }

  void UpdateData() {
    if (!changed) {
      return;
    }

    changed = false;
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, real_size, real_size, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data_ptr.get());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  inline int Width() { return width; }
  inline int Height() { return height; }
  inline int Channel() { return channel; }
  inline int RealSize() { return real_size; }
  inline unsigned int TextureID() { return texture_id; }
  inline unsigned char *Data() { return data_ptr.get(); }
  inline void clear_changed() { changed = false; };
};
} // namespace yapre
