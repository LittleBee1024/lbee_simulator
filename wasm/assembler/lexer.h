#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/***************************************
 * Variables and functions from flex
 ***************************************/
extern FILE *yasin;
extern int yaslineno;
int yaslex();


/***************************************
 * User define functions used in *.lex
 ***************************************/

namespace LBEE_YAS
{
   extern int lineno;
   extern int pass;
   extern int vcode;
   extern int block_factor;
   extern FILE *outfile;
   extern int hit_error;

   void save_line(char *);
   void add_instr(char *);
   void add_reg(char *);
   void add_num(int64_t);
   void add_punct(char);
   void add_ident(char *);
   void fail(const char *);
   void finish_line();
}
