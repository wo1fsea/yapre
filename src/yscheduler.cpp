#include "yscheduler.h"
#include "ycore.h"

#include "asio.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std::chrono_literals;

namespace yapre {
namespace scheduler {

const int kWorkerNum = 4;
const auto kRunForDeltaMs = 100ms;
std::unordered_map<ScheduleTag, std::shared_ptr<asio::io_context>> context_map;
std::vector<std::shared_ptr<std::thread>> worker_threads;

void _WorkerRun() {
  auto worker_context = context_map[ScheduleTag::WorkerTag];
  while (!core::ToStop()) {
    /*
    if (worker_context->stopped()) {
      worker_context->restart();
    }
    */
    worker_context->run_one_for(kRunForDeltaMs);
  }
}

void Init() {
  for (int tag = 0; tag < ScheduleTag::NUM_TAGS; tag++) {
    context_map.emplace(static_cast<ScheduleTag>(tag),
                        std::make_shared<asio::io_context>());
  }

  for (int i = 0; i < kWorkerNum; i++) {
    auto worker_thread = std::make_shared<std::thread>(_WorkerRun);
    worker_threads.emplace_back(worker_thread);
  }
}

void Deinit() {
  for (auto kv : context_map) {
    kv.second->stop();
  }

  for (auto thread : worker_threads) {
    thread->join();
  }
  worker_threads.clear();
  context_map.clear();
}

void Update() {
  auto main_context = context_map[ScheduleTag::MainTag];
  /*
  if (main_context->stopped()) {
    main_context->restart();
  }
  */
  main_context->run_one_for(kRunForDeltaMs);
}

void Post(ScheduleTag tag, Function function) {
  auto &context = *(context_map[tag]);
  asio::post(context, function);
}
void Dispatch(ScheduleTag tag, Function function) {
  auto &context = *(context_map[tag]);
  asio::dispatch(context, function);
}
void Defer(ScheduleTag tag, Function function) {
  auto &context = *(context_map[tag]);
  asio::defer(context, function);
}

void Wait(ScheduleTag tag, Function function,
          std::chrono::steady_clock::duration duration) {
  auto &context = *(context_map[tag]);
  auto timer = std::make_shared<asio::steady_timer>(context, duration);
  timer->async_wait([function, timer](std::error_code ec) { function(); });
}

void PostOnMain(Function function) { Post(MainTag, function); }
void DispatchOnMain(Function function) { Dispatch(MainTag, function); }
void DeferOnMain(Function function) { Defer(MainTag, function); }
void WaitOnMain(Function function,
                std::chrono::steady_clock::duration duration) {
  Wait(MainTag, function, duration);
}

void PostOnWorker(Function function) { Post(WorkerTag, function); }
void DispatchOnWorker(Function function) { Dispatch(WorkerTag, function); }
void DeferOnWorker(Function function) { Defer(WorkerTag, function); }
void WaitOnWorker(Function function,
                  std::chrono::steady_clock::duration duration) {
  Wait(WorkerTag, function, duration);
}

void _UpdateFunction(Function function,
                     std::chrono::steady_clock::duration duration,
                     std::shared_ptr<asio::steady_timer> timer) {
  if (core::ToStop())
    return;

  function();
  timer->expires_at(timer->expiry() + duration);
  timer->async_wait([function, duration, timer](std::error_code ec) {
    _UpdateFunction(function, duration, timer);
  });
}

void SetupUpdateFunctionOnMain(Function function,
                               std::chrono::steady_clock::duration duration) {

  auto &context = *(context_map[MainTag]);
  auto timer = std::make_shared<asio::steady_timer>(context, duration);
  timer->async_wait([function, duration, timer](std::error_code ec) {
    _UpdateFunction(function, duration, timer);
  });
}

}; // namespace scheduler
}; // namespace yapre
