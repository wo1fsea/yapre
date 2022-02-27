#include "ytexture.h"

#include "bgfx/bgfx.h"
#include "stb_image.h"

#include <cassert>

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

  data_ptr.resize(real_size * real_size * channel);

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

  texture_handler = bgfx::createTexture2D(
      (uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8,
      0, bgfx::copy(data_ptr.data(), width * height * channel));
}

Texture::Texture(unsigned int width_, unsigned int height_)
    : width(width_), height(height_) {
  int size = width > height ? width : height;
  real_size = 1;
  while (real_size < size) {
    real_size *= 2;
  }
  data_ptr.resize(real_size * real_size * channel);

  for (int i = 0; i < real_size * real_size * channel; i++) {
    data_ptr[i] = 0;
  }

  texture_handler = bgfx::createTexture2D(
      (uint16_t)width, (uint16_t)height, false, 1, bgfx::TextureFormat::BGRA8,
      0, bgfx::copy(data_ptr.data(), width * height * channel));
}

Texture::~Texture() { bgfx::destroy(texture_handler); }

void Texture::UpdateData() {
  if (!changed) {
    return;
  }

  bgfx::updateTexture2D(texture_handler, 0, 0, 0, 0, (uint16_t)width,
                        (uint16_t)height,
                        bgfx::copy(data_ptr.data(), width * height * channel));
  changed = false;
}

} // namespace yapre
