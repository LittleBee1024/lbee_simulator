#include "output.h"

#include <stdio.h>
#include <stdarg.h>

FileOut::FileOut(const char *filename) :
   m_out(filename)
{
}

FileOut::~FileOut()
{
   m_out.close();
}

void FileOut::print(const char *format, ...) const
{
   char buffer[1024];
   va_list args;
   va_start(args, format);
   vsnprintf (buffer, sizeof(buffer), format, args);
   va_end(args);
   m_out << buffer;
}
