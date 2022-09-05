#pragma once

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
    * @brief APIs to export simulator information
    *
    */
   int sim_get_code_len();
   const char *sim_get_code_instr(int pos);
   const char *sim_get_code_comment(int pos);
}
