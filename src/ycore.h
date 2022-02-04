#pragma once

#include <array>

#include "yaudio.h"
#include "yinput.h"
#include "ylua.h"
#include "yrenderer.h"
#include "yrepl.h"

namespace yapre {
constexpr int kSubsysmtemNum = 5;
constexpr int kMinUpdateDeltaMs = 30;

using InitFPtr = bool (*)(void);
using DeinitFPtr = void (*)(void);
using UpdateFPtr = void (*)(int);

constexpr std::array<InitFPtr, kSubsysmtemNum> kInitFPtrs{
    &repl::Init, &audio::Init, &renderer::Init, &input::Init, &lua::Init};

constexpr std::array<DeinitFPtr, kSubsysmtemNum> kDeinitFPtrs{
    &input::Deinit, &renderer::Deinit, &audio::Deinit, &lua::Deinit,
    &repl::Deinit};

constexpr std::array<UpdateFPtr, kSubsysmtemNum> kUpdateFPtrs{
    &input::Update, &lua::Update, &audio::Update, &renderer::Update,
    &repl::Update};

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
