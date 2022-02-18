#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string>

/***************************************
 * Variables and functions from flex
 ***************************************/
extern FILE *yasin;
extern int yaslineno;
extern int yaslex();

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

class YasLexer
{
public:
   explicit YasLexer(const char *inFilename);
   YasLexer(const char *buf, size_t size);

   std::string parse();
   void parse(const char *outFilename);

private:
   void reset();

private:
   int m_pass;
   FILE *m_in;
   FILE *m_out;
};
