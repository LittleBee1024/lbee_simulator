#include "./global.h"
#include "./_common/output.h"

#include "./global.h"

namespace GLOBAL
{
   std::string g_binCode;
   std::vector<uint64_t> g_registers(REG_NONE, 0);
   std::vector<int> g_diffMemoryAddrs;
   std::vector<uint64_t> g_diffMemoryQWords;
   std::shared_ptr<IO::OutputInterface> g_simOut = std::make_shared<IO::StdOut>();

   SIM::Yis *SimSingleton::getInstance()
   {
      static SIM::Yis yis(g_simOut);
      return &yis;
   }
}
