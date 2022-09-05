// Global variables used by C API

#pragma once

#include "./simulator/sim_yis.h"

#include <memory>
#include <string>

namespace GLOBAL
{
   // global var to store binary code after assembly
   extern std::string binCode;
   // global simulator output
   extern std::shared_ptr<IO::OutputInterface> simOut;

   class SimSingleton
   {
   public:
      static SIM::Yis *getInstance();

   private:
      SimSingleton() = delete;
      SimSingleton(const SimSingleton &) = delete;
      SimSingleton &operator=(const SimSingleton &) = delete;
   };
}
