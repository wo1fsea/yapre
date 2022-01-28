#include "ytexture.h"
#include "stb_image.h"

namespace yapre {

Texture::Texture(const std::string &file_path) {
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

Texture::Texture(unsigned int width_, unsigned int height_)
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

Texture::~Texture() { glDeleteTextures(1, &texture_id); }

void Texture::UpdateData() {
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

} // namespace yapre
