#include "./sim_viewer.h"
#include "./global.h"

#include "./_common/isa.h"

#include <assert.h>

namespace SIM
{
   SimViewer::SimViewer(const Yis *sim) : m_sim(sim)
   {
   }

   int SimViewer::getCodeLen() const
   {
      return m_sim->m_code.size();
   }

   int SimViewer::getCodeAddr(int pos) const
   {
      assert(pos < m_sim->m_code.size());
      return (m_sim->m_code)[pos].addr;
   }

   const char *SimViewer::getCodeInstr(int pos) const
   {
      assert(pos < m_sim->m_code.size());
      return (m_sim->m_code)[pos].instr.c_str();
   }

   const char *SimViewer::getCodeComment(int pos) const
   {
      assert(pos < m_sim->m_code.size());
      return (m_sim->m_code)[pos].comment.c_str();
   }

   int SimViewer::getCurPC() const
   {
      return m_sim->m_pc;
   }

   void SimViewer::updateGlobalRegisterCache() const
   {
      for (int id = REG_RAX; id < REG_NONE; id++)
      {
         GLOBAL::g_registers[id] = (m_sim->m_reg).getRegVal((REG_ID)id);
      }
   }

   int SimViewer::updateGlobalDiffMemCache() const
   {
      GLOBAL::g_diffMemoryAddrs.clear();
      GLOBAL::g_diffMemoryQWords.clear();
      for (size_t addr = 0; addr < (m_sim->m_mem).size(); addr += sizeof(word_t))
      {
         word_t oldVal = 0;
         (m_sim->m_memCopy).getWord(addr, &oldVal);
         word_t newVal = 0;
         (m_sim->m_mem).getWord(addr, &newVal);
         if (oldVal != newVal)
         {
            GLOBAL::g_diffMemoryAddrs.push_back(addr);
            GLOBAL::g_diffMemoryQWords.push_back(newVal);
         }
      }
      return GLOBAL::g_diffMemoryAddrs.size();
   }

   uint64_t SimViewer::gemMemory(int addr) const
   {
      uint64_t val = 0;
      if (!(m_sim->m_mem).getWord(addr, (word_t *)&val))
         assert(0 && "Failed to get memory data because of invalid memory address");
      return val;
   }

   size_t SimViewer::getMemorySize() const
   {
      return (m_sim->m_mem).size();
   }

   int SimViewer::getCC() const
   {
      return m_sim->m_cc;
   }
}
