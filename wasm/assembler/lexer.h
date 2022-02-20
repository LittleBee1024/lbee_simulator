#pragma once

#include "isa.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

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
   token_rec():
      type(TOK_ERR),
      sval(""),
      ival(0),
      cval(0)
   {}

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
   Context() : m_lineno(0), m_hasError(false), m_addr(0), m_tokenPos(0) {}

   void fail(const char *message);
   void addSymbol(const char *name, int p);
   void addToken(token_t type, const char *s, word_t i, char c);
   void resetLine();

   bool hasError() const;
   void print_code(FILE *out, int pos);
   void save_line(const char *s);
   token_rec getCurToken() const;
   void popToken();
   bool done() const;
   int getAddress() const;
   void setAddress(int a);
   void get_reg(int codepos, int hi);
   void get_mem(int codepos);
   void get_num(int codepos, int bytes, int offset);
   void initDecodeBuf(int instrSize, uint8_t code);

private:
   int findSymbol(const char *name);

private:
   std::unordered_map<std::string, int> m_symbols;
   int m_lineno;
   std::string m_line;
   bool m_hasError;
   int m_addr;
   std::vector<token_rec> m_tokens;
   size_t m_tokenPos;
   std::vector<char> decodeBuf;
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

private:
   void resetYasIn();

private:
   FILE *m_in;
   FILE *m_out;

   int m_pass;
   // set when any error happened
   int m_hitError;
   // current processing data
   Context m_context;
};
