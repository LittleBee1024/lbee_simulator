#pragma once

#include "./simulator/sim_yis.h"

namespace SIM
{
   class SimViewer
   {
   public:
      SimViewer(const Yis *sim);
      ~SimViewer() = default;
      SimViewer(const SimViewer&) = delete;
      SimViewer &operator=(const SimViewer &) = delete;

      int getCodeLen() const;
      int getCodeAddr(int pos) const;
      const char *getCodeInstr(int pos) const;
      const char *getCodeComment(int pos) const;

   private:
      const Yis *m_sim;
   };
}
