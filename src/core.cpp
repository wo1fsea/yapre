#include "core.h"

#define GLM_FORCE_RADIANS 1

#include <iostream>

#include <SDL.h>

namespace yapre
{
    namespace core {
        bool Init() {
           return true;
        }

        void Deinit() {}

        bool Update() {
            SDL_Event event;
            SDL_PollEvent(&event);

            return event.type != SDL_QUIT;
        }
    } // namespace pinhole
}
