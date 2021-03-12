#pragma once

#include <array>

#include "audio.h"
#include "renderer.h"

namespace yapre {
constexpr int kMaxSubsysmtemNum = 2;
using InitFPtr = bool (*)(void);
using DeinitFPtr = void (*)(void);
using UpdateFPtr = void (*)(void);

constexpr std::array<InitFPtr, kMaxSubsysmtemNum> kInitFPtrs{
    &audio::Init,
    &renderer::Init,
};

constexpr std::array<DeinitFPtr, kMaxSubsysmtemNum> kDeinitFPtrs{
    &audio::Deinit,
    &renderer::Deinit,
};

constexpr std::array<UpdateFPtr, kMaxSubsysmtemNum> kUpdateFPtrs{
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
