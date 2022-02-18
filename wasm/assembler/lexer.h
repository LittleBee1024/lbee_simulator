#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string>

class YasLexer
{
public:
   explicit YasLexer(const char *inFilename);
   explicit YasLexer(const char *buf, size_t size);
   ~YasLexer();

   void parse(const char *outFilename);
   std::string parse();

private:
   void resetYasIn();

private:
   FILE *m_in;
   FILE *m_out;
   int m_pass;
};

/***************************************
 * Variables and functions from flex
 ***************************************/
extern FILE *yasin;
extern int yaslineno;
extern int yaslex(YasLexer *);

/***************************************
 * User define functions used in *.lex
 ***************************************/

namespace LBEE_YAS
{
   void save_line(char *);
   void add_instr(char *);
   void add_reg(char *);
   void add_num(int64_t);
   void add_punct(char);
   void add_ident(char *);
   void fail(const char *);
   void finish_line();
}
