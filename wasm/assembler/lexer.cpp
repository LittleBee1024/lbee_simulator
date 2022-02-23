#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>

/***************************************
 * Variables and functions from flex
 ***************************************/
extern FILE *yasin;
extern int yaslex(YasLexer *);

#define DONE 1
#define ERR -1
#define CONTINUE 0

namespace
{

void hexstuff(char *dest, word_t value, int len)
{
   int i;
   for (i = 0; i < len; i++)
   {
      char c;
      int h = (value >> 4 * i) & 0xF;
      c = h < 10 ? h + '0' : h - 10 + 'a';
      dest[len - i - 1] = c;
   }
}

}

YasLexer::YasLexer(const char *inFilename) : m_in(nullptr),
                                             m_out(nullptr),
                                             m_pass(0),
                                             m_hitError(0)
{
   m_in = fopen(inFilename, "r");
   if (!m_in)
   {
      throw std::runtime_error("Can't open input file " + std::string(inFilename));
   }
   // yasin is a global variable defined in flex
   yasin = m_in;
}

int YasLexer::parse(const char *outFilename)
{
   m_out = fopen(outFilename, "w");
   if (!m_out)
   {
      throw std::runtime_error("Can't open output file " + std::string(outFilename));
   }

   m_pass = 1;
   resetYasIn();
   yaslex(this);
   if (m_hitError)
      return m_hitError;

   m_pass = 2;
   resetYasIn();
   yaslex(this);
   return m_hitError;
}

YasLexer::~YasLexer()
{
   if (m_in)
      fclose(m_in);
   if (m_out)
      fclose(m_out);
}

void YasLexer::resetYasIn()
{
   fseek(m_in, 0, SEEK_SET);
}

void YasLexer::load(const char *s)
{
   m_impl.loadLine(s);
}

void YasLexer::addInstr(char *s)
{
   m_impl.addToken(TOK_INSTR, s, 0, ' ');
}

void YasLexer::addReg(char *s)
{
   m_impl.addToken(TOK_REG, s, 0, ' ');
}

void YasLexer::addNum(int64_t i)
{
   m_impl.addToken(TOK_NUM, NULL, i, ' ');
}

void YasLexer::addPunct(char c)
{
   m_impl.addToken(TOK_PUNCT, NULL, 0, c);
}

void YasLexer::addIdent(char *s)
{
   m_impl.addToken(TOK_IDENT, s, 0, ' ');
}

void YasLexer::error(const char *message)
{
   m_impl.fail(message);
}

void YasLexer::processTokens()
{
   if (m_impl.hasError())
      return reset();

   if (m_impl.processEmptyLine(m_out, m_pass))
      return reset();

   if (m_impl.processLabel(m_out, m_pass))
      return reset();

   if (m_impl.processPosInstr(m_out, m_pass))
      return reset();

   if (m_impl.processAlignInstr(m_out, m_pass))
      return reset();

   if (m_impl.processNormalInstr(m_out, m_pass))
      return reset();
}

void YasLexer::reset()
{
   if (m_impl.hasError())
      m_hitError = 1;
   m_impl.resetLine();
}

void LexerImpl::addSymbol(const char *name, int p)
{
   m_symbols.emplace(name, p);
}

int LexerImpl::findSymbol(const char *name)
{
   auto iter = m_symbols.find(name);
   if (iter == m_symbols.end())
   {
      std::string msg = "Fail to find label: " + std::string(name);
      fail(msg.c_str());
      return -1;
   }

   return iter->second;
}

/* Parse Register from set of m_impl.tokens and put into high or low
   4 bits of code[codepos] */
void LexerImpl::getReg(int codepos, int hi)
{
   int rval = REG_NONE;
   char c;
   if (m_tokens.front().type != TOK_REG)
   {
      fail("Expecting Register ID");
      return;
   }

   rval = find_register(m_tokens.front().sval.c_str());
   m_tokens.pop_front();

   /* Insert into output */
   c = decodeBuf[codepos];
   if (hi)
      c = (c & 0x0F) | (rval << 4);
   else
      c = (c & 0xF0) | rval;
   decodeBuf[codepos] = c;
}

/* Get memory reference.
   Can be of form:
   Num(Reg)
   (Reg)
   Num
   Ident
   Ident(Reg)
   Put Reg in low position of current byte, and Number in following bytes
   */
void LexerImpl::getMem(int codepos)
{
   char rval = REG_NONE;
   word_t val = 0;
   int i;
   char c;
   TokenType type = m_tokens.front().type;
   /* Deal with optional displacement */
   if (type == TOK_NUM)
   {
      m_tokens.pop_front();
      val = m_tokens.front().ival;
      type = m_tokens.front().type;
   }
   else if (type == TOK_IDENT)
   {
      val = findSymbol(m_tokens.front().sval.c_str());
      m_tokens.pop_front();
      type = m_tokens.front().type;
   }
   /* Check for optional register */
   if (type == TOK_PUNCT)
   {
      if (m_tokens.front().cval == '(')
      {
         m_tokens.pop_front();
         if (m_tokens.front().type != TOK_REG)
         {
            fail("Expecting Register Id");
            return;
         }

         rval = find_register(m_tokens.front().sval.c_str());
         m_tokens.pop_front();
         if (m_tokens.front().type != TOK_PUNCT || m_tokens.front().cval != ')')
         {
            fail("Expecting ')'");
            return;
         }
         m_tokens.pop_front();
      }
   }
   c = (decodeBuf[codepos] & 0xF0) | (rval & 0xF);
   decodeBuf[codepos++] = c;
   for (i = 0; i < 8; i++)
      decodeBuf[codepos + i] = (val >> (i * 8)) & 0xFF;
}

/* Get numeric value of given number of bytes */
/* Offset indicates value to subtract from number (for PC relative) */
void LexerImpl::getNum(int codepos, int bytes, int offset)
{
   word_t val = 0;
   int i;
   if (m_tokens.front().type == TOK_NUM)
   {
      val = m_tokens.front().ival;
   }
   else if (m_tokens.front().type == TOK_IDENT)
   {
      val = findSymbol(m_tokens.front().sval.c_str());
   }
   else
   {
      fail("Number Expected");
      return;
   }
   m_tokens.pop_front();

   val -= offset;
   for (i = 0; i < bytes; i++)
      decodeBuf[codepos + i] = (val >> (i * 8)) & 0xFF;
}

void LexerImpl::resetLine()
{
   m_hasError = false;
   m_tokens.clear();
   decodeBuf.clear();
}

void LexerImpl::addToken(TokenType type, const char *s, word_t i, char c)
{
   m_tokens.emplace_back(type, s, i, c);
}

/**
 * Printing format:
 *                               | <line>
 */
void LexerImpl::printLine(FILE *out)
{
   char outstring[33];
   snprintf(outstring, sizeof(outstring), "                            | ");
   fprintf(out, "%s%s\n", outstring, m_line.c_str());
}

/**
 * Printing format:
 *  0xHHHH: cccccccccccccccccccc | <line>
 *      where HHHH is address
 *      cccccccccccccccccccc is code
 */
void LexerImpl::printCode(FILE *out)
{
   if (m_addr > 0xFFFF)
   {
      fail("Code address limit exceeded");
      exit(1);
   }

   char outstring[33];
   if (m_addr > 0xFFF)
   {
      snprintf(outstring, sizeof(outstring), "0x0000:                      | ");
      hexstuff(outstring + 2, m_addr, 4);
      for (size_t i = 0; i < decodeBuf.size(); i++)
         hexstuff(outstring + 7 + 2 * i, decodeBuf[i] & 0xFF, 2);
   }
   else
   {
      snprintf(outstring, sizeof(outstring), "0x000:                      | ");
      hexstuff(outstring + 2, m_addr, 3);
      for (size_t i = 0; i < decodeBuf.size(); i++)
         hexstuff(outstring + 7 + 2 * i, decodeBuf[i] & 0xFF, 2);
   }
   fprintf(out, "%s%s\n", outstring, m_line.c_str());
}

void LexerImpl::loadLine(const char *s)
{
   if (s == nullptr)
      return;

   m_line = s;
   for (size_t i = m_line.size() - 1; m_line[i] == '\n' || m_line[i] == '\r'; i--)
      m_line[i] = '\0';
   m_lineno++;
}

bool LexerImpl::hasError() const
{
   return m_hasError;
}

void LexerImpl::fail(const char *message)
{
   if (!m_hasError)
   {
      fprintf(stderr, "Error on line %d: %s\n", m_lineno, message);
      fprintf(stderr, "Line %d, Byte 0x%.4x: %s\n",
              m_lineno, m_addr, m_line.c_str());
   }
   m_hasError = true;
}

int LexerImpl::processEmptyLine(FILE *out, int pass)
{
   if (m_tokens.empty())
   {
      if (pass > 1)
         printLine(out);
      return DONE;
   }
   return CONTINUE;
}

int LexerImpl::processLabel(FILE *out, int pass)
{
   if (m_tokens.front().type != TOK_IDENT)
      return CONTINUE;

   Token labelToken = m_tokens.front();
   m_tokens.pop_front();
   if (m_tokens.front().type != TOK_PUNCT || m_tokens.front().cval != ':')
   {
      fail("Missing Colon");
      return ERR;
   }

   if (pass == 1)
      addSymbol(labelToken.sval.c_str(), m_addr);
   m_tokens.pop_front();
   if (m_tokens.empty())
   {
      if (pass > 1)
         printCode(out);
      return DONE;
   }

   return CONTINUE;
}

int LexerImpl::processPosInstr(FILE *out, int pass)
{
   if (m_tokens.front().type != TOK_INSTR)
   {
      fail("Bad Instruction");
      return ERR;
   }

   if (strcmp(m_tokens.front().sval.c_str(), ".pos") != 0)
      return CONTINUE;

   m_tokens.pop_front();
   if (m_tokens.front().type != TOK_NUM)
   {
      fail("Invalid Address");
      return ERR;
   }

   m_addr = m_tokens.front().ival;
   if (pass > 1)
   {
      printCode(out);
   }
   return DONE;
}

int LexerImpl::processAlignInstr(FILE *out, int pass)
{
   if (m_tokens.front().type != TOK_INSTR)
   {
      fail("Bad Instruction");
      return ERR;
   }

   if (strcmp(m_tokens.front().sval.c_str(), ".align") != 0)
      return CONTINUE;

   int a;
   m_tokens.pop_front();
   if (m_tokens.front().type != TOK_NUM || (a = m_tokens.front().ival) <= 0)
   {
      fail("Invalid Alignment");
      return ERR;
   }

   m_addr = ((m_addr + a - 1) / a) * a;
   if (pass > 1)
   {
      printCode(out);
   }
   return DONE;
}

int LexerImpl::processNormalInstr(FILE *out, int pass)
{
   if (m_tokens.front().type != TOK_INSTR)
   {
      fail("Bad Instruction");
      return ERR;
   }

   instr_ptr instr = find_instr(m_tokens.front().sval.c_str());
   if (instr == NULL)
   {
      fail("Invalid Instruction");
      return ERR;
   }
   // get expected instruction token, pop it from the deque
   m_tokens.pop_front();

   // don't process instruction in pass 1
   if (pass == 1)
   {
      m_addr += instr->bytes;
      return DONE;
   }

   // process the instructions
   decodeBuf.resize(instr->bytes, 0);
   decodeBuf[0] = instr->code;
   decodeBuf[1] = HPACK(REG_NONE, REG_NONE);
   switch (instr->arg1)
   {
   case R_ARG:
      getReg(instr->arg1pos, instr->arg1hi);
      break;
   case M_ARG:
      getMem(instr->arg1pos);
      break;
   case I_ARG:
      getNum(instr->arg1pos, instr->arg1hi, 0);
      break;
   case NO_ARG:
   default:
      break;
   }

   if (instr->arg2 != NO_ARG)
   {
      /* Get comma  */
      if (m_tokens.front().type != TOK_PUNCT || m_tokens.front().cval != ',')
      {
         fail("Expecting Comma");
         return ERR;
      }
      m_tokens.pop_front();

      /* Get second argument */
      switch (instr->arg2)
      {
      case R_ARG:
         getReg(instr->arg2pos, instr->arg2hi);
         break;
      case M_ARG:
         getMem(instr->arg2pos);
         break;
      case I_ARG:
         getNum(instr->arg2pos, instr->arg2hi, 0);
         break;
      case NO_ARG:
      default:
         break;
      }
   }

   printCode(out);
   m_addr += instr->bytes;
   return DONE;
}
