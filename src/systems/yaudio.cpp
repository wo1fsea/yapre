#include "yaudio.h"

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_timer.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <list>
#include <type_traits>

#include "yluabind.hpp"

namespace yapre {
namespace audio {
const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;

struct BeepData {
  double Freq;
  int Samples;
  unsigned int Position;
};

static std::list<BeepData> beeps;

void AudioCallback(void *_beeper, Uint8 *_stream, int _length);

bool Init() {
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

  if (SDL_OpenAudio(&desiredSpec, NULL)) {
    return false;
  }

  SDL_PauseAudio(0);

  return true;
}

void AudioCallback(void *_, Uint8 *_stream, int _length) {
  Sint16 *stream = (Sint16 *)_stream;
  int length = _length / 2;

  Sint16 *buffer = new Sint16[length];
  for (int i = 0; i < length; i++) {
    stream[i] = 0;
  }

  for (auto &beep : beeps) {
    int samples = std::min(beep.Samples, length);
    for (int i = 0; i < samples; i++) {
      buffer[i] = AMPLITUDE *
                  std::sin(beep.Position * beep.Freq * 2 * M_PI / FREQUENCY);
      beep.Position += 1;
    }
    beep.Samples -= samples;

    SDL_MixAudio((Uint8 *)stream, (Uint8 *)buffer, samples * 2,
                 SDL_MIX_MAXVOLUME / 4);
  }

  beeps.remove_if([](const BeepData &value) { return value.Samples <= 0; });
  delete[] buffer;
}

void Deinit() { SDL_CloseAudio(); }
void Update(int delta_ms) {}

void Beep(double freq, int duration) {
  SDL_LockAudio();

  auto i =
      std::find_if(beeps.begin(), beeps.end(), [freq](const BeepData &value) {
        return value.Freq == freq;
      });

  if (i == beeps.end()) {
    beeps.insert(beeps.begin(), BeepData{freq, duration * FREQUENCY / 1000, 0});
  } else {
    i->Samples = duration * FREQUENCY / 1000;
  }

  SDL_UnlockAudio();
}

}; // namespace audio
}; // namespace yapre
