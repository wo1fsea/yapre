#include "yaudio.h"
#include "ycore.h"
#include "yinput.h"
#include "yluabind.hpp"
#include "yrenderer.h"
#include "yrepl.h"
#include "yscheduler.h"

namespace yapre {
namespace lua {

void Define() {
  // core
  lua::GStateModule{"yapre"}.Define("Exit", core::SetToStop);
  lua::GStateModule{"yapre"}.Define("platform", core::platform);

  // audio
  lua::GStateModule{"yapre"}.Define("Beep", audio::Beep);

  // input
  lua::GStateModule{"yapre"}
      .Define("BindKeyboardInputCallback", input::BindKeyboardInputCallback)
      .Define("UnbindKeyboardInputCallback", input::UnbindKeyboardInputCallback)
      .Define("BindMouseInputCallback", input::BindMouseInputCallback)
      .Define("UnbindMouseInputCallback", input::UnbindMouseInputCallback)
      .Define("BindTouchInputCallback", input::BindTouchInputCallback)
      .Define("UnbindTouchInputCallback", input::UnbindMouseInputCallback);

  // renderer
  lua::GStateModule("yapre")
      .Define<void (*)(const std::string &, std::tuple<int, int, int>,
                       std::tuple<int, int>, float,
                       std::tuple<float, float, float>)>("DrawSprite",
                                                         renderer::DrawSprite)
      .Define<void (*)(Texture *, std::tuple<int, int, int>,
                       std::tuple<int, int>, float,
                       std::tuple<float, float, float>)>(
          "DrawSpriteWithImageData", renderer::DrawSprite)
      .Define("DrawText", renderer::DrawText)
      .Define<std::tuple<int, int> (*)(const std::string &, float,
                                       std::tuple<int, int>)>(
          "CalculateTextRenderSize", renderer::CalculateTextRenderSize)
      .Define("SetClearColor", renderer::SetClearColor)
      .Define("SetRenderSize", renderer::SetPreferredResolution)
      .Define("SetKeepAspect", renderer::SetKeepAspect);

  // repl
  lua::GStateModule("yapre").Define("DebugRead", repl::DebugRead);
  lua::GStateModule("yapre").Define("DebugWrite", repl::DebugWrite);

  // timer
  lua::GStateModule{"yapre"}.Define(
      "AddTimer",
      std::function([](int ms, const std::function<void()> &callback) {
        scheduler::WaitOnMain(callback, std::chrono::milliseconds(ms));
      }));

  // class
  lua::GLuaClass<Texture>("yapre", "Texture")
      .Ctor<unsigned int, unsigned int>("new")
      .Member("SetPixel", &Texture::SetPixel);
}

}; // namespace lua
}; // namespace yapre