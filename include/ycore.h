#pragma once

#include <array>

#include "yaudio.h"
#include "ylua.h"
#include "yrenderer.h"

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
    &lua::Deinit,
    &audio::Deinit,
    &renderer::Deinit,
};

constexpr std::array<UpdateFPtr, kMaxSubsysmtemNum> kUpdateFPtrs{
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
}; // namespace core
}; // namespace yapre
