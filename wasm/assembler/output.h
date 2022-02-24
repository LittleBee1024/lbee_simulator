#pragma once

#include <fstream>

class OutputInterface
{
public:
   OutputInterface() = default;
   virtual ~OutputInterface() = default;

   virtual void print(const char *format, ...) const = 0;
};

class FileOut : public OutputInterface
{
public:
   explicit FileOut(const char *filename);
   FileOut(const FileOut &) = delete;
   FileOut &operator=(const FileOut &) = delete;
   ~FileOut() override;

   void print(const char *format, ...) const override;

private:
   mutable std::ofstream m_out;
};
