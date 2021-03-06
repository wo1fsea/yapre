#include "audio.h"

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_timer.h>
#include <algorithm>
#include <iterator>
#include <list>
#include <cmath>
#include <iostream>
#include <type_traits>


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
            unsigned int Position;
        };

        static std::list<BeepData> beeps;

        void AudioCallback(void *_beeper, Uint8 *_stream, int _length);

        bool Init()
        {
            if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
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

            SDL_PauseAudio(0);
           PlayMario();
            return true;
        }


        void AudioCallback(void *_, Uint8 *_stream, int _length)
        {
            Sint16 *stream = (Sint16*) _stream;
            int length = _length / 2;

            Sint16* buffer = new Sint16[length];
            for(int i=0; i< length; i++)
            {
                stream[i] = 0;
            }

            for(auto &beep : beeps)
            {
                int samples = std::min(beep.Samples, length);
                for (int i = 0; i < samples; i++) {
                    buffer[i] = AMPLITUDE * std::sin(beep.Position * beep.Freq * 2 * M_PI / FREQUENCY);
                    beep.Position += 1;
                }
                beep.Samples -= samples;

		        SDL_MixAudio((Uint8*)stream, (Uint8*)buffer, samples * 2, SDL_MIX_MAXVOLUME / 4);
            } 
            
            beeps.remove_if([](const BeepData& value){ return value.Samples <= 0;});
            delete [] buffer;
        }

        void Deinit()
        {
            SDL_CloseAudio();
        }

        void Beep(double freq, int duration)
        {
            SDL_LockAudio();
            
            auto i = std::find_if(
                    beeps.begin(), 
                    beeps.end(), 
                    [freq](const BeepData& value){ return value.Freq == freq;}
                    );

            if(i == beeps.end())
            {
                beeps.insert(beeps.begin(), BeepData{freq, duration * FREQUENCY / 1000, 0});
            }
            else
            {
                i->Samples = duration * FREQUENCY / 1000;
            }

            SDL_UnlockAudio();
        }
        

        int time = 0;
        void MPInit()
        {
            time = 0;
        }
        
        Uint32 TimerCallback( Uint32 interval, void* param )
        {
            BeepData* beep = (BeepData*) param;
            Beep(beep->Freq, beep->Samples);
            delete beep;
            return 0;
        }

        void PMPlayAndWait(double freq, int duration)
        {
            SDL_AddTimer(
                    time, 
                    TimerCallback,
                    new BeepData{freq, duration, 0}
                    );
            time += duration;
        }
        
        void PMWait(int duration)
        {
            time += duration;
        }

        void PlayMario()
        {
            PMPlayAndWait(659, 125);
            PMPlayAndWait(659, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(167);
            PMPlayAndWait(523, 125);
            PMPlayAndWait(659, 125);
            PMWait(125);
            PMPlayAndWait(784, 125);
            PMWait(375);
            PMPlayAndWait(392, 125);
            PMWait(375);
            PMPlayAndWait(523, 125);
            PMWait(250);
            PMPlayAndWait(392, 125);
            PMWait(250);
            PMPlayAndWait(330, 125);
            PMWait(250);
            PMPlayAndWait(440, 125);
            PMWait(125);
            PMPlayAndWait(494, 125);
            PMWait(125);
            PMPlayAndWait(466, 125);
            PMWait(42);
            PMPlayAndWait(440, 125);
            PMWait(125);
            PMPlayAndWait(392, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(125);
            PMPlayAndWait(784, 125);
            PMWait(125);
            PMPlayAndWait(880, 125);
            PMWait(125);
            PMPlayAndWait(698, 125);
            PMPlayAndWait(784, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(125);
            PMPlayAndWait(523, 125);
            PMWait(125);
            PMPlayAndWait(587, 125);
            PMPlayAndWait(494, 125);
            PMWait(125);
            PMPlayAndWait(523, 125);
            PMWait(250);
            PMPlayAndWait(392, 125);
            PMWait(250);
            PMPlayAndWait(330, 125);
            PMWait(250);
            PMPlayAndWait(440, 125);
            PMWait(125);
            PMPlayAndWait(494, 125);
            PMWait(125);
            PMPlayAndWait(466, 125);
            PMWait(42);
            PMPlayAndWait(440, 125);
            PMWait(125);
            PMPlayAndWait(392, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(125);
            PMPlayAndWait(784, 125);
            PMWait(125);
            PMPlayAndWait(880, 125);
            PMWait(125);
            PMPlayAndWait(698, 125);
            PMPlayAndWait(784, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(125);
            PMPlayAndWait(523, 125);
            PMWait(125);
            PMPlayAndWait(587, 125);
            PMPlayAndWait(494, 125);
            PMWait(375);
            PMPlayAndWait(784, 125);
            PMPlayAndWait(740, 125);
            PMPlayAndWait(698, 125);
            PMWait(42);
            PMPlayAndWait(622, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(167);
            PMPlayAndWait(415, 125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMWait(125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMPlayAndWait(587, 125);
            PMWait(250);
            PMPlayAndWait(784, 125);
            PMPlayAndWait(740, 125);
            PMPlayAndWait(698, 125);
            PMWait(42);
            PMPlayAndWait(622, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(167);
            PMPlayAndWait(698, 125);
            PMWait(125);
            PMPlayAndWait(698, 125);
            PMPlayAndWait(698, 125);
            PMWait(625);
            PMPlayAndWait(784, 125);
            PMPlayAndWait(740, 125);
            PMPlayAndWait(698, 125);
            PMWait(42);
            PMPlayAndWait(622, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(167);
            PMPlayAndWait(415, 125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMWait(125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMPlayAndWait(587, 125);
            PMWait(250);
            PMPlayAndWait(622, 125);
            PMWait(250);
            PMPlayAndWait(587, 125);
            PMWait(250);
            PMPlayAndWait(523, 125);
            PMWait(1125);
            PMPlayAndWait(784, 125);
            PMPlayAndWait(740, 125);
            PMPlayAndWait(698, 125);
            PMWait(42);
            PMPlayAndWait(622, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(167);
            PMPlayAndWait(415, 125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMWait(125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMPlayAndWait(587, 125);
            PMWait(250);
            PMPlayAndWait(784, 125);
            PMPlayAndWait(740, 125);
            PMPlayAndWait(698, 125);
            PMWait(42);
            PMPlayAndWait(622, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(167);
            PMPlayAndWait(698, 125);
            PMWait(125);
            PMPlayAndWait(698, 125);
            PMPlayAndWait(698, 125);
            PMWait(625);
            PMPlayAndWait(784, 125);
            PMPlayAndWait(740, 125);
            PMPlayAndWait(698, 125);
            PMWait(42);
            PMPlayAndWait(622, 125);
            PMWait(125);
            PMPlayAndWait(659, 125);
            PMWait(167);
            PMPlayAndWait(415, 125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMWait(125);
            PMPlayAndWait(440, 125);
            PMPlayAndWait(523, 125);
            PMPlayAndWait(587, 125);
            PMWait(250);
            PMPlayAndWait(622, 125);
            PMWait(250);
            PMPlayAndWait(587, 125);
            PMWait(250);
            PMPlayAndWait(523, 125);
            PMWait(625);
        }
    };
};

