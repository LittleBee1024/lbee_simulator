#include "./c_api.h"
#include "./global.h"
#include "./sim_viewer.h"
#include "./_common/io_interface.h"
#include "./_common/input.h"
#include "./_common/output.h"
#include "./assembler/yas.h"
#include "./simulator/state.h"

#include <string>
#include <memory>
#include <assert.h>

const char *sim_assemble(const char *buf)
{
   GLOBAL::g_binCode.clear();
   if (std::string(buf).empty())
      return "";

   std::unique_ptr<IO::InputInterface> in = std::make_unique<IO::MemIn>(buf);
   std::shared_ptr<IO::OutputInterface> out = std::make_shared<IO::MemOut>();
   YAS::Lexer(std::move(in)).parse(out);

   GLOBAL::g_binCode = dynamic_cast<IO::MemOut *>(out.get())->dump();
   return GLOBAL::g_binCode.c_str();
}

void sim_load_code_save()
{
   std::shared_ptr<IO::InputInterface> in = std::make_shared<IO::MemIn>(GLOBAL::g_binCode.c_str());
   auto yis = GLOBAL::SimSingleton::getInstance();
   if (yis->loadCode(in) == 0)
   {
      GLOBAL::g_simOut->out("[WARN] No code was loaded\n");
      return;
   }
   yis->save();
   return;
}

void sim_reset_recover()
{
   auto yis = GLOBAL::SimSingleton::getInstance();
   yis->reset();
   yis->recover();
}

const char *sim_step_run()
{
   auto yis = GLOBAL::SimSingleton::getInstance();

   SIM::State state = yis->runOneCycle();
   GLOBAL::g_simOut->out("[INFO] The cycle is done with State=%s\n", SIM::getStateName(state));

   return SIM::getStateName(state);
}

int sim_get_code_len()
{
   const SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeLen();
}

int sim_get_code_addr(int pos)
{
   const SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeAddr(pos);
}

const char *sim_get_code_instr(int pos)
{
   const SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeInstr(pos);
}

const char *sim_get_code_comment(int pos)
{
   const SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeComment(pos);
}

int sim_get_cur_pc()
{
   const SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCurPC();
}

uint64_t *sim_get_registers()
{
   const SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   v.updateGlobalRegisterCache();
   return GLOBAL::g_registers.data();
}

int sim_get_diff_mem_counts()
{
   const SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.updateGlobalDiffMemCache();
}

int *sim_get_diff_mem_addr()
{
   return GLOBAL::g_diffMemoryAddrs.data();
}

uint64_t *sim_get_diff_mem_data()
{
   return GLOBAL::g_diffMemoryQWords.data();
}
