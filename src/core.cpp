#include "core.h"
#include "audio.h"

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
            static bool a = false;
            if(!a)
            {
                yapre::audio::Beep(440., 10000);
                a = true;
            }
            SDL_Event event;
            SDL_PollEvent(&event);
            return event.type != SDL_QUIT;
        }
    } // namespace pinhole
}
