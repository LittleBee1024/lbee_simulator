#include "./c_api.h"
#include "./global.h"
#include "./sim_viewer.h"
#include "./_common/io_interface.h"
#include "./_common/input.h"
#include "./_common/output.h"
#include "./assembler/yas.h"

#include <string>
#include <memory>

const char *sim_assemble(const char *buf)
{
   GLOBAL::binCode.clear();
   if (std::string(buf).empty())
      return "";

   std::unique_ptr<IO::InputInterface> in = std::make_unique<IO::MemIn>(buf);
   std::shared_ptr<IO::OutputInterface> out = std::make_shared<IO::MemOut>();
   YAS::Lexer(std::move(in)).parse(out);

   GLOBAL::binCode = dynamic_cast<IO::MemOut *>(out.get())->dump();
   return GLOBAL::binCode.c_str();
}

void sim_load_code_save()
{
   std::shared_ptr<IO::InputInterface> in = std::make_shared<IO::MemIn>(GLOBAL::binCode.c_str());
   auto yis = GLOBAL::SimSingleton::getInstance();
   if (yis->loadCode(in) == 0)
   {
      GLOBAL::simOut->out("[WARN] No code was loaded\n");
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

int sim_get_code_len()
{
   SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeLen();
}

int sim_get_code_addr(int pos)
{
   SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeAddr(pos);
}

const char *sim_get_code_instr(int pos)
{
   SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeInstr(pos);
}

const char *sim_get_code_comment(int pos)
{
   SIM::SimViewer v(GLOBAL::SimSingleton::getInstance());
   return v.getCodeComment(pos);
}
