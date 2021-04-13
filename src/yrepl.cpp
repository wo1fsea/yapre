#include "yrepl.h"
#include "ylua.h"
#include "ycore.h"

#include <thread>
#include <memory>
#include <iostream>
#include <string>

namespace yapre {
namespace repl {

std::thread *console = nullptr;
std::string input;

bool Init()
{
    console = new std::thread(
            [](){
            while (!core::ToStop()) {
            std::cin >> input;    
            std::cout << "line: " << input << '\n';
            }
            }
            );
    return true;
}
void Deinit()
{
   console->detach(); 
   console = nullptr;
}

void Update(int delta_ms)
{
    if()
}

}; // namespace core
}; // name
