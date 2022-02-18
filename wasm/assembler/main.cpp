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

int main_func(int argc, char *argv[])
{
   std::string infname;
   std::string outfname;
   int nextarg = 1;
   if (argc < 2)
      usage(argv[0]);
   while (argv[nextarg][0] == '-')
   {
      char flag = argv[nextarg][1];
      switch (flag)
      {
      case 'V':
         LBEE_YAS::vcode = 1;
         if (argv[nextarg][2])
         {
            LBEE_YAS::block_factor = atoi(argv[nextarg] + 2);
            if (LBEE_YAS::block_factor != 8)
            {
               fprintf(stderr, "Unknown blocking factor %d\n", LBEE_YAS::block_factor);
               exit(1);
            }
         }
         nextarg++;
         break;
      case 'v':
         nextarg++;
         break;
      default:
         usage(argv[0]);
      }
   }
   infname = argv[nextarg];
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

   if (LBEE_YAS::vcode)
   {
      LBEE_YAS::outfile = stdout;
   }
   else
   {
      outfname = infname.substr(0, infname.find_last_of('.')) + ".yo";
      LBEE_YAS::outfile = fopen(outfname.c_str(), "w");
      if (!LBEE_YAS::outfile)
      {
         fprintf(stderr, "Can't open output file '%s'\n", outfname.c_str());
         exit(1);
      }
   }

   LBEE_YAS::pass = 1;

   yaslex();
   fclose(yasin);

   if (LBEE_YAS::hit_error)
      exit(1);

   LBEE_YAS::pass = 2;
   yasin = fopen(infname.c_str(), "r");
   if (!yasin)
   {
      fprintf(stderr, "Can't open input file '%s'\n", infname.c_str());
      exit(1);
   }

   yaslex();
   fclose(yasin);
   fclose(LBEE_YAS::outfile);
   return LBEE_YAS::hit_error;
}

int main(int argc, char *argv[])
{
   return main_func(argc, argv);
}
