#include "./demo.h"

#include <algorithm>
#include <iostream>
#include <string>

namespace DEMO
{

int sub(int a, int b)
{
   return a - b;
}

const char* toUppercase(const char* in)
{
   static std::string buf;
   buf.clear();
   buf.append(in);
   std::transform(buf.begin(), buf.end(), buf.begin(), ::toupper);
   return buf.c_str();
}

}

