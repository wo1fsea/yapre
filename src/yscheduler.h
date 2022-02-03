#pragma once

#include <chrono>
#include <functional>

namespace yapre {
namespace scheduler {

using Function = std::function<void(void)>;
enum ScheduleTag : unsigned int { MainTag = 0, WorkerTag, NUM_TAGS };

void Init();
void Deinit();
void Update();
void Post(ScheduleTag tag, Function function);
void Dispatch(ScheduleTag tag, Function function);
void Defer(ScheduleTag tag, Function function);
void Wait(ScheduleTag tag, Function function,
          std::chrono::steady_clock::duration duration);

void PostOnMain(Function function);
void DispatchOnMain(Function function);
void DeferOnMain(Function function);
void WaitOnMain(Function function,
                std::chrono::steady_clock::duration duration);

void PostOnWorker(Function function);
void DispatchOnWorker(Function function);
void DeferOnWorker(Function function);
void WaitOnWorker(Function function,
                  std::chrono::steady_clock::duration duration);

void SetupUpdateFunctionOnWorker(Function function,
                                 std::chrono::steady_clock::duration duration);
}; // namespace scheduler
}; // namespace yapre
