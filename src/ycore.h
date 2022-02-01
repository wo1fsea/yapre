#pragma once

#include <array>

#include "yaudio.h"
#include "yinput.h"
#include "ylua.h"
#include "yrenderer.h"
#include "yrepl.h"
#include "ytimer.h"

namespace yapre {
constexpr int kSubsysmtemNum = 6;
constexpr int kMinUpdateDeltaMs = 30;

using InitFPtr = bool (*)(void);
using DeinitFPtr = void (*)(void);
using UpdateFPtr = void (*)(int);

constexpr std::array<InitFPtr, kSubsysmtemNum> kInitFPtrs{
    &repl::Init,     &timer::Init, &audio::Init,
    &renderer::Init, &input::Init, &lua::Init};

constexpr std::array<DeinitFPtr, kSubsysmtemNum> kDeinitFPtrs{
    &input::Deinit, &renderer::Deinit, &audio::Deinit,
    &timer::Deinit, &lua::Deinit,      &repl::Deinit};

constexpr std::array<UpdateFPtr, kSubsysmtemNum> kUpdateFPtrs{
    &input::Update, &timer::Update,    &lua::Update,
    &audio::Update, &renderer::Update, &repl::Update};

namespace core {
bool Init();
void Deinit();
void Update();
void Serialize();
void Unserialize();
bool ToStop();
void SetToStop();
}; // namespace core
}; // namespace yapre
