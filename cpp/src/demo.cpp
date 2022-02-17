#include "./demo.h"

#include <iostream>

namespace DEMO
{

int sub(int a, int b)
{
   return a - b;
}

int multiply(int a, int b)
{
   return a * b;
}

const char* get_string()
{
   static const char str[] = "hello world";
   return str;
}

int set_string(const char* s)
{
   std::cout << s << std::endl;
   return 2;
}

}

