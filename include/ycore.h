#pragma once

#include <array>

#include "yaudio.h"
#include "yinput.h"
#include "ylua.h"
#include "yrenderer.h"
#include "ytimer.h"
#include "yrepl.h"

namespace yapre {
constexpr int kMaxSubsysmtemNum = 6;
using InitFPtr = bool (*)(void);
using DeinitFPtr = void (*)(void);
using UpdateFPtr = void (*)(int);

constexpr std::array<InitFPtr, kMaxSubsysmtemNum> kInitFPtrs{
    &timer::Init, &audio::Init, &renderer::Init, &input::Init, &lua::Init, &repl::Init
};

constexpr std::array<DeinitFPtr, kMaxSubsysmtemNum> kDeinitFPtrs{
    &input::Deinit, &renderer::Deinit, &audio::Deinit,
    &timer::Deinit, &lua::Deinit, &repl::Deinit
};

constexpr std::array<UpdateFPtr, kMaxSubsysmtemNum> kUpdateFPtrs{
    &input::Update, &timer::Update, &lua::Update,
    &audio::Update, &renderer::Update, &repl::Update
};

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
