// Global variables used by C API

#pragma once

#include "./simulator/sim_yis.h"

#include <memory>
#include <string>
#include <vector>

namespace GLOBAL
{
   // global var to store binary code after assembly
   extern std::string g_binCode;
   // global array to store all latest register values
   extern std::vector<uint64_t> g_registers;
   // global array to store diff memory address and uint64 data
   extern std::vector<int> g_diffMemoryAddrs;
   extern std::vector<uint64_t> g_diffMemoryQWords;
   // global simulator output
   extern std::shared_ptr<IO::OutputInterface> g_simOut;

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
