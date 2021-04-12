#pragma once

namespace yapre {
namespace audio {
namespace TONE {
const double D = 293.665;
const double E = 329.628;
const double F = 349.228;
const double G = 391.995;
const double A = 440.000;
const double B = 493.883;
const double c = 554.365;
const double d = 587.330;
}; // namespace TONE

bool Init();
void Deinit();
void Update(int delta_ms);
void Beep(double freq, int duration);
void PlayMario();
}; // namespace audio
}; // namespace yapre
