#include "lexer.h"

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <vector>

/***************************************
 * Variables and functions from flex
 ***************************************/
extern FILE *yasin;
extern int yaslex(YasLexer *);

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

YasLexer::YasLexer(const char *buf, size_t size)
{
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

std::string YasLexer::parse()
{
   return "";
}

void YasLexer::resetYasIn()
{
   fseek(m_in, 0, SEEK_SET);
}

void YasLexer::save_line(const char *s)
{
   m_context.save_line(s);
}

void YasLexer::add_instr(char *s)
{
   m_context.addToken(TOK_INSTR, s, 0, ' ');
}

void YasLexer::add_reg(char *s)
{
   m_context.addToken(TOK_REG, s, 0, ' ');
}

void YasLexer::add_num(int64_t i)
{
   m_context.addToken(TOK_NUM, NULL, i, ' ');
}

void YasLexer::add_punct(char c)
{
   m_context.addToken(TOK_PUNCT, NULL, 0, c);
}

void YasLexer::add_ident(char *s)
{
   m_context.addToken(TOK_IDENT, s, 0, ' ');
}

void YasLexer::error(const char *message)
{
   m_context.fail(message);
}

void YasLexer::finish_line()
{
   // Empty line, to start next line
   if (m_context.done())
   {
      if (m_pass > 1)
         m_context.print_code(m_out, m_context.getAddress());
      start_line();
      return;
   }

   // error happened, to start next line
   if (m_context.hasError())
   {
      start_line();
      return;
   }

   // process label, to start next line if the line only has label
   if (m_context.getCurToken().type == TOK_IDENT)
   {
      token_rec labelToken = m_context.getCurToken();
      m_context.popToken();
      if (m_context.getCurToken().type != TOK_PUNCT || m_context.getCurToken().cval != ':')
      {
         m_context.fail("Missing Colon");
         start_line();
         return;
      }

      if (m_pass == 1)
         m_context.addSymbol(labelToken.sval.c_str(), m_context.getAddress());
      m_context.popToken();
      if (m_context.done())
      {
         if (m_pass > 1)
            m_context.print_code(m_out, m_context.getAddress());
         start_line();
         return;
      }
   }

   // it should be instruction if the token is not label
   if (m_context.getCurToken().type != TOK_INSTR)
   {
      m_context.fail("Bad Instruction");
      start_line();
      return;
   }

   // process .pos instruction
   if (strcmp(m_context.getCurToken().sval.c_str(), ".pos") == 0)
   {
      m_context.popToken();
      if (m_context.getCurToken().type != TOK_NUM)
      {
         m_context.fail("Invalid Address");
         start_line();
         return;
      }

      m_context.setAddress(m_context.getCurToken().ival);
      if (m_pass > 1)
      {
         m_context.print_code(m_out, m_context.getAddress());
      }
      start_line();
      return;
   }

   // process .align instruction
   if (strcmp(m_context.getCurToken().sval.c_str(), ".align") == 0)
   {
      int a;
      m_context.popToken();
      if (m_context.getCurToken().type != TOK_NUM || (a = m_context.getCurToken().ival) <= 0)
      {
         m_context.fail("Invalid Alignment");
         start_line();
         return;
      }
      m_context.setAddress(((m_context.getAddress() + a - 1) / a) * a);

      if (m_pass > 1)
      {
         m_context.print_code(m_out, m_context.getAddress());
      }
      start_line();
      return;
   }

   // process normal instruction for its size
   instr_ptr instr = find_instr(m_context.getCurToken().sval.c_str());
   if (instr == NULL)
   {
      m_context.fail("Invalid Instruction");
      instr = bad_instr();
   }
   int instrSize = instr->bytes;
   int instrAddr = m_context.getAddress();
   m_context.setAddress(m_context.getAddress() + instrSize);

   // don't process instruction in pass 1
   if (m_pass == 1)
   {
      start_line();
      return;
   }

   // process the instructions
   m_context.popToken();
   m_context.initDecodeBuf(instrSize, instr->code);
   switch (instr->arg1)
   {
   case R_ARG:
      m_context.get_reg(instr->arg1pos, instr->arg1hi);
      break;
   case M_ARG:
      m_context.get_mem(instr->arg1pos);
      break;
   case I_ARG:
      m_context.get_num(instr->arg1pos, instr->arg1hi, 0);
      break;
   case NO_ARG:
   default:
      break;
   }
   if (instr->arg2 != NO_ARG)
   {
      /* Get comma  */
      if (m_context.getCurToken().type != TOK_PUNCT || m_context.getCurToken().cval != ',')
      {
         m_context.fail("Expecting Comma");
         start_line();
         return;
      }
      m_context.popToken();

      /* Get second argument */
      switch (instr->arg2)
      {
      case R_ARG:
         m_context.get_reg(instr->arg2pos, instr->arg2hi);
         break;
      case M_ARG:
         m_context.get_mem(instr->arg2pos);
         break;
      case I_ARG:
         m_context.get_num(instr->arg2pos, instr->arg2hi, 0);
         break;
      case NO_ARG:
      default:
         break;
      }
   }

   m_context.print_code(m_out, instrAddr);
   start_line();
}

void YasLexer::start_line()
{
   if (m_context.hasError())
      m_hitError = 1;
   // clear current context to continue the next line
   m_context.resetLine();
}

void Context::addSymbol(const char *name, int p)
{
   m_symbols.emplace(name, p);
}

int Context::findSymbol(const char *name)
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

/* Parse Register from set of m_context.tokens and put into high or low
   4 bits of code[codepos] */
void Context::get_reg(int codepos, int hi)
{
   int rval = REG_NONE;
   char c;
   if (getCurToken().type != TOK_REG)
   {
      fail("Expecting Register ID");
      return;
   }

   rval = find_register(getCurToken().sval.c_str());
   popToken();

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
void Context::get_mem(int codepos)
{
   char rval = REG_NONE;
   word_t val = 0;
   int i;
   char c;
   token_t type = getCurToken().type;
   /* Deal with optional displacement */
   if (type == TOK_NUM)
   {
      popToken();
      val = getCurToken().ival;
      type = getCurToken().type;
   }
   else if (type == TOK_IDENT)
   {
      val = findSymbol(getCurToken().sval.c_str());
      popToken();
      type = getCurToken().type;
   }
   /* Check for optional register */
   if (type == TOK_PUNCT)
   {
      if (getCurToken().cval == '(')
      {
         popToken();
         if (getCurToken().type != TOK_REG)
         {
            fail("Expecting Register Id");
            return;
         }

         rval = find_register(getCurToken().sval.c_str());
         popToken();
         if (getCurToken().type != TOK_PUNCT || getCurToken().cval != ')')
         {
            fail("Expecting ')'");
            return;
         }
         popToken();
      }
   }
   c = (decodeBuf[codepos] & 0xF0) | (rval & 0xF);
   decodeBuf[codepos++] = c;
   for (i = 0; i < 8; i++)
      decodeBuf[codepos + i] = (val >> (i * 8)) & 0xFF;
}

/* Get numeric value of given number of bytes */
/* Offset indicates value to subtract from number (for PC relative) */
void Context::get_num(int codepos, int bytes, int offset)
{
   word_t val = 0;
   int i;
   if (getCurToken().type == TOK_NUM)
   {
      val = getCurToken().ival;
   }
   else if (getCurToken().type == TOK_IDENT)
   {
      val = findSymbol(getCurToken().sval.c_str());
   }
   else
   {
      fail("Number Expected");
      return;
   }
   popToken();

   val -= offset;
   for (i = 0; i < bytes; i++)
      decodeBuf[codepos + i] = (val >> (i * 8)) & 0xFF;
}

void Context::resetLine() {
   m_hasError = false;
   m_tokens.clear();
   m_tokenPos = 0;
   decodeBuf.clear();
}

void Context::addToken(token_t type, const char *s, word_t i, char c)
{
   m_tokens.emplace_back(type, s, i, c);
}

/**
 * Printing format:
 *  0xHHHH: cccccccccccccccccccc | <line>
 *      where HHHH is address
 *      cccccccccccccccccccc is code
 */
void Context::print_code(FILE *out, int pos)
{
   char outstring[33];
   if (pos > 0xFFF)
   {
      if (m_tokens.size())
      {
         if (pos > 0xFFFF)
         {
            fail("Code address limit exceeded");
            exit(1);
         }
         snprintf(outstring, sizeof(outstring), "0x0000:                      | ");
         hexstuff(outstring + 2, pos, 4);
         for (size_t i = 0; i < decodeBuf.size(); i++)
            hexstuff(outstring + 7 + 2 * i, decodeBuf[i] & 0xFF, 2);
      }
      else
         snprintf(outstring, sizeof(outstring), "                             | ");
   }
   else
   {
      if (m_tokens.size())
      {
         if (pos > 0xFFF)
         {
            fail("Code address limit exceeded");
            exit(1);
         }
         snprintf(outstring, sizeof(outstring), "0x000:                      | ");
         hexstuff(outstring + 2, pos, 3);
         for (size_t i = 0; i < decodeBuf.size(); i++)
            hexstuff(outstring + 7 + 2 * i, decodeBuf[i] & 0xFF, 2);
      }
      else
         snprintf(outstring, sizeof(outstring), "                            | ");
   }
   fprintf(out, "%s%s\n", outstring, m_line.c_str());
}

void Context::save_line(const char *s)
{
   assert(s);
   m_line = s;
   for (size_t i = m_line.size() - 1; m_line[i] == '\n' || m_line[i] == '\r'; i--)
      m_line[i] = '\0';
   m_lineno++;
}

bool Context::hasError() const
{
   return m_hasError;
}

void Context::fail(const char *message)
{
   if (!m_hasError)
   {
      fprintf(stderr, "Error on line %d: %s\n", m_lineno, message);
      fprintf(stderr, "Line %d, Byte 0x%.4x: %s\n",
         m_lineno, m_addr, m_line.c_str());
   }
   m_hasError = true;
}

token_rec Context::getCurToken() const
{
   return m_tokens.size() > m_tokenPos ? m_tokens[m_tokenPos] : token_rec();
}

void Context::popToken()
{
   m_tokenPos++;
}

bool Context::done() const
{
   return m_tokens.empty() || m_tokens.size() <= m_tokenPos;
}

int Context::getAddress() const
{
   return m_addr;
}

void Context::setAddress(int a)
{
   m_addr = a;
}

void Context::initDecodeBuf(int instrSize, uint8_t code)
{
   decodeBuf.resize(instrSize, 0);
   decodeBuf[0] = code;
   decodeBuf[1] = HPACK(REG_NONE, REG_NONE);
}
