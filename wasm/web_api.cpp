#ifndef EM_PORT_API
#   if defined(__EMSCRIPTEN__)
#      include <emscripten.h>
#      if defined(__cplusplus)
#         define EM_PORT_API(rettype) extern "C" rettype EMSCRIPTEN_KEEPALIVE
#      else
#         define EM_PORT_API(rettype) rettype EMSCRIPTEN_KEEPALIVE
#      endif
#   else
#      if defined(__cplusplus)
#         define EM_PORT_API(rettype) extern "C" rettype
#      else
#         define EM_PORT_API(rettype) rettype
#      endif
#   endif
#endif

#include "./c_api.h"

EM_PORT_API(const char*) Sim_Assemble(const char* in) {
   return sim_assemble(in);
}

EM_PORT_API(void) Sim_Load_Code_Save() {
   return sim_load_code_save();
}

EM_PORT_API(void) Sim_Reset_Recover() {
   return sim_reset_recover();
}

EM_PORT_API(const char*) Sim_Step_Run() {
   return sim_step_run();
}

EM_PORT_API(int) Sim_Get_Code_Len() {
   return sim_get_code_len();
}

EM_PORT_API(int) Sim_Get_Code_Addr(int pos) {
   return sim_get_code_addr(pos);
}

EM_PORT_API(const char*) Sim_Get_Code_Instr(int pos) {
   return sim_get_code_instr(pos);
}

EM_PORT_API(const char*) Sim_Get_Code_Comment(int pos) {
   return sim_get_code_comment(pos);
}

EM_PORT_API(int) Sim_Get_Cur_PC() {
   return sim_get_cur_pc();
}

