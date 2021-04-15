#include "yrepl.h"
#include "ycore.h"
#include "ylua.h"

#include "linenoise.hpp"

#include <memory>
#include <ostream>
#include <string>
#include <thread>

namespace yapre {
namespace repl {

std::thread *console = nullptr;
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
      // Add text to history
      linenoise::AddHistory(line.c_str());
      input = line;
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
  if (input != "") {
    std::cout << "========== lua ==========" << std::endl;
    std::cout << lua::DoString(input) << std::endl;
    std::cout << "========== lua ==========" << std::endl;
    input = "";
  }
}

}; // namespace repl
}; // namespace yapre
