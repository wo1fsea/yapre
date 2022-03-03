#include "ytexture.h"

#include "bgfx/bgfx.h"
#include "stb_image.h"

#include <cassert>

namespace yapre {

std::unordered_map<std::string, std::shared_ptr<Texture>>
    Texture::texture_cache;

std::shared_ptr<Texture> Texture::GetFromFile(const std::string &file_path) {
  std::shared_ptr<Texture> texture_ptr;
  auto i = Texture::texture_cache.find(file_path);
  if (i != Texture::texture_cache.end()) {
    texture_ptr = i->second;
  } else {
    texture_ptr = std::make_shared<Texture>(file_path);
    Texture::texture_cache[file_path] = texture_ptr;
  }
  return texture_ptr;
}

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
      (uint16_t)real_size, (uint16_t)real_size, false, 1,
      bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_POINT,
      bgfx::makeRef(data_ptr.data(), data_ptr.size()));
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
      (uint16_t)real_size, (uint16_t)real_size, false, 1,
      bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_POINT,
      bgfx::makeRef(data_ptr.data(), data_ptr.size()));
}

Texture::~Texture() { bgfx::destroy(texture_handler); }

} // namespace yapre
