#pragma once

#include "isa.h"

#include <deque>
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
} TokenType;

// Token representation
struct Token
{
   Token(TokenType t, const char *s, word_t i, char c):
      type(t),
      sval(s ? s : ""),
      ival(i),
      cval(c)
   {}

   TokenType type;
   std::string sval;
   word_t ival;
   char cval;
};

class LexerImpl
{
public:
   LexerImpl() : m_lineno(0), m_hasError(false), m_addr(0) {}

   void loadLine(const char *s);
   void resetLine();

   void addToken(TokenType type, const char *s, word_t i, char c);
   void fail(const char *message);

   bool hasError() const;
   int processEmptyLine(FILE *out, int pass);
   int processLabel(FILE *out, int pass);
   int processPosInstr(FILE *out, int pass);
   int processAlignInstr(FILE *out, int pass);
   int processNormalInstr(FILE *out, int pass);

private:
   void addSymbol(const char *name, int p);
   int findSymbol(const char *name);

   void get_reg(int codepos, int hi);
   void get_mem(int codepos);
   void get_num(int codepos, int bytes, int offset);

   void printCode(FILE *out);
   void printLine(FILE *out);

private:
   std::unordered_map<std::string, int> m_symbols;
   int m_lineno;
   std::string m_line;
   bool m_hasError;
   int m_addr;
   std::deque<Token> m_tokens;
   std::vector<char> decodeBuf;
};

class YasLexer
{
public:
   explicit YasLexer(const char *inFilename);
   ~YasLexer();
   int parse(const char *outFilename);

public:
   void load(const char *);
   void processTokens();
   void error(const char *);

   // add different tokens
   void addInstr(char *);
   void addReg(char *);
   void addNum(int64_t);
   void addPunct(char);
   void addIdent(char *);

private:
   void reset();
   void resetYasIn();

private:
   FILE *m_in;
   FILE *m_out;

   int m_pass;
   int m_hitError;
   LexerImpl m_impl;
};
