#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL_main.h>

#include "core.h"


int main(int argc, char *args[]) {
  if (!yapre::core::Init()) {
    return 1;
  }

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop([](){yapre::core::Update();}, 60, 1);
  //yapre::core::Update();
#else
  while (pyapre::core::Update()) {

  } 
#endif

  return 0;
}

