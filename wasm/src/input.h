#pragma once

#include <stdio.h>
#include <string>

namespace ASSEMBLER
{

class InputInterface
{
public:
   InputInterface() = default;
   InputInterface(const InputInterface &) = delete;
   InputInterface &operator=(const InputInterface &) = delete;
   virtual ~InputInterface() = default;

   virtual FILE *get() = 0;
};

class FileIn : public InputInterface
{
public:
   explicit FileIn(const char *filename);
   ~FileIn() override;

   FILE *get() override;

private:
   FILE *m_in;
};

class MemIn : public InputInterface
{
public:
   explicit MemIn(const char *buf);
   ~MemIn() override;

   FILE *get() override;

private:
   std::string m_buf;
   FILE *m_in;
};

}
