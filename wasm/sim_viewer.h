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

      int getCurPC() const;
      void updateGlobalRegisterCache() const;

      int updateGlobalDiffMemCache() const;
      uint64_t gemMemory(int addr) const;
      size_t getMemorySize() const;

   private:
      const Yis *m_sim;
   };
}
