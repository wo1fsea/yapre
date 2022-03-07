#include "yrenderer.h"

#include "yfont.h"
#include "yluabind.hpp"
#include "ytexture.h"
#include "ywindow.h"

#include "SDL.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN

#if BX_PLATFORM_IOS
void *YapreSDLGetNwh(SDL_SysWMinfo wmi, SDL_Window *window);
#endif

namespace yapre {

namespace renderer {

const float kMaxZ = 1024 * 1024;

std::tuple<int, int> preferred_resolution = std::make_tuple(320, 240);
std::tuple<int, int> render_resolution = std::make_tuple(320, 240);
std::tuple<int, int> backbuffer_resolution = std::make_tuple(320, 240);

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

bgfx::VertexBufferHandle vert_buff_handler = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle idx_buff_handler = BGFX_INVALID_HANDLE;
bgfx::UniformHandle sprite_tex_sampler = BGFX_INVALID_HANDLE;
bgfx::UniformHandle sprite_color_handler = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;

bgfx::FrameBufferHandle scene_frame_buffer = BGFX_INVALID_HANDLE;

uint32_t scene_view_id = 0;
uint32_t backbuffer_view_id = 1;

uint32_t clean_color = 0xffffffff;

void UpdateRenderResolution();
void UpdateBackbufferResolution();

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
  bgfx_init.resolution.width = std::get<0>(render_resolution);
  bgfx_init.resolution.height = std::get<1>(render_resolution);
  bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
  bgfx::init(bgfx_init);

  bgfx::VertexLayout pos_col_vert_layout;
  pos_col_vert_layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
      .end();
  vert_buff_handler = bgfx::createVertexBuffer(
      bgfx::makeRef(cube_vertices, sizeof(cube_vertices)), pos_col_vert_layout);
  idx_buff_handler = bgfx::createIndexBuffer(
      bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list)));

  sprite_tex_sampler =
      bgfx::createUniform("spriteTex", bgfx::UniformType::Sampler);
  sprite_color_handler =
      bgfx::createUniform("spriteColor", bgfx::UniformType::Vec4);

  program = loadProgram("v_simple.bin", "f_simple.bin");

  SetPreferredResolution(320, 240);

  SDL_AddEventWatch(OnWindowEvent, yapre::window::mainWindow);
  return true;
}

void Deinit() { SDL_DelEventWatch(OnWindowEvent, yapre::window::mainWindow); }

std::tuple<int, int> GetPreferredResolution() { return preferred_resolution; }
std::tuple<int, int> GetRenderResolution() { return render_resolution; }
std::tuple<int, int> GetBackbufferResolution() { return backbuffer_resolution; }

void SetPreferredResolution(int width, int height) {
  preferred_resolution = std::make_tuple(width, height);
  UpdateRenderResolution();
  window::ResetWindowSize();
}

void UpdateRenderResolution() {
    if(keep_aspect){
        render_resolution = preferred_resolution;
    }
    else{
        render_resolution = backbuffer_resolution;
    }
    
    auto [w, h] = render_resolution;
 

  lua::GStateModule("yapre")
      .Define("render_width", w)
      .Define("render_height", h);

  const uint64_t tsFlags = 0 | BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT |
                           BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT |
                           BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

  if (isValid(scene_frame_buffer)) {
    bgfx::destroy(scene_frame_buffer);
  }
  scene_frame_buffer = bgfx::createFrameBuffer(
      uint16_t(w), uint16_t(h), bgfx::TextureFormat::RGBA8, tsFlags);
}

void UpdateBackbufferResolution() {
    auto [w, h] = window::GetDrawableSize();
    auto [dw, dh] = GetPreferredResolution();

    if (1.0 * dw / dh > 1.0 * w / h) {
      dh = (int)(1.0 * dw * h / w + 0.5);
    } else {
      dw = (int)(1.0 * dh * w / h + 0.5);
    }
    backbuffer_resolution = std::make_tuple(dw, dh);

  bgfx::reset(w, h, BGFX_RESET_VSYNC);
    
    UpdateRenderResolution();
}

void DrawSprite(const std::string &texture_filename,
                std::tuple<int, int, int> position, std::tuple<int, int> size,
                float rotate, std::tuple<float, float, float> color) {
  auto texture_ptr = Texture::GetFromFile(texture_filename);
  DrawSprite(texture_ptr.get(), position, size, rotate, color);
}

void DrawSprite(Texture *texture, std::tuple<int, int, int> position,
                std::tuple<int, int> size, float rotate,
                std::tuple<float, float, float> color) {
  auto [x, y, z] = position;
  auto [R, G, B] = color;
  auto r_color = std::make_tuple(R, G, B, 1.0f);
  int draw_id = std::get<2>(position) * 1024 * 1024 + draw_count;
  draw_count++;
  draw_list.emplace_back(DrawData{draw_id, std::make_tuple(x, y, draw_count),
                                  size, r_color, rotate, texture});
}

void Draw(DrawData draw_data) {
  auto texture_ptr = draw_data.texture_ptr;
  if (texture_ptr->Width() == 0 || texture_ptr->Height() == 0)
    return;

  auto [x, y, z] = draw_data.position;
  auto [width, height] = draw_data.size;
  auto [R, G, B, A] = draw_data.color;
  auto real_size = texture_ptr->RealSize();

  /*
  float cam_rotation[16];
  bx::mtxRotateXYZ(cam_rotation, 0.f, 0.f, 0.f);

  float cam_translation[16];
  bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

  float cam_transform[16];
  bx::mtxMul(cam_transform, cam_translation, cam_rotation);

  float view[16];
  bx::mtxInverse(view, cam_transform);
  */
  auto [w, h] = GetPreferredResolution();

  float model[16];
  float model_translate[16];
  float model_scale[16];

  bx::mtxTranslate(model_translate, x, y, z);
  bx::mtxScale(model_scale, width * real_size / texture_ptr->Width(),
               height * real_size / texture_ptr->Height(), 1.0f);
  bx::mtxMul(model, model_scale, model_translate);

  bgfx::setTransform(model);

  bgfx::setVertexBuffer(0, vert_buff_handler);
  bgfx::setIndexBuffer(idx_buff_handler);
  bgfx::setTexture(0, sprite_tex_sampler, texture_ptr->TextureHandler());

  float spriteColorData[4] = {R, G, B, 1.f};
  bgfx::setUniform(sprite_color_handler, spriteColorData);
  bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_DEPTH_TEST_ALWAYS |
                 BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA,
                                       BGFX_STATE_BLEND_INV_SRC_ALPHA));

  bgfx::submit(scene_view_id, program);
}

void DrawText(const std::string &text, float scale,
              std::tuple<int, int, int> position, std::tuple<int, int> area,
              std::tuple<float, float, float> color) {

  char *str = (char *)text.c_str();  // utf-8 string
  char *str_i = str;                 // string iterator
  char *end = str + strlen(str) + 1; // end iterator

  auto [a_w, a_h] = area;
  auto [o_x, o_y, o_z] = position;
  auto [c_x, c_y, c_z] = position;

  do {
    uint32_t code = utf8::next(str_i, end); // get 32 bit code of a utf-8 symbol
    if (code == 0)
      continue;

    if (code == '\n') {
      c_x = o_x;
      c_y += font::kFontSize * scale;
      if (a_h > 0 && c_y > o_y + a_h) {
        break;
      }
      continue;
    }

    int i_x = c_x;
    int i_y = c_y;
    int i_z = c_z++;

    auto char_data = font::GetCharData(code);

    i_x += char_data->bearing_x * scale;
    i_y +=
        (font::GetCharData(0x9F8D)->bearing_y - char_data->bearing_y) * scale;

    auto i_size =
        std::make_tuple(char_data->width * scale, char_data->height * scale);

    if (char_data->width > 0 && char_data->height > 0) {
      DrawSprite(char_data->texture.get(), std::make_tuple(i_x, i_y, i_z),
                 i_size, 0, color);
    }

    c_x += char_data->advance * scale;

    if (a_w > 0 && c_x > o_x + a_w) {
      c_x = o_x;
      c_y += font::kFontSize * scale;
    }

    if (a_h > 0 && c_y > o_y + a_h) {
      break;
    }
  } while (str_i < end);
}

std::tuple<int, int> CalculateTextRenderSize(const std::string &text,
                                             float scale,
                                             std::tuple<int, int> area) {
  auto [aw, ah] = area;
  int render_width = 0;
  int render_height = 0;
  int pos_x = 0;
  int pos_y = 0;

  char *str = (char *)text.c_str();  // utf-8 string
  char *str_i = str;                 // string iterator
  char *end = str + strlen(str) + 1; // end iterator

  do {
    uint32_t code = utf8::next(str_i, end); // get 32 bit code of a utf-8 symbol
    if (code == 0)
      continue;

    if (code == '\n') {
      pos_x = 0;
      pos_y += font::kFontSize * scale;
      if (ah > 0 && pos_y > ah) {
        break;
      }
      continue;
    }

    auto char_data = font::GetCharData(code);
    pos_x += char_data->advance * scale;

    if (aw > 0 && pos_x > aw) {
      pos_x = 0;
      pos_y += font::kFontSize * scale;
    }

    if (ah > 0 && pos_y > ah) {
      break;
    }

    render_width = pos_x > render_width ? pos_x : render_width;
    render_height = pos_y + font::kFontSize * scale;
  } while (str_i < end);

  return std::make_tuple(render_width, render_height);
}

void DrawScene() {
  auto [width, height] = GetRenderResolution();

    
  bgfx::setViewRect(scene_view_id, 0, 0, width, height);
  bgfx::setViewMode(scene_view_id, bgfx::ViewMode::Sequential);
  bgfx::setViewFrameBuffer(scene_view_id, scene_frame_buffer);
  bgfx::setViewClear(scene_view_id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     clean_color, 0.0f, 0);
    float proj[16];
    bx::mtxOrtho(proj, 0.f, width, height, 0.f, -kMaxZ, kMaxZ, 0, true);
    bgfx::setViewTransform(scene_view_id, NULL, proj);
    
  std::sort(draw_list.begin(), draw_list.end(),
            [](const DrawData &a, const DrawData &b) { return a.id < b.id; });

  for (const DrawData &draw_data : draw_list) {
    Draw(draw_data);
  }
  draw_list.clear();
  draw_count = 0;
}

void DrawScreen() {
    auto [w_w, w_h] = window::GetDrawableSize();
  auto [r_w, r_h] = GetRenderResolution();
  auto [b_w, b_h] = GetBackbufferResolution();
    
  float scale = b_w / r_w;

  bgfx::setViewClear(backbuffer_view_id, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                     clean_color, 0.0f, 0);
  float proj[16];
  bx::mtxOrtho(proj, 0.f, b_w, b_h, 0.f, -kMaxZ, kMaxZ, 0, true);

  bgfx::setViewTransform(backbuffer_view_id, NULL, proj);
  bgfx::setViewRect(backbuffer_view_id, 0, 0, (uint16_t)w_w, (uint16_t)w_h);

  float model[16];
  float model_translate[16];
  float model_scale[16];

  bx::mtxTranslate(model_translate, (b_w - r_w) / 2., (b_h - r_h) / 2., 0);
  bx::mtxScale(model_scale, r_w, r_h, 1.0f);
  bx::mtxMul(model, model_scale, model_translate);

  bgfx::setTransform(model);

  bgfx::setVertexBuffer(0, vert_buff_handler);
  bgfx::setIndexBuffer(idx_buff_handler);
  bgfx::setTexture(0, sprite_tex_sampler, bgfx::getTexture(scene_frame_buffer));

  float spriteColorData[4] = {1.0f, 1.f, 1.f, 1.f};
  bgfx::setUniform(sprite_color_handler, spriteColorData);
  bgfx::setState(BGFX_STATE_WRITE_RGB |
                 BGFX_STATE_DEPTH_TEST_ALWAYS |
                 BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA,
                                       BGFX_STATE_BLEND_INV_SRC_ALPHA));

  bgfx::submit(backbuffer_view_id, program);
}

void Update(int delta_ms) {
  UpdateBackbufferResolution();

  DrawScene();
  DrawScreen();

  bgfx::frame();
}

std::tuple<int, int> ConvertToViewport(int x, int y) {
  auto [w_w, w_h] = window::GetDrawableSize();
  auto [r_w, r_h] = GetRenderResolution();
  auto [b_w, b_h] = GetBackbufferResolution();
  float scale = 1.0 * w_w / b_w;

  return std::make_tuple((x / scale - (b_w - r_w) / 2),
                         (y / scale - (b_h - r_h) / 2));
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

      auto [p_w, p_h] = GetPreferredResolution();
      auto [r_w, r_h] = GetRenderResolution();
      auto [b_w, b_h] = GetBackbufferResolution();

      std::cout << p_w << "," << p_h << std::endl;
      std::cout << r_w << "," << r_h << std::endl;
      std::cout << b_w << "," << b_h << std::endl;
    }
    return 0;
  }
}

} // namespace renderer
} // namespace yapre
