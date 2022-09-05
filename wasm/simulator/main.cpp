#include "./simulator/sim_interface.h"
#include "./simulator/sim_yis.h"
#include "./simulator/state.h"
#include "./_common/io_interface.h"
#include "./_common/output.h"
#include "./_common/input.h"

#include <memory>
#include <unistd.h>
#include <assert.h>

char binCode[] ="\
0x000:                      |         .pos 0                                           \n\
0x000: 30f40002000000000000 |         irmovq stack, %rsp      # Set up stack pointer   \n\
0x00a: 803800000000000000   |         call main               # Execute main program   \n\
0x013: 00                   |         halt                    # Terminate program      \n\
                            |                                                          \n\
                            | # Array of 4 elements                                    \n\
0x018:                      |         .align 8                                         \n\
0x018: 0d000d000d000000     | array:  .quad 0x000d000d000d                             \n\
0x020: c000c000c0000000     |         .quad 0x00c000c000c0                             \n\
0x028: 000b000b000b0000     |         .quad 0x0b000b000b00                             \n\
0x030: 00a000a000a00000     |         .quad 0xa000a000a000                             \n\
                            |                                                          \n\
0x038: 30f71800000000000000 | main:   irmovq array,%rdi                                \n\
0x042: 30f60400000000000000 |         irmovq $4,%rsi                                   \n\
0x04c: 805600000000000000   |         call sum             # sum(array, 4)             \n\
0x055: 90                   |         ret                                              \n\
                            |                                                          \n\
                            | # long sum(long *start, long count)                      \n\
                            | # start in %rdi, count in %rsi                           \n\
0x056: 30f80800000000000000 | sum:    irmovq $8,%r8        # Constant 8                \n\
0x060: 30f90100000000000000 |         irmovq $1,%r9        # Constant 1                \n\
0x06a: 6300                 |         xorq %rax,%rax       # sum = 0                   \n\
0x06c: 6266                 |         andq %rsi,%rsi       # Set CC                    \n\
0x06e: 708700000000000000   |         jmp     test         # Goto test                 \n\
0x077: 50a70000000000000000 | loop:   mrmovq (%rdi),%r10   # Get *start                \n\
0x081: 60a0                 |         addq %r10,%rax       # Add to sum                \n\
0x083: 6087                 |         addq %r8,%rdi        # start++                   \n\
0x085: 6196                 |         subq %r9,%rsi        # count--.  Set CC          \n\
0x087: 747700000000000000   | test:   jne    loop          # Stop when 0               \n\
0x090: 90                   |         ret                  # Return                    \n\
                            |                                                          \n\
                            | # Stack starts here and grows to lower addresses         \n\
0x200:                      |         .pos 0x200                                       \n\
0x200:                      | stack:                                                   \n\
";

int main(int argc, char *argv[])
{
   std::shared_ptr<IO::InputInterface> in = std::make_shared<IO::MemIn>(binCode);
   std::shared_ptr<IO::OutputInterface> out = std::make_shared<IO::StdOut>();
   std::unique_ptr<SIM::SimInterface> sim = std::make_unique<SIM::Yis>(out);

   if (!sim->loadCode(in))
   {
      out->out("[ERROR] Failed to load code\n");
      return -1;
   }

   sim->save();
   sim->run(1000);
   sim->diffReg();
   sim->diffMem();

   return 0;
}
