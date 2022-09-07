#pragma once

#include <stdint.h>

extern "C"
{

   /**
    * @brief convert assembly code to machine code
    *
    * @param asmCode is a pointer to assembly ASM code buffer
    * @return a pointer to bin code buffer
    */
   const char *sim_assemble(const char *asmCode);

   /**
    * @brief Load binary code from "assemble" function to the simulator and save the memory and registers
    *
    */
   void sim_load_code_save();

   /**
    * @brief Reset the states of the simulator and recover the memory and registers from "load_code_save" function
    *
    */
   void sim_reset_recover();

   /**
    * @brief Run simulator in serveral steps after loading code
    * 
    * @return Current state name
    */
   const char *sim_step_run();

   /**
    * @brief APIs to export simulator code memroy
    *
    */
   int sim_get_code_len();
   int sim_get_code_addr(int pos);
   const char *sim_get_code_instr(int pos);
   const char *sim_get_code_comment(int pos);

   /**
    * @brief APIs to export simulator registers and data memory
    * 
    */
   int sim_get_cur_pc();
   uint64_t *sim_get_registers();
   int sim_get_diff_mem_counts();
   int *sim_get_diff_mem_addr();
   uint64_t *sim_get_diff_mem_data();
}
