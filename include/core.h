#pragma once

#include <array>

#include "audio.h"
#include "renderer.h"
#include "ylua.h"

namespace yapre {
constexpr int kMaxSubsysmtemNum = 3;
using InitFPtr = bool (*)(void);
using DeinitFPtr = void (*)(void);
using UpdateFPtr = void (*)(void);

constexpr std::array<InitFPtr, kMaxSubsysmtemNum> kInitFPtrs{
    &audio::Init,
    &renderer::Init,
    &lua::Init,
};

constexpr std::array<DeinitFPtr, kMaxSubsysmtemNum> kDeinitFPtrs{
    &audio::Deinit,
    &renderer::Deinit,
    &lua::Deinit,
};

constexpr std::array<UpdateFPtr, kMaxSubsysmtemNum> kUpdateFPtrs{
    &audio::Update,
    &renderer::Update,
    &lua::Update,
};

namespace core {
bool Init();
void Deinit();
void Update();
void Serialize();
void Unserialize();
bool ToStop();
}; // namespace core
}; // namespace yapre
