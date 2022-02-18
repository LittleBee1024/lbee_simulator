#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

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

   if (infname.size() > 500)
   {
      fprintf(stderr, "File name too long\n");
      exit(1);
   }

   yasin = fopen(infname.c_str(), "r");
   if (!yasin)
   {
      fprintf(stderr, "Can't open input file '%s'\n", infname.c_str());
      exit(1);
   }

   outfname = infname.substr(0, infname.find_last_of('.')) + ".yo";
   LBEE_YAS::outfile = fopen(outfname.c_str(), "w");
   if (!LBEE_YAS::outfile)
   {
      fprintf(stderr, "Can't open output file '%s'\n", outfname.c_str());
      exit(1);
   }

   LBEE_YAS::pass = 1;
   yaslex();
   fclose(yasin);

   if (LBEE_YAS::hit_error)
      exit(1);

   yasin = fopen(infname.c_str(), "r");
   if (!yasin)
   {
      fprintf(stderr, "Can't open input file '%s'\n", infname.c_str());
      exit(1);
   }

   LBEE_YAS::pass = 2;
   yaslex();
   fclose(yasin);
   fclose(LBEE_YAS::outfile);
   return LBEE_YAS::hit_error;
}

int main(int argc, char *argv[])
{
   return mainImpl(argc, argv);
}
