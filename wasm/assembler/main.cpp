#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <exception>
#include <iostream>

static void usage(char *pname)
{
   printf("Usage: %s file.ys\n", pname);
   exit(0);
}

static bool endsWith(const std::string& str, const std::string& suffix)
{
   return str.size() >= suffix.size() &&
      0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

int mainImpl(int argc, char *argv[])
{
   std::string infname;
   std::string outfname;
   if (argc < 2)
      usage(argv[0]);

   infname = argv[1];
   if (!endsWith(infname, ".ys"))
      usage(argv[0]);

   outfname = infname.substr(0, infname.find_last_of('.')) + ".yo";

   try{
      YasLexer l(infname.c_str());
      l.parse(outfname.c_str());
   }
   catch(std::exception &e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }
   catch(...)
   {
      std::cerr << "Unknow error happens" << std::endl;
      return -1;
   }

   return 0;
}

int main(int argc, char *argv[])
{
   return mainImpl(argc, argv);
}
