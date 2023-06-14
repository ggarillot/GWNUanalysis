#pragma once

#include <atomic>
#include <string>

class Globals
{
    inline static std::atomic<unsigned int> nameCtr = 0;

  public:
    static std::string getNewName() { return "o" + std::to_string(nameCtr++); }
};