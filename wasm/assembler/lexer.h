#pragma once

#include "isa.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

typedef enum
{
   TOK_IDENT,
   TOK_NUM,
   TOK_REG,
   TOK_INSTR,
   TOK_PUNCT,
   TOK_ERR
} token_t;

// Token representation
typedef struct
{
   std::string sval; /* String    */
   word_t ival;      /* Integer   */
   char cval;        /* Character */
   token_t type;     /* Type    */
} token_rec, *token_ptr;

struct symbol_t
{
   symbol_t(const char *n, int p): name(n), pos(p) {}
   std::string name;
   int pos;
};

class YasLexer
{
public:
   explicit YasLexer(const char *inFilename);
   explicit YasLexer(const char *buf, size_t size);
   ~YasLexer();

   void parse(const char *outFilename);
   std::string parse();

public:
   void save_line(char *);
   void add_instr(char *);
   void add_reg(char *);
   void add_num(int64_t);
   void add_punct(char);
   void add_ident(char *);
   void error(const char *);
   void finish_line();

private:
   void fail(const char *message);
   void start_line();
   void add_token(token_t type, char *s, word_t i, char c);
   void hexstuff(char *dest, word_t value, int len);
   void add_symbol(const char *name, int p);
   int find_symbol(const char *name);
   void get_reg(int codepos, int hi);
   void get_mem(int codepos);
   void get_num(int codepos, int bytes, int offset);
   void print_code(FILE *out, int pos);

private:
   void resetYasIn();

private:
   FILE *m_in;
   FILE *m_out;
   int m_pass;
   // symbole table
   std::vector<symbol_t> m_symbols;
   // current line string
   std::string m_curLine;
   // tokens in current line
   std::vector<token_rec> m_tokens;
   // current process token position
   int m_tokenPos;
   // current process code
   std::vector<char> m_curCode;
};
