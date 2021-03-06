#include "lexer.h"
#include "input.h"
#include "output.h"
#include "api.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

namespace {

const char *s_code = "\
# Execution begins at address 0\n\
   .pos 0\n\
   irmovq stack, %rsp   # Set up stack pointer\n\
   call main            # Execute main program\n\
   halt                 # Terminate program\n\
\n\
# Array of 4 elements\n\
   .align 8\n\
array:   .quad 0x000d000d000d\n\
   .quad 0x00c000c000c0\n\
   .quad 0x0b000b000b00\n\
   .quad 0xa000a000a000\n\
\n\
main:   irmovq array,%rdi\n\
   irmovq $4,%rsi\n\
   call sum             # sum(array, 4)\n\
   ret\n\
\n\
# long sum(long *start, long count)\n\
# start in %rdi, count in %rsi\n\
sum:   irmovq $8,%r8    # Constant 8\n\
   irmovq $1,%r9        # Constant 1\n\
   xorq %rax,%rax       # sum = 0\n\
   andq %rsi,%rsi       # Set CC\n\
   jmp     test         # Goto test\n\
loop:   mrmovq (%rdi),%r10   # Get *start\n\
   addq %r10,%rax       # Add to sum\n\
   addq %r8,%rdi        # start++\n\
   subq %r9,%rsi        # count--.  Set CC\n\
test:   jne    loop          # Stop when 0\n\
   ret                  # Return\n\
\n\
# Stack starts here and grows to lower addresses\n\
   .pos 0x200\n\
stack:\n";

}

static void usage(char *pname)
{
   printf("Usage: %s file.ys\n", pname);
   exit(0);
}

static bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() &&
          0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

void demo()
{
   std::string code = LBEE_SIMULATOR::assemble(s_code);
   std::cout << code;
}

int main(int argc, char *argv[])
{
   if (argc == 1)
   {
      printf("Run demo\n");
      demo();
      return 0;
   }

   std::string infname = argv[1];
   if (!endsWith(infname, ".ys"))
      usage(argv[0]);
   ASSEMBLER::FileIn in(infname.c_str());

   std::string outfname = infname.substr(0, infname.find_last_of('.')) + ".yo";
   ASSEMBLER::FileOut out(outfname.c_str());

   int ret = ASSEMBLER::YasLexer(in, out).parse();
   (ret == ERROR) ? printf("Yas Lexer parse has error\n") : printf("Yas Lexer parse is done\n");

   return ret;
}
