#pragma once

#include <ostream>

class OutputInterface
{
public:
   OutputInterface() = default;
   virtual ~OutputInterface() = default;

   virtual void print(const char *format, ...) const = 0;
};

class Output : public OutputInterface
{
public:
   explicit Output(std::ostream &out) : m_out(out) {};
   Output(const Output &) = delete;
   Output &operator=(const Output &) = delete;
   ~Output() override = default;

   void print(const char *format, ...) const override;

private:
   std::ostream &m_out;
};
