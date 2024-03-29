#include <array>

#include "yaudio.h"
#include "yinput.h"
#include "ylua.h"
#include "yrenderer.h"
#include "yrepl.h"

namespace yapre {
constexpr int kSysmtemNum = 5;

using InitFPtr = bool (*)(void);
using DeinitFPtr = void (*)(void);
using UpdateFPtr = void (*)(int);

constexpr std::array<InitFPtr, kSysmtemNum> kInitFPtrs{
    &repl::Init, &audio::Init, &renderer::Init, &input::Init, &lua::Init};

constexpr std::array<DeinitFPtr, kSysmtemNum> kDeinitFPtrs{
    &input::Deinit, &renderer::Deinit, &audio::Deinit, &lua::Deinit,
    &repl::Deinit};

constexpr std::array<UpdateFPtr, kSysmtemNum> kUpdateFPtrs{
    &input::Update, &lua::Update, &audio::Update, &renderer::Update,
    &repl::Update};

inline bool InitSystems() {
  for (bool (*fptr)(void) : kInitFPtrs) {
    if (!fptr())
      return false;
  }
  return true;
}

inline void DeinitSystems() {
  for (void (*fptr)(void) : kDeinitFPtrs) {
    fptr();
  }
}

} // namespace yapre
