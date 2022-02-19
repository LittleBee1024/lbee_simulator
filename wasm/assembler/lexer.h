#pragma once

#include "isa.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string>

// Token types
typedef enum
{
   TOK_IDENT,
   TOK_NUM,
   TOK_REG,
   TOK_INSTR,
   TOK_PUNCT,
   TOK_ERR
} token_t;

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
   void add_symbol(char *name, int p);
   int find_symbol(char *name);
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
};

