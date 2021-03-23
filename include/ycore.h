#pragma once

#include <array>

#include "yaudio.h"
#include "ylua.h"
#include "yrenderer.h"
#include "yinput.h"

namespace yapre {
constexpr int kMaxSubsysmtemNum = 4;
using InitFPtr = bool (*)(void);
using DeinitFPtr = void (*)(void);
using UpdateFPtr = void (*)(void);

constexpr std::array<InitFPtr, kMaxSubsysmtemNum> kInitFPtrs{
    &audio::Init,
    &renderer::Init,
    &input::Init,
    &lua::Init,
};

constexpr std::array<DeinitFPtr, kMaxSubsysmtemNum> kDeinitFPtrs{
    &lua::Deinit,
    &input::Deinit,
    &audio::Deinit,
    &renderer::Deinit,
};

constexpr std::array<UpdateFPtr, kMaxSubsysmtemNum> kUpdateFPtrs{
    &input::Update,
    &lua::Update,
    &audio::Update,
    &renderer::Update,
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
