#include "yrenderer_bgfx.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "yfont.h"
#include "yluabind.hpp"
#include "yshader.h"
#include "ytexture.h"
#include "ywindow.h"

#include "SDL.h"
#include "SDL_syswm.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"

#include "file-ops.h"

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

namespace renderer_bgfx {

const float kMaxZ = 1024 * 1024;

int render_width = 800;
int render_height = 6000;
bool keep_aspect = true;

unsigned int VBO = 0;
unsigned int draw_count = 0;

struct PosColorVertex {
  float x;
  float y;
  float z;
  uint32_t abgr;
};

static PosColorVertex cube_vertices[] = {
    {-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},
    {-1.0f, -1.0f, 1.0f, 0xff00ff00},  {1.0f, -1.0f, 1.0f, 0xff00ffff},
    {-1.0f, 1.0f, -1.0f, 0xffff0000},  {1.0f, 1.0f, -1.0f, 0xffff00ff},
    {-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t cube_tri_list[] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6,
    1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

static bgfx::ShaderHandle createShader(const std::string &shader,
                                       const char *name) {
  const bgfx::Memory *mem = bgfx::copy(shader.data(), shader.size());
  const bgfx::ShaderHandle handle = bgfx::createShader(mem);
  bgfx::setName(handle, name);
  return handle;
}

struct context_t {
  SDL_Window *window = nullptr;
  bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
  bgfx::VertexBufferHandle vbh = BGFX_INVALID_HANDLE;
  bgfx::IndexBufferHandle ibh = BGFX_INVALID_HANDLE;

  float cam_pitch = 0.0f;
  float cam_yaw = 0.0f;
  float rot_scale = 0.01f;

  int prev_mouse_x = 0;
  int prev_mouse_y = 0;

  int width = 0;
  int height = 0;

  bool quit = false;
};

using DrawData =
    std::tuple<unsigned int, unsigned int, glm::mat4, glm::mat4, glm::vec3>;
std::unordered_map<std::string, std::shared_ptr<Texture>> texture_map;
std::vector<DrawData> draw_list;
glm::fvec4 clean_color = glm::fvec4(0.2, 0.2, 0.2, 1);

const float default_vertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

Shader *shader_sprite = nullptr;
Shader *shader_text = nullptr;
int viewport_x = 0;
int viewport_y = 0;
int viewport_w = render_width;
int viewport_h = render_height;

context_t context;

bool Init() {

#if !BX_PLATFORM_EMSCRIPTEN
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  std::cout << "FFF" << std::endl;
  if (!SDL_GetWindowWMInfo(yapre::window::mainWindow, &wmi)) {
    std::cout << "SDL_SysWMinfo could not be retrieved. SDL_Error:"
              << SDL_GetError() << std::endl;
    return false;
  }
  bgfx::renderFrame(); // single threaded mode
#endif                 // !BX_PLATFORM_EMSCRIPTEN

  bgfx::PlatformData pd{};
#if BX_PLATFORM_WINDOWS
  pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_OSX
  pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_LINUX
  pd.ndt = wmi.info.x11.display;
  pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_EMSCRIPTEN
  pd.nwh = (void *)"#canvas";
#elif BX_PLATFORM_IOS
  pd.nwh = YapreSDLGetNwh(wmi, window);
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX ? BX_PLATFORM_LINUX ?
       // BX_PLATFORM_EMSCRIPTEN

  int width = 800;
  int height = 600;

  bgfx::Init bgfx_init;
  bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
  bgfx_init.resolution.width = width;
  bgfx_init.resolution.height = height;
  bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
  bgfx_init.platformData = pd;
  bgfx::init(bgfx_init);

  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495EDFF, 1.0f,
                     0);
  bgfx::setViewRect(0, 0, 0, width, height);

  bgfx::VertexLayout pos_col_vert_layout;
  pos_col_vert_layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
      .end();
  bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
      bgfx::makeRef(cube_vertices, sizeof(cube_vertices)), pos_col_vert_layout);
  bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
      bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list)));

  // fixme: using loadProgram
  std::string vshader;
  if (!fileops::read_file("./bgfx_shader/osx/v_simple.bin", vshader)) {
    std::cout << "1" << std::endl;
    return false;
  }

  std::string fshader;
  if (!fileops::read_file("./bgfx_shader/osx/f_simple.bin", fshader)) {
    std::cout << "2" << std::endl;
    return false;
  }

  bgfx::ShaderHandle vsh = createShader(vshader, "vshader");
  bgfx::ShaderHandle fsh = createShader(fshader, "fshader");
  bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);
  // end using loadProgram

  context.width = width;
  context.height = height;
  context.program = program;
  context.window = yapre::window::mainWindow;
  context.vbh = vbh;
  context.ibh = ibh;

  return true;
}

void Deinit() { delete shader_sprite; }

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

void SetRenderSize(int width, int height) {}

void DrawSprite(const std::string &texture_filename, glm::vec3 position,
                glm::vec2 size, float rotate, glm::vec3 color) {
  std::shared_ptr<Texture> texture_ptr;
}

void DrawSprite(const std::string &texture_filename, int x, int y, int z,
                int width, int height, float rotate, float R, float G,
                float B) {}

void DrawSprite(const std::string &texture_filename,
                std::tuple<int, int, int> position, std::tuple<int, int> size,
                float rotate, std::tuple<float, float, float> color) {}

void DrawSprite(Texture *texture, glm::vec3 position, glm::vec2 size,
                float rotate, glm::vec3 color) {}

void DrawSprite(Texture *image_data, int x, int y, int z, int width, int height,
                float rotate, float R, float G, float B) {}

void DrawSprite(Texture *image_data, std::tuple<int, int, int> position,
                std::tuple<int, int> size, float rotate,
                std::tuple<float, float, float> color) {}

void DrawText(const std::string &text, float scale, glm::vec3 position,
              glm::vec2 area, glm::vec3 color) {}

void DrawText(const std::string &text, float scale,
              std::tuple<int, int, int> position, std::tuple<int, int> area,
              std::tuple<float, float, float> color) {}

std::tuple<int, int> CalculateTextRenderSize(const std::string &text,
                                             float scale,
                                             std::tuple<int, int> area) {
  return std::make_tuple(0, 0);
}

void RefreshViewport() {}

void Update(int delta_ms) {
  float cam_rotation[16];
  bx::mtxRotateXYZ(cam_rotation, 0.f, 0.f, 0.f);

  float cam_translation[16];
  bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

  float cam_transform[16];
  bx::mtxMul(cam_transform, cam_translation, cam_rotation);

  float view[16];
  bx::mtxInverse(view, cam_transform);

  float proj[16];
  bx::mtxProj(proj, 60.0f, float(context.width) / float(context.height), 0.1f,
              100.0f, bgfx::getCaps()->homogeneousDepth);

  bgfx::setViewTransform(0, view, proj);

  float model[16];
  bx::mtxIdentity(model);
  bgfx::setTransform(model);

  bgfx::setVertexBuffer(0, context.vbh);
  bgfx::setIndexBuffer(context.ibh);

  bgfx::submit(0, context.program);

  bgfx::frame();
}

std::tuple<int, int> ConvertToViewport(int x, int y) {
  auto [rw, rh] = GetRealRenderSize();
  return std::make_tuple((x - viewport_x) * rw / viewport_w,
                         (y - viewport_y) * rh / viewport_h);
}

void SetClearColor(float R, float G, float B, float A) {
  clean_color.r = R;
  clean_color.g = G;
  clean_color.b = B;
  clean_color.a = A;
}

void SetKeepAspect(bool keey_aspect_) {}

} // namespace renderer_bgfx
} // namespace yapre
