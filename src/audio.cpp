#include "audio.h"

#include <SDL.h>
#include <SDL_audio.h>
#include <iterator>
#include <queue>
#include <cmath>
#include <iostream>


namespace yapre
{
    namespace audio 
    {
        const int AMPLITUDE = 28000;
        const int FREQUENCY = 44100;

        struct BeepData
        {
            double Freq;
            int Samples;
        };

        static double v = 0;
        static std::queue<BeepData> beeps;

        void AudioCallback(void *_beeper, Uint8 *_stream, int _length);

        bool Init()
        {
            if (SDL_Init(SDL_INIT_AUDIO)) {
                return false;
            }
            SDL_AudioSpec desiredSpec;

            desiredSpec.freq = FREQUENCY;
            desiredSpec.format = AUDIO_S16SYS;
            desiredSpec.channels = 1;
            desiredSpec.silence = 0;
            desiredSpec.samples = 2048;
            desiredSpec.callback = AudioCallback;
            desiredSpec.userdata = nullptr;

            if(SDL_OpenAudio(&desiredSpec, NULL))
            {
                return false;
            }

            // start play audio
            SDL_PauseAudio(0);
            return true;
        }


        void GenerateSamples(Sint16 *stream, int length)
        {
            int i = 0;
            while (i < length) {

                if (beeps.empty()) {
                    while (i < length) {
                        stream[i] = 0;
                        i++;
                    }
                    return;
                }
                BeepData& bo = beeps.front();

                int samplesToDo = std::min(i + bo.Samples, length);

                while (i < samplesToDo) {
                    stream[i] = AMPLITUDE * std::sin(bo.Samples * bo.Freq * 2 * M_PI / FREQUENCY);
                    i++;
                    bo.Samples -= 1;
                }

                if (bo.Samples == 0) {
                    beeps.pop();
                    std::cout << "F" << std::endl;
                }
            }
        }

        void AudioCallback(void *_beeper, Uint8 *_stream, int _length)
        {
            Sint16 *stream = (Sint16*) _stream;
            int length = _length / 2;

            GenerateSamples(stream, length);
        }

        void Deinit()
        {
            SDL_CloseAudio();
        }

        void Beep(double freq, int duration)
        {
            BeepData bo;
            bo.Freq = freq;
            bo.Samples = duration * FREQUENCY / 1000;

            SDL_LockAudio();
            beeps.push(bo);
            SDL_UnlockAudio();
        }
    };
};

