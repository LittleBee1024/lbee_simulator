#include "./sim_viewer.h"

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

}
