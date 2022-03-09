#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL_main.h>

#include "ycore.h"
#include "ywindow.h"

int main(int argc, char *args[]) {
  if (!yapre::window::Init()) {
    return 0;
  }

  if (!yapre::core::Init()) {
    return 0;
  }

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(yapre::core::Update, 0, 1);
#else
  while (!yapre::core::ToStop()) {
    yapre::core::Update();
  }

  yapre::core::Deinit();
  yapre::window::Deinit();
#endif

  std::exit(0);
  return 0;
}
