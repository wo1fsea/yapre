#include "yrepl.h"
#include "ycore.h"
#include "ylua.h"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace yapre {
namespace repl {

std::thread *console = nullptr;
std::string input = "";

bool Init() {
  console = new std::thread([]() {
    while (!core::ToStop()) {
      std::string tmp;
      std::getline(std::cin, tmp);
      input = tmp;
    }
  });
  return true;
}
void Deinit() {
  console->join();
  console = nullptr;
}

void Update(int delta_ms) {
  if (input != "") {
    std::cout << lua::DoString(input) << std::endl;
    input = "";
  }
}

}; // namespace repl
}; // namespace yapre
