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

void YasLexer::loadLine(const char *s)
{
   m_context.loadLine(s);
}

void YasLexer::addInstr(char *s)
{
   m_context.addToken(TOK_INSTR, s, 0, ' ');
}

void YasLexer::addReg(char *s)
{
   m_context.addToken(TOK_REG, s, 0, ' ');
}

void YasLexer::addNum(int64_t i)
{
   m_context.addToken(TOK_NUM, NULL, i, ' ');
}

void YasLexer::addPunct(char c)
{
   m_context.addToken(TOK_PUNCT, NULL, 0, c);
}

void YasLexer::addIdent(char *s)
{
   m_context.addToken(TOK_IDENT, s, 0, ' ');
}

void YasLexer::error(const char *message)
{
   m_context.fail(message);
}

void YasLexer::processTokens()
{
   // Empty line, to start next line
   if (m_context.processEmptyLine(m_out, m_pass))
   {
      resetLine();
      return;
   }

   // error happened, to start next line
   if (m_context.hasError())
   {
      resetLine();
      return;
   }

   // process label, to start next line if the line only has label
   if (m_context.processLabel(m_out, m_pass))
   {
      resetLine();
      return;
   }

   // it should be instruction if the token is not label
   if (m_context.startProcessInstr())
   {
      resetLine();
      return;
   }

   // process .pos instruction
   if (m_context.processPosInstr(m_out, m_pass))
   {
      resetLine();
      return;
   }

   // process .align instruction
   if (m_context.processAlignInstr(m_out, m_pass))
   {
      resetLine();
      return;
   }

   // process normal instruction
   m_context.processNormalInstr(m_out, m_pass);
   resetLine();
   return;
}

void YasLexer::resetLine()
{
   if (m_context.hasError())
      m_hitError = 1;
   // clear current context to continue the next line
   m_context.reset();
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

void Context::reset()
{
   m_hasError = false;
   m_tokens.clear();
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
void Context::printNoTokenLine(FILE *out)
{
   char outstring[33];
   snprintf(outstring, sizeof(outstring), "                            | ");
   fprintf(out, "%s%s\n", outstring, m_line.c_str());
}

void Context::print_code(FILE *out, int pos)
{
   char outstring[33];
   if (pos > 0xFFF)
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
   fprintf(out, "%s%s\n", outstring, m_line.c_str());
}

void Context::loadLine(const char *s)
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
   return m_tokens.front();
}

void Context::popToken()
{
   m_tokens.pop_front();
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

int Context::processEmptyLine(FILE *out, int pass)
{
   if (m_tokens.empty())
   {
      if (pass > 1)
         printNoTokenLine(out);
      return DONE;
   }
   return CONTINUE;
}

int Context::processLabel(FILE *out, int pass)
{
   if (getCurToken().type != TOK_IDENT)
      return CONTINUE;

   token_rec labelToken = getCurToken();
   popToken();
   if (getCurToken().type != TOK_PUNCT || getCurToken().cval != ':')
   {
      fail("Missing Colon");
      return ERR;
   }

   if (pass == 1)
      addSymbol(labelToken.sval.c_str(), getAddress());
   popToken();
   if (m_tokens.empty())
   {
      if (pass > 1)
         print_code(out, getAddress());
      return DONE;
   }

   return CONTINUE;
}

int Context::startProcessInstr()
{
   if (getCurToken().type != TOK_INSTR)
   {
      fail("Bad Instruction");
      return ERR;
   }
   return CONTINUE;
}

int Context::processPosInstr(FILE *out, int pass)
{
   if (strcmp(getCurToken().sval.c_str(), ".pos") != 0)
      return CONTINUE;

   popToken();
   if (getCurToken().type != TOK_NUM)
   {
      fail("Invalid Address");
      return ERR;
   }

   setAddress(getCurToken().ival);
   if (pass > 1)
   {
      print_code(out, getAddress());
   }
   return DONE;
}

int Context::processAlignInstr(FILE *out, int pass)
{
   if (strcmp(getCurToken().sval.c_str(), ".align") != 0)
      return CONTINUE;

   int a;
   popToken();
   if (getCurToken().type != TOK_NUM || (a = getCurToken().ival) <= 0)
   {
      fail("Invalid Alignment");
      return ERR;
   }

   setAddress(((getAddress() + a - 1) / a) * a);
   if (pass > 1)
   {
      print_code(out, getAddress());
   }
   return DONE;
}

int Context::processNormalInstr(FILE *out, int pass)
{
   instr_ptr instr = find_instr(getCurToken().sval.c_str());
   if (instr == NULL)
   {
      fail("Invalid Instruction");
      return ERR;
   }
   int instrSize = instr->bytes;
   int instrAddr = getAddress();
   setAddress(getAddress() + instrSize);

   // don't process instruction in pass 1
   if (pass == 1)
   {
      return DONE;
   }

   // process the instructions
   popToken();
   initDecodeBuf(instrSize, instr->code);
   switch (instr->arg1)
   {
   case R_ARG:
      get_reg(instr->arg1pos, instr->arg1hi);
      break;
   case M_ARG:
      get_mem(instr->arg1pos);
      break;
   case I_ARG:
      get_num(instr->arg1pos, instr->arg1hi, 0);
      break;
   case NO_ARG:
   default:
      break;
   }

   if (instr->arg2 != NO_ARG)
   {
      /* Get comma  */
      if (getCurToken().type != TOK_PUNCT || getCurToken().cval != ',')
      {
         fail("Expecting Comma");
         return ERR;
      }
      popToken();

      /* Get second argument */
      switch (instr->arg2)
      {
      case R_ARG:
         get_reg(instr->arg2pos, instr->arg2hi);
         break;
      case M_ARG:
         get_mem(instr->arg2pos);
         break;
      case I_ARG:
         get_num(instr->arg2pos, instr->arg2hi, 0);
         break;
      case NO_ARG:
      default:
         break;
      }
   }

   print_code(out, instrAddr);
   return DONE;
}
