#pragma once

#include <fstream>
#include <sstream>
#include <string>

class OutputInterface
{
public:
   OutputInterface() = default;
   OutputInterface(const OutputInterface &) = delete;
   OutputInterface &operator=(const OutputInterface &) = delete;
   virtual ~OutputInterface() = default;

   virtual void print(const char *format, ...) const = 0;
};

class FileOut : public OutputInterface
{
public:
   explicit FileOut(const char *filename);
   ~FileOut() override;

   void print(const char *format, ...) const override;

private:
   mutable std::ofstream m_out;
};

class MemOut : public OutputInterface
{
public:
   MemOut() = default;

   void print(const char *format, ...) const override;
   std::string dump() const;

private:
   mutable std::stringstream m_out;
};
