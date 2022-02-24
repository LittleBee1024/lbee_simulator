#pragma once

#include <fstream>
#include <sstream>
#include <string>

namespace ASSEMBLER
{

class OutputInterface
{
public:
   OutputInterface() = default;
   OutputInterface(const OutputInterface &) = delete;
   OutputInterface &operator=(const OutputInterface &) = delete;
   virtual ~OutputInterface() = default;

   virtual void print(const char *format, ...) = 0;
};

class FileOut : public OutputInterface
{
public:
   explicit FileOut(const char *filename);
   ~FileOut() override;

   void print(const char *format, ...) override;

private:
   std::ofstream m_out;
};

class MemOut : public OutputInterface
{
public:
   MemOut() = default;

   void print(const char *format, ...) override;
   std::string dump() const;

private:
   std::stringstream m_out;
};

}
