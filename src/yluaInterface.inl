#include "yaudio.h"
#include "yinput.h"
#include "yluabind.hpp"
#include "yrenderer.h"
#include "yrepl.h"
#include "ytimer.h"

namespace yapre {
namespace lua {
void Define() {
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

  lua::GStateModule("yapre")
      .Define<void (*)(const std::string &, std::tuple<int, int, int>,
                       std::tuple<int, int>, float,
                       std::tuple<float, float, float>)>("DrawSprite",
                                                         renderer::DrawSprite)
      .Define<void (*)(Texture *, std::tuple<int, int, int>,
                       std::tuple<int, int>, float,
                       std::tuple<float, float, float>)>(
          "DrawSpriteWithImageData", renderer::DrawSprite)
      .Define<void (*)(const std::string &, float, std::tuple<int, int, int>,
                       std::tuple<int, int>, std::tuple<float, float, float>)>(
          "DrawText", renderer::DrawText)
      .Define<std::tuple<int, int> (*)(const std::string &, float,
                                       std::tuple<int, int>)>(
          "CalculateTextRenderSize", renderer::CalculateTextRenderSize)
      .Define("SetClearColor", renderer::SetClearColor)
      .Define("SetRenderSize", renderer::SetRenderSize)
      .Define("SetKeepAspect", renderer::SetKeepAspect);

  lua::GLuaClass<Texture>("yapre", "Texture")
      .Ctor<unsigned int, unsigned int>("new")
      .Member("SetPixel", &Texture::SetPixel);

  lua::GStateModule("yapre").Define("DebugRead", repl::DebugRead);
  lua::GStateModule("yapre").Define("DebugWrite", repl::DebugWrite);
  lua::GStateModule{"yapre"}.Define("AddTimer", timer::AddTimer);
}
}; // namespace lua
}; // namespace yapre