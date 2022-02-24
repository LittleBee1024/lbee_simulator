#pragma once

#include "isa.h"
#include "output.h"

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>

#define DONE 1
#define ERROR -1
#define SUCCESS 0

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
   explicit LexerImpl(OutputInterface &out) : m_out(out), m_lineno(0), m_hasError(false), m_addr(0) {}

   void loadLine(const char *s);
   void resetLine();

   void addToken(TokenType type, const char *s, word_t i, char c);
   void fail(const char *message);

   bool hasError() const;
   int processEmptyLine(int pass);
   int processLabel(int pass);
   int processPosInstr(int pass);
   int processAlignInstr(int pass);
   int processNormalInstr(int pass);

private:
   void addSymbol(const char *name, int p);
   int findSymbol(const char *name);

   void getReg(int codepos, int hi);
   void getMem(int codepos);
   void getNum(int codepos, int bytes, int offset);

   void printCode();
   void printLine();

private:
   OutputInterface &m_out;
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
   YasLexer(FILE *in, OutputInterface &out);
   int parse();

public:
   void load(const char *);
   void processTokens();
   void error(const char *);

   // add different tokens
   void addInstr(const char *);
   void addReg(const char *);
   void addNum(int64_t);
   void addPunct(char);
   void addIdent(const char *);

private:
   void reset();
   void setYasIn(FILE *in);
   void resetYasIn();

private:
   int m_pass;
   bool m_hitError;
   LexerImpl m_impl;
};
