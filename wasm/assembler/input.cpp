#include "input.h"

FileIn::FileIn(const char *filename)
{
   m_in = fopen(filename, "r");
}

FileIn::~FileIn()
{
   if (m_in)
      fclose(m_in);
}

FILE *FileIn::get()
{
   return m_in;
}

MemIn::MemIn(const char *buf):
   m_buf(buf)
{
   m_in = fmemopen(&m_buf[0], m_buf.size(), "r");
}

MemIn::~MemIn()
{
   if (m_in)
      fclose(m_in);
}

FILE *MemIn::get()
{
   return m_in;
}
