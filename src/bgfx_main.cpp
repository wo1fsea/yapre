#include "SDL.h"
#include "SDL_syswm.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"

#include "file-ops.h"

#include <string>

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN

#if BX_PLATFORM_IOS
void* YapreSDLGetNwh(SDL_SysWMinfo wmi, SDL_Window *window);
#endif

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

void main_loop(void *data) {
  auto context = static_cast<context_t *>(data);

  for (SDL_Event currentEvent; SDL_PollEvent(&currentEvent) != 0;) {
    if (currentEvent.type == SDL_QUIT) {
      context->quit = true;
      break;
    }
  }

  // simple input code for orbit camera
  int mouse_x, mouse_y;
  const int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
  if ((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
    int delta_x = mouse_x - context->prev_mouse_x;
    int delta_y = mouse_y - context->prev_mouse_y;
    context->cam_yaw += float(-delta_x) * context->rot_scale;
    context->cam_pitch += float(-delta_y) * context->rot_scale;
  }

  context->prev_mouse_x = mouse_x;
  context->prev_mouse_y = mouse_y;

  float cam_rotation[16];
  bx::mtxRotateXYZ(cam_rotation, context->cam_pitch, context->cam_yaw, 0.0f);

  float cam_translation[16];
  bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

  float cam_transform[16];
  bx::mtxMul(cam_transform, cam_translation, cam_rotation);

  float view[16];
  bx::mtxInverse(view, cam_transform);

  float proj[16];
  bx::mtxProj(proj, 60.0f, float(context->width) / float(context->height), 0.1f,
              100.0f, bgfx::getCaps()->homogeneousDepth);

  bgfx::setViewTransform(0, view, proj);

  float model[16];
  bx::mtxIdentity(model);
  bgfx::setTransform(model);

  bgfx::setVertexBuffer(0, context->vbh);
  bgfx::setIndexBuffer(context->ibh);

  bgfx::submit(0, context->program);

  bgfx::frame();

#if BX_PLATFORM_EMSCRIPTEN
  if (context->quit) {
    emscripten_cancel_main_loop();
  }
#endif
}

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  const int width = 800;
  const int height = 600;
  SDL_Window *window = SDL_CreateWindow(argv[0], SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, width, height,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

#if !BX_PLATFORM_EMSCRIPTEN
  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(window, &wmi)) {
    printf("SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
           SDL_GetError());
    return 1;
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

  std::string vshader;
  if (!fileops::read_file("data/bgfx_shader/ios/v_simple.bin", vshader)) {
    return 1;
  }

  std::string fshader;
  if (!fileops::read_file("data/bgfx_shader/ios/f_simple.bin", fshader)) {
    return 1;
  }

  bgfx::ShaderHandle vsh = createShader(vshader, "vshader");
  bgfx::ShaderHandle fsh = createShader(fshader, "fshader");
  bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

  context_t context;
  context.width = width;
  context.height = height;
  context.program = program;
  context.window = window;
  context.vbh = vbh;
  context.ibh = ibh;

#if BX_PLATFORM_EMSCRIPTEN
  emscripten_set_main_loop_arg(main_loop, &context, -1, 1);
#else
  while (!context.quit) {
    main_loop(&context);
  }
#endif // BX_PLATFORM_EMSCRIPTEN

  bgfx::destroy(vbh);
  bgfx::destroy(ibh);
  bgfx::destroy(program);

  bgfx::shutdown();

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
