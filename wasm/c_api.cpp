#include "./c_api.h"
#include "./global.h"
#include "./_common/io_interface.h"
#include "./_common/input.h"
#include "./_common/output.h"
#include "./assembler/yas.h"

#include <string>
#include <memory>

const char *assemble(const char *buf)
{
   static std::string yasOutBuf;

   std::unique_ptr<IO::InputInterface> in = std::make_unique<IO::MemIn>(buf);
   std::shared_ptr<IO::OutputInterface> out = std::make_shared<IO::MemOut>();
   YAS::Lexer(std::move(in)).parse(out);

   yasOutBuf.clear();
   yasOutBuf = dynamic_cast<IO::MemOut *>(out.get())->dump();
   return yasOutBuf.c_str();
}
