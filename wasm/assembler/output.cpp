#include "output.h"

#include <stdio.h>
#include <stdarg.h>

void Output::print(const char *format, ...) const
{
   char buffer[1024];
   va_list args;
   va_start(args, format);
   vsnprintf (buffer, sizeof(buffer), format, args);
   va_end(args);
   m_out << buffer;
}
