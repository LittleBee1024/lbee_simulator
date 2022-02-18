#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(char *pname)
{
   printf("Usage: %s [-V[n] -v] file.ys\n", pname);
   printf("   -V[n]  Generate memory initialization in Verilog format (n-way blocking)\n");
   printf("   -v  Enable verbose\n");
   exit(0);
}

extern FILE *yasin;
int main_func(int argc, char *argv[])
{
   int rootlen;
   char infname[512];
   char outfname[512];
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
   rootlen = strlen(argv[nextarg]) - 3;
   if (strcmp(argv[nextarg] + rootlen, ".ys"))
      usage(argv[0]);
   if (rootlen > 500)
   {
      fprintf(stderr, "File name too long\n");
      exit(1);
   }
   strncpy(infname, argv[nextarg], rootlen);
   strcpy(infname + rootlen, ".ys");

   yasin = fopen(infname, "r");
   if (!yasin)
   {
      fprintf(stderr, "Can't open input file '%s'\n", infname);
      exit(1);
   }

   if (LBEE_YAS::vcode)
   {
      LBEE_YAS::outfile = stdout;
   }
   else
   {
      strncpy(outfname, argv[nextarg], rootlen);
      strcpy(outfname + rootlen, ".yo");
      LBEE_YAS::outfile = fopen(outfname, "w");
      if (!LBEE_YAS::outfile)
      {
         fprintf(stderr, "Can't open output file '%s'\n", outfname);
         exit(1);
      }
   }

   LBEE_YAS::pass = 1;

   yaslex();
   fclose(yasin);

   if (LBEE_YAS::hit_error)
      exit(1);

   LBEE_YAS::pass = 2;
   LBEE_YAS::lineno = 1;
   yasin = fopen(infname, "r");
   if (!yasin)
   {
      fprintf(stderr, "Can't open input file '%s'\n", infname);
      exit(1);
   }

   yaslex();
   fclose(yasin);
   fclose(LBEE_YAS::outfile);
   return LBEE_YAS::hit_error;
}
