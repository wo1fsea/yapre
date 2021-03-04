#pragma once

namespace yapre
{
    namespace audio 
    {
        bool Init();
        void Deinit();
        void Beep(double freq, int duration);
    };
};

