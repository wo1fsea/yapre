#include "yrenderer.h"

#include "yfont.h"
#include "yluabind.hpp"
#include "ytexture.h"
#include "ywindow.h"

#include "SDL.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN

#if BX_PLATFORM_IOS
void *YapreSDLGetNwh(SDL_SysWMinfo wmi, SDL_Window *window);
#endif

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace yapre {

namespace renderer {

const float kMaxZ = 1024 * 1024;

int render_width = 320;
int render_height = 240;
bool keep_aspect = true;
unsigned int draw_count = 0;

struct PosColorVertex {
  float x;
  float y;
  float z;
  float u;
  float v;
};

static PosColorVertex cube_vertices[] = {
    {0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    // 0{0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
    // 1{1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
};

static const uint16_t cube_tri_list[] = {
    0, 1, 2, 0, 3, 1,
};

bgfx::ShaderHandle loadShader(const std::string &name) {
#if BX_PLATFORM_WINDOWS
  std::string shaderPath = "./bgfx_shader/windows/";
#elif BX_PLATFORM_OSX
  std::string shaderPath = "./bgfx_shader/osx/";
#elif BX_PLATFORM_LINUX
  std::string shaderPath = "./bgfx_shader/linux/";
#elif BX_PLATFORM_EMSCRIPTEN
  std::string shaderPath = "./bgfx_shader/asmjs/";
#elif BX_PLATFORM_IOS
  std::string shaderPath = "./bgfx_shader/ios/";
#elif BX_PLATFORM_ANDROID
  std::string shaderPath = "./bgfx_shader/android/";
#endif

  std::stringstream shaderStream;
  std::ifstream shaderFile(shaderPath + name);
  shaderStream << shaderFile.rdbuf();
  shaderFile.close();
  std::string shaderFileContext = shaderStream.str();
  const bgfx::Memory *mem =
      bgfx::copy(shaderFileContext.data(), shaderFileContext.size());
  const bgfx::ShaderHandle handle = bgfx::createShader(mem);
  bgfx::setName(handle, name.c_str());
  return handle;
}

bgfx::ProgramHandle loadProgram(const std::string &vsName,
                                const std::string &psName) {
  bgfx::ShaderHandle vsh = loadShader(vsName);
  bgfx::ShaderHandle fsh = loadShader(psName);
  bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);
  return program;
}

struct DrawData {
  int id = 0;
  std::tuple<int, int, int> position;
  std::tuple<int, int> size;
  std::tuple<float, float, float, float> color;
  float rotate;
  Texture *texture_ptr;
};

std::vector<DrawData> draw_list;

bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;
bgfx::UniformHandle sampler = BGFX_INVALID_HANDLE;
bgfx::UniformHandle spriteColor = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;

uint32_t clean_color = 0xffffffff;

int viewport_x = 0;
int viewport_y = 0;
int viewport_w = render_width;
int viewport_h = render_height;

inline bgfx::PlatformData _GetPlatformData() {
  bgfx::PlatformData pd{};
  std::tie(pd.nwh, pd.ndt) = window::GetPlatformData();
  return pd;
}

inline bgfx::RendererType::Enum _GetRendererType() {
#if BX_PLATFORM_WINDOWS
  return bgfx::RendererType::OpenGL;
#elif BX_PLATFORM_OSX
  return bgfx::RendererType::Metal;
#elif BX_PLATFORM_LINUX
  return bgfx::RendererType::OpenGL;
#elif BX_PLATFORM_EMSCRIPTEN
  return bgfx::RendererType::OpenGLES;
#elif BX_PLATFORM_IOS
  return bgfx::RendererType::Metal;
#elif BX_PLATFORM_ANDROID
  return bgfx::RendererType::OpenGLES;
#endif
  return bgfx::RendererType::Count;
}

int OnWindowEvent(void *data, SDL_Event *event);

bool Init() {
#if !BX_PLATFORM_EMSCRIPTEN
  bgfx::renderFrame(); // single threaded mode
#endif                 // !BX_PLATFORM_EMSCRIPTEN

  bgfx::Init bgfx_init;
  bgfx_init.type = _GetRendererType();
  bgfx_init.platformData = _GetPlatformData();
  bgfx_init.resolution.width = render_width;
  bgfx_init.resolution.height = render_height;
  bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
  bgfx::init(bgfx_init);

  bgfx::VertexLayout pos_col_vert_layout;
  pos_col_vert_layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
  vbh = bgfx::createVertexBuffer(
      bgfx::makeRef(cube_vertices, sizeof(cube_vertices)), pos_col_vert_layout);
  ibh = bgfx::createIndexBuffer(
      bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list)));

  sampler = bgfx::createUniform("spriteTex", bgfx::UniformType::Sampler);
  spriteColor = bgfx::createUniform("spriteColor", bgfx::UniformType::Vec4);

  program = loadProgram("v_simple.bin", "f_simple.bin");

  SDL_AddEventWatch(OnWindowEvent, yapre::window::mainWindow);
  SetRenderSize(320, 240);
  return true;
}

void Deinit() { SDL_DelEventWatch(OnWindowEvent, yapre::window::mainWindow); }

std::tuple<int, int> GetPreferRenderSize() {
  return std::make_tuple(render_width, render_height);
}

std::tuple<int, int> GetRealRenderSize() {
  auto [w, h] = window::GetDrawableSize();
  int dw = render_width;
  int dh = render_height;

  if (!keep_aspect) {
    if (1.0 * dw / dh > 1.0 * w / h) {
      dh = (int)(1.0 * dw * h / w + 0.5);
    } else {
      dw = (int)(1.0 * dh * w / h + 0.5);
    }
  }
  return std::make_tuple(dw, dh);
}

void _UpdateRenderSize(int width, int height) {
  lua::GStateModule("yapre")
      .Define("render_width", width)
      .Define("render_height", height);
}

void SetRenderSize(int width, int height) {
  render_width = width;
  render_height = height;
  _UpdateRenderSize(width, height);

  window::ResetWindowSize();

  auto [w, h] = GetRealRenderSize();
  bgfx::reset(w, h, BGFX_RESET_VSYNC);
  bgfx::setViewRect(0, 0, 0, w, h);
}

void DrawSprite(const std::string &texture_filename, int x, int y, int z,
                int width, int height, float rotate, float R, float G,
                float B) {
  DrawSprite(texture_filename, std::make_tuple(x, y, z),
             std::make_tuple(width, height), rotate, std::make_tuple(R, G, B));
}

void DrawSprite(const std::string &texture_filename,
                std::tuple<int, int, int> position, std::tuple<int, int> size,
                float rotate, std::tuple<float, float, float> color) {
  auto texture_ptr = Texture::GetFromFile(texture_filename);
  DrawSprite(texture_ptr.get(), position, size, rotate, color);
}

void DrawSprite(Texture *texture, int x, int y, int z, int width, int height,
                float rotate, float R, float G, float B) {
  DrawSprite(texture, std::make_tuple(x, y, z), std::make_tuple(width, height),
             rotate, std::make_tuple(R, G, B));
}

void DrawSprite(Texture *texture, std::tuple<int, int, int> position,
                std::tuple<int, int> size, float rotate,
                std::tuple<float, float, float> color) {
  auto [x, y, z] = position;
  auto [R, G, B] = color;
  auto r_color = std::make_tuple(R, G, B, 1.0f);
  int draw_id = std::get<2>(position) * 1024 * 1024 + draw_count;
  draw_count++;
  draw_list.emplace_back(DrawData{0, std::make_tuple(x, y, draw_count), size,
                                  r_color, rotate, texture});
}

void Draw(DrawData draw_data) {
  auto [x, y, z] = draw_data.position;
  auto [width, height] = draw_data.size;
  auto [R, G, B, A] = draw_data.color;
  auto real_size = draw_data.texture_ptr->RealSize();

  draw_count += 1;

  float cam_rotation[16];
  bx::mtxRotateXYZ(cam_rotation, 0.f, 0.f, 0.f);

  float cam_translation[16];
  bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

  float cam_transform[16];
  bx::mtxMul(cam_transform, cam_translation, cam_rotation);

  float view[16];
  bx::mtxInverse(view, cam_transform);

  float proj[16];
  auto [w, h] = GetRealRenderSize();
  bx::mtxOrtho(proj, 0.f, w, h, 0.f, -kMaxZ, kMaxZ, 0, true);

  bgfx::setViewTransform(0, view, proj);
  bgfx::setViewRect(0, 0, 0, (uint16_t)w, (uint16_t)h);

  float model[16];
  float model_translate[16];
  float model_scale[16];

  bx::mtxTranslate(model_translate, x, y, z);
  bx::mtxScale(model_scale, width * real_size / draw_data.texture_ptr->Width(),
               height * real_size / draw_data.texture_ptr->Height(), 1.0f);
  bx::mtxMul(model, model_scale, model_translate);

  bgfx::setTransform(model);

  bgfx::setVertexBuffer(0, vbh);
  bgfx::setIndexBuffer(ibh);
  bgfx::setTexture(0, sampler, draw_data.texture_ptr->TextureHandler());

  float spriteColorData[4] = {R, G, B, 1.f};
  bgfx::setUniform(spriteColor, spriteColorData);
  bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z |
                 BGFX_STATE_DEPTH_TEST_ALWAYS |
                 BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA,
                                       BGFX_STATE_BLEND_INV_SRC_ALPHA));
  bgfx::setViewMode(0, bgfx::ViewMode::Sequential);
  bgfx::submit(0, program);
}

void DrawText(const std::string &text, float scale,
              std::tuple<int, int, int> position, std::tuple<int, int> area,
              std::tuple<float, float, float> color) {}

std::tuple<int, int> CalculateTextRenderSize(const std::string &text,
                                             float scale,
                                             std::tuple<int, int> area) {
  return std::make_tuple(0, 0);
}

void RefreshViewport() {
  auto [w, h] = window::GetDrawableSize();
  auto [dw, dh] = GetRealRenderSize();
  int rx = 0;
  int ry = 0;

  if (1.0 * dw / dh > 1.0 * w / h) {
    int rh = w * dh / dw;
    ry = (h - rh) / 2;
    h = rh;
  } else {
    int rw = h * dw / dh;
    rx = (w - rw) / 2;
    w = rw;
  }

  _UpdateRenderSize(dw, dh);

  viewport_x = rx;
  viewport_y = ry;
  viewport_w = w;
  viewport_h = h;
}

void Update(int delta_ms) {
  draw_count = 0;

  std::sort(draw_list.begin(), draw_list.end(),
            [](const DrawData &a, const DrawData &b) { return a.id < b.id; });

  auto [w, h] = window::GetDrawableSize();
  lua::GStateModule("yapre")
      .Define("drawable_width", w)
      .Define("drawable_height", h);

  RefreshViewport();
  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clean_color, 0.0f,
                     0);

  for (auto draw_data : draw_list) {
    Draw(draw_data);
  }

  draw_list.clear();
  bgfx::frame();
  draw_count = 0;
}

std::tuple<int, int> ConvertToViewport(int x, int y) {
  auto [rw, rh] = GetRealRenderSize();
  return std::make_tuple((x - viewport_x) * rw / viewport_w,
                         (y - viewport_y) * rh / viewport_h);
}

void SetClearColor(float R, float G, float B, float A) {
  clean_color = (int)(255 * R);
  clean_color = (clean_color << 8) + (int)(255 * G);
  clean_color = (clean_color << 8) + (int)(255 * B);
  clean_color = (clean_color << 8) + (int)(255 * A);
}

void SetKeepAspect(bool keey_aspect_) { keep_aspect = keey_aspect_; }

int OnWindowEvent(void *data, SDL_Event *event) {
  if (event->type == SDL_WINDOWEVENT &&
      event->window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_Window *win = SDL_GetWindowFromID(event->window.windowID);
    if (win == (SDL_Window *)data) {
      auto [w, h] = yapre::window::GetDrawableSize();
      SetRenderSize(w, h);
      std::cout << w << "," << h << std::endl;
    }
  }
  return 0;
}

} // namespace renderer
} // namespace yapre
