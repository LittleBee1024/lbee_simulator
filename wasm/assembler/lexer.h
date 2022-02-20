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
struct token_rec
{
   token_rec(token_t t, const char *s, word_t i, char c):
      type(t),
      sval(s ? s : ""),
      ival(i),
      cval(c)
   {}
   token_t type;
   std::string sval;
   word_t ival;
   char cval;
};

struct symbol_t
{
   symbol_t(const char *n, int p): name(n), pos(p) {}
   std::string name;
   int pos;
};

// class for lexer context, will be updated during processing a line
class Context
{
public:
   Context() : addr(0), tokenPos(0), m_lineno(0), m_hasError(false) {}

   void clear();
   void addToken(token_t type, const char *s, word_t i, char c);
   void fail(const char *message);
   bool hasError() const;
   void print_code(FILE *out, int pos);
   void save_line(const char *s);

   int addr;
   std::vector<token_rec> tokens;
   int tokenPos;
   std::vector<char> decodeBuf;

private:
   int m_lineno;
   std::string m_line;
   bool m_hasError;
};

class YasLexer
{
public:
   explicit YasLexer(const char *inFilename);
   explicit YasLexer(const char *buf, size_t size);
   ~YasLexer();

   int parse(const char *outFilename);
   std::string parse();

public:
   void save_line(const char *);
   void add_instr(char *);
   void add_reg(char *);
   void add_num(int64_t);
   void add_punct(char);
   void add_ident(char *);
   void error(const char *);
   void finish_line();

private:
   void start_line();
   void add_symbol(const char *name, int p);
   int find_symbol(const char *name);
   void get_reg(int codepos, int hi);
   void get_mem(int codepos);
   void get_num(int codepos, int bytes, int offset);

private:
   void resetYasIn();

private:
   FILE *m_in;
   FILE *m_out;

   // symbole table
   std::vector<symbol_t> m_symbols;

   int m_pass;
   // set when any error happened
   int m_hitError;
   // current processing data
   Context m_context;
};
