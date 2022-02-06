#include "yrepl.h"
#include "ycore.h"
#include "ylua.h"
#include "yluabind.hpp"

#include "linenoise.hpp"

#include <chrono>
#include <istream>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <thread>

namespace yapre {
namespace repl {

#ifdef __EMSCRIPTEN__

bool Init() { return true; }
void Deinit() {}
void Update(int delta_ms) {}

#else

std::thread *console = nullptr;
std::mutex input_mutex;
std::string input = "";

bool Init() {
  console = new std::thread([]() {
    const auto path = "history.txt";

    // Setup completion words every time when a user types
    linenoise::SetCompletionCallback(
        [](const char *editBuffer, std::vector<std::string> &completions) {
          if (editBuffer[0] == 'y') {
            completions.push_back("yapre");
            // completions.push_back("yy");
          }
        });

    // Enable the multi-line mode
    linenoise::SetMultiLine(true);

    // Set max length of the history
    linenoise::SetHistoryMaxLen(4);

    // Load history
    linenoise::LoadHistory(path);

    while (!core::ToStop()) {
      // Read line
      std::string line;
      auto quit = linenoise::Readline("", line);

      if (quit) {
        break;
      }

      {
        std::lock_guard<std::mutex> guard(input_mutex);
        // Add text to history
        linenoise::AddHistory(line.c_str());
        input = line;
      }
    }

    // Save history
    linenoise::SaveHistory(path);
  });

  return true;
}
void Deinit() {
  // console->detach();
  // console = nullptr;
}

void Update(int delta_ms) {
  if (input_mutex.try_lock()) {
    if (input != "") {
      std::cout << "========== lua ==========" << std::endl;
      std::cout << lua::DoString(input) << std::endl;
      std::cout << "========== lua ==========" << std::endl;
      input = "";
    }
    input_mutex.unlock();
  }
}

std::string DebugRead() {
  using namespace std::chrono_literals;
  while (input == "" || !input_mutex.try_lock()) {
    std::this_thread::sleep_for(100ms);
  }

  auto result = input;
  input = "";
  input_mutex.unlock();
  return result;
}

void DebugWrite(std::string data) { std::cout << data; }

#endif

}; // namespace repl
}; // namespace yapre
