#include "./global.h"
#include "./_common/output.h"

#include "./global.h"

namespace GLOBAL
{
   std::string binCode;
   std::shared_ptr<IO::OutputInterface> simOut = std::make_shared<IO::StdOut>();

   SIM::Yis *SimSingleton::getInstance()
   {
      static SIM::Yis yis(simOut);
      return &yis;
   }
}
