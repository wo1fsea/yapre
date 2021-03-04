#include "audio.h"

#include <SDL.h>
#include <SDL_audio.h>
#include <queue>
#include <cmath>


namespace yapre
{
    namespace audio 
    {
        const int AMPLITUDE = 28000;
        const int FREQUENCY = 44100;

        struct BeepObject
        {
            double freq;
            int samplesLeft;
        };

        static double v = 0;
        static std::queue<BeepObject> beeps;

        void AudioCallback(void *_beeper, Uint8 *_stream, int _length);

        bool Init()
        {
            SDL_Init(SDL_INIT_AUDIO);
            SDL_AudioSpec desiredSpec;

            desiredSpec.freq = FREQUENCY;
            desiredSpec.format = AUDIO_S16SYS;
            desiredSpec.channels = 1;
            desiredSpec.samples = 2048;
            desiredSpec.callback = AudioCallback;
            desiredSpec.userdata = nullptr;

            SDL_AudioSpec obtainedSpec;

            // you might want to look for errors here
            SDL_OpenAudio(&desiredSpec, &obtainedSpec);

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
                BeepObject& bo = beeps.front();

                int samplesToDo = std::min(i + bo.samplesLeft, length);
                bo.samplesLeft -= samplesToDo - i;

                while (i < samplesToDo) {
                    stream[i] = AMPLITUDE * std::sin(v * 2 * M_PI / FREQUENCY);
                    i++;
                    v += bo.freq;
                }

                if (bo.samplesLeft == 0) {
                    beeps.pop();
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
            BeepObject bo;
            bo.freq = freq;
            bo.samplesLeft = duration * FREQUENCY / 1000;

            SDL_LockAudio();
            beeps.push(bo);
            SDL_UnlockAudio();
        }
    };
};

