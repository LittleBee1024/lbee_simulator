#include "api.h"
#include "input.h"
#include "output.h"
#include "lexer.h"

namespace LBEE_SIMULATOR
{

std::string assemble(const char *buf)
{
   ASSEMBLER::MemIn in(buf);
   ASSEMBLER::MemOut out;
   ASSEMBLER::YasLexer(in, out).parse();
   return out.dump();
}

}
