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

#include "util.h"
#include "demo.h"

EM_PORT_API(int) Hello() {
   return LIBDEMO::hello();
}

EM_PORT_API(int) Sub(int a, int b) {
   return DEMO::sub(a, b);
}

EM_PORT_API(int) Multiply(int a, int b) {
   return DEMO::multiply(a, b);
}

EM_PORT_API(const char*) GetString() {
   return DEMO::get_string();
}

EM_PORT_API(int) SetString(const char* s) {
   return DEMO::set_string(s);
}

