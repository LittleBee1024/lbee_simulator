#include "lexer.h"

#include <exception>
#include <iostream>
#include <stdio.h>
#include <string>

static void usage(char *pname)
{
   printf("Usage: %s file.ys\n", pname);
   exit(0);
}

static bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() &&
          0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

static int parseFromFile(const char *infname, const char *outfname)
{
   int ret = 0;
   try
   {
      YasLexer l(infname);
      ret = l.parse(outfname);
   }
   catch (const std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }
   catch (...)
   {
      std::cerr << "Unknow error happens" << std::endl;
      return -1;
   }

   return ret;
}

int main(int argc, char *argv[])
{
   if (argc < 2)
      usage(argv[0]);

   std::string infname = argv[1];
   if (!endsWith(infname, ".ys"))
      usage(argv[0]);

   std::string outfname = infname.substr(0, infname.find_last_of('.')) + ".yo";

   return parseFromFile(infname.c_str(), outfname.c_str());
}
