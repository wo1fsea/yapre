#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "core.h"

int main(int argc, char *args[]) {
  if (!yapre::core::Init()) {
    return 1;
  }

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(yapre::core::update, 60, 1);
#else
  while (yapre::core::Update()) {

  } 
#endif

  return 0;
}

