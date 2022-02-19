#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <vector>

/***************************************
 * Variables and functions from flex
 ***************************************/
extern FILE *yasin;
extern int yaslineno;
extern int yaslex(YasLexer *);

namespace
{
   // Address of current instruction being processed
   int bytepos = 0;
   // Am I trying to finish off a line with an error
   int error_mode = 0;

   /* Information about current instruction being generated */
   char code[10];   /* Byte encoding */
   int bcount = 0;  /* Length of current instruction */

   FILE *outfile;
   // Have I hit any errors
   int hit_error = 0;
}

YasLexer::YasLexer(const char *inFilename) : m_in(nullptr),
                                             m_out(nullptr),
                                             m_pass(0),
                                             m_tokenPos(0)
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

void YasLexer::parse(const char *outFilename)
{
   m_out = fopen(outFilename, "w");
   if (!m_out)
   {
      throw std::runtime_error("Can't open output file " + std::string(outFilename));
   }
   // TODO: replace outfile with std::ostream
   outfile = m_out;

   m_pass = 1;
   resetYasIn();
   yaslex(this);

   m_pass = 2;
   resetYasIn();
   yaslex(this);
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

void YasLexer::save_line(char *s)
{
   m_curLine.clear();
   m_curLine = s;
   for (size_t i = m_curLine.size() - 1; m_curLine[i] == '\n' || m_curLine[i] == '\r'; i--)
      m_curLine[i] = '\0';
}

void YasLexer::add_instr(char *s)
{
   add_token(TOK_INSTR, s, 0, ' ');
}

void YasLexer::add_reg(char *s)
{
   add_token(TOK_REG, s, 0, ' ');
}

void YasLexer::add_num(int64_t i)
{
   add_token(TOK_NUM, NULL, i, ' ');
}

void YasLexer::add_punct(char c)
{
   add_token(TOK_PUNCT, NULL, 0, c);
}

void YasLexer::add_ident(char *s)
{
   add_token(TOK_IDENT, s, 0, ' ');
}

void YasLexer::error(const char *message)
{
   fail(message);
}

void YasLexer::finish_line()
{
   int size;
   instr_ptr instr;
   int savebytepos = bytepos;
   m_tokenPos = 0;
   if (m_tokens.empty())
   {
      if (m_pass > 1)
         print_code(outfile, savebytepos);
      start_line();
      return; /* Empty line */
   }
   /* Completion of an erroneous line */
   if (error_mode)
   {
      start_line();
      return;
   }

   /* See if this is a labeled line */
   if (m_tokens[0].type == TOK_IDENT)
   {
      if (m_tokens[1].type != TOK_PUNCT || m_tokens[1].cval != ':')
      {
         fail("Missing Colon");
         start_line();
         return;
      }
      else
      {
         if (m_pass == 1)
            add_symbol(m_tokens[0].sval.c_str(), bytepos);
         m_tokenPos += 2;
         if (m_tokens.size() == 2)
         {
            /* That's all for this line */
            if (m_pass > 1)
               print_code(outfile, savebytepos);
            start_line();
            return;
         }
      }
   }
   /* Get instruction */
   if (m_tokens[m_tokenPos].type != TOK_INSTR)
   {
      fail("Bad Instruction");
      start_line();
      return;
   }
   /* Process .pos */
   if (strcmp(m_tokens[m_tokenPos].sval.c_str(), ".pos") == 0)
   {
      if (m_tokens[++m_tokenPos].type != TOK_NUM)
      {
         fail("Invalid Address");
         start_line();
         return;
      }
      bytepos = m_tokens[m_tokenPos].ival;
      if (m_pass > 1)
      {
         print_code(outfile, bytepos);
      }
      start_line();
      return;
   }
   /* Process .align */
   if (strcmp(m_tokens[m_tokenPos].sval.c_str(), ".align") == 0)
   {
      int a;
      if (m_tokens[++m_tokenPos].type != TOK_NUM || (a = m_tokens[m_tokenPos].ival) <= 0)
      {
         fail("Invalid Alignment");
         start_line();
         return;
      }
      bytepos = ((bytepos + a - 1) / a) * a;

      if (m_pass > 1)
      {
         print_code(outfile, bytepos);
      }
      start_line();
      return;
   }
   /* Get instruction size */
   instr = find_instr(m_tokens[m_tokenPos++].sval.c_str());
   if (instr == NULL)
   {
      fail("Invalid Instruction");
      instr = bad_instr();
   }
   size = instr->bytes;
   bytepos += size;
   bcount = size;

   /* If this is m_pass 1, then we're done */
   if (m_pass == 1)
   {
      start_line();
      return;
   }

   /* Here's where we really process the instructions */
   code[0] = instr->code;
   code[1] = HPACK(REG_NONE, REG_NONE);
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
      if (m_tokens[m_tokenPos].type != TOK_PUNCT || m_tokens[m_tokenPos].cval != ',')
      {
         fail("Expecting Comma");
         start_line();
         return;
      }
      m_tokenPos++;

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

   print_code(outfile, savebytepos);
   start_line();
}

void YasLexer::fail(const char *message)
{
   if (!error_mode)
   {
      fprintf(stderr, "Error on line %d: %s\n", yaslineno, message);
      fprintf(stderr, "Line %d, Byte 0x%.4x: %s\n",
              yaslineno, bytepos, m_curLine.c_str());
   }
   error_mode = 1;
   hit_error = 1;
}

void YasLexer::start_line()
{
   error_mode = 0;
   m_tokenPos = 0;
   bcount = 0;
   m_tokens.clear();
}

void YasLexer::add_token(token_t type, char *s, word_t i, char c)
{
   if (m_tokens.empty())
      start_line();
   token_rec token;
   token.type = type;
   token.sval = s ? s : "";
   token.ival = i;
   token.cval = c;
   m_tokens.push_back(token);
}

void YasLexer::hexstuff(char *dest, word_t value, int len)
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

void YasLexer::add_symbol(const char *name, int p)
{
   m_symbols.emplace_back(name, p);
}

int YasLexer::find_symbol(const char *name)
{
   for (const auto &s : m_symbols)
   {
      if (s.name.compare(name) == 0)
         return s.pos;
   }
   fail("Can't find label");
   return -1;
}

/* Parse Register from set of m_tokens and put into high or low
   4 bits of code[codepos] */
void YasLexer::get_reg(int codepos, int hi)
{
   int rval = REG_NONE;
   char c;
   if (m_tokens[m_tokenPos].type != TOK_REG)
   {
      fail("Expecting Register ID");
      return;
   }
   else
   {
      rval = find_register(m_tokens[m_tokenPos].sval.c_str());
   }
   /* Insert into output */
   c = code[codepos];
   if (hi)
      c = (c & 0x0F) | (rval << 4);
   else
      c = (c & 0xF0) | rval;
   code[codepos] = c;
   m_tokenPos++;
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
void YasLexer::get_mem(int codepos)
{
   char rval = REG_NONE;
   word_t val = 0;
   int i;
   char c;
   token_t type = m_tokens[m_tokenPos].type;
   /* Deal with optional displacement */
   if (type == TOK_NUM)
   {
      val = m_tokens[m_tokenPos++].ival;
      type = m_tokens[m_tokenPos].type;
   }
   else if (type == TOK_IDENT)
   {
      val = find_symbol(m_tokens[m_tokenPos++].sval.c_str());
      type = m_tokens[m_tokenPos].type;
   }
   /* Check for optional register */
   if (type == TOK_PUNCT)
   {
      if (m_tokens[m_tokenPos].cval == '(')
      {
         m_tokenPos++;
         if (m_tokens[m_tokenPos].type == TOK_REG)
            rval = find_register(m_tokens[m_tokenPos++].sval.c_str());
         else
         {
            fail("Expecting Register Id");
            return;
         }
         if (m_tokens[m_tokenPos].type != TOK_PUNCT || m_tokens[m_tokenPos++].cval != ')')
         {
            fail("Expecting ')'");
            return;
         }
      }
   }
   c = (code[codepos] & 0xF0) | (rval & 0xF);
   code[codepos++] = c;
   for (i = 0; i < 8; i++)
      code[codepos + i] = (val >> (i * 8)) & 0xFF;
}

/* Get numeric value of given number of bytes */
/* Offset indicates value to subtract from number (for PC relative) */
void YasLexer::get_num(int codepos, int bytes, int offset)
{
   word_t val = 0;
   int i;
   if (m_tokens[m_tokenPos].type == TOK_NUM)
   {
      val = m_tokens[m_tokenPos].ival;
   }
   else if (m_tokens[m_tokenPos].type == TOK_IDENT)
   {
      val = find_symbol(m_tokens[m_tokenPos].sval.c_str());
   }
   else
   {
      fail("Number Expected");
      return;
   }
   val -= offset;
   for (i = 0; i < bytes; i++)
      code[codepos + i] = (val >> (i * 8)) & 0xFF;
   m_tokenPos++;
}
/**
 * Printing format:
 *  0xHHHH: cccccccccccccccccccc | <line>
 *      where HHHH is address
 *      cccccccccccccccccccc is code
 */
void YasLexer::print_code(FILE *out, int pos)
{
   char outstring[33];
   if (pos > 0xFFF)
   {
      if (m_tokens.size())
      {
         int i;
         if (pos > 0xFFFF)
         {
            fail("Code address limit exceeded");
            exit(1);
         }
         snprintf(outstring, sizeof(outstring), "0x0000:                      | ");
         hexstuff(outstring + 2, pos, 4);
         for (i = 0; i < bcount; i++)
            hexstuff(outstring + 7 + 2 * i, code[i] & 0xFF, 2);
      }
      else
         snprintf(outstring, sizeof(outstring), "                             | ");
   }
   else
   {
      if (m_tokens.size())
      {
         int i;
         if (pos > 0xFFF)
         {
            fail("Code address limit exceeded");
            exit(1);
         }
         snprintf(outstring, sizeof(outstring), "0x000:                      | ");
         hexstuff(outstring + 2, pos, 3);
         for (i = 0; i < bcount; i++)
            hexstuff(outstring + 7 + 2 * i, code[i] & 0xFF, 2);
      }
      else
         snprintf(outstring, sizeof(outstring), "                            | ");
   }
   fprintf(out, "%s%s\n", outstring, m_curLine.c_str());
}
