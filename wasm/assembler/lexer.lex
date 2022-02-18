/* Grammar for Y86-64 Assembler */
%{
#include "lexer.h"
#define YY_DECL int yaslex(YasLexer *)
%}

%option noinput
%option nounput
%option noyywrap

Instr         rrmovq|cmovle|cmovl|cmove|cmovne|cmovge|cmovg|rmmovq|mrmovq|irmovq|addq|subq|andq|xorq|jmp|jle|jl|je|jne|jge|jg|call|ret|pushq|popq|"."byte|"."word|"."long|"."quad|"."pos|"."align|halt|nop|iaddq
Letter        [a-zA-Z]
Digit         [0-9]
Ident         {Letter}({Letter}|{Digit}|_)*
Hex           [0-9a-fA-F]
Blank         [ \t]
Newline       [\n\r]
Return        [\r]
Char          [^\n\r]
Reg           %rax|%rcx|%rdx|%rbx|%rsi|%rdi|%rsp|%rbp|%r8|%r9|%r10|%r11|%r12|%r13|%r14

/* ERR condition is started if no token is matched */
%x ERR
%%

^{Char}*{Return}*{Newline}      {LBEE_YAS::save_line(yytext); REJECT;} /* Snarf input line */
#{Char}*{Return}*{Newline}      {LBEE_YAS::finish_line();}
"//"{Char}*{Return}*{Newline}   {LBEE_YAS::finish_line();}
"/*"{Char}*{Return}*{Newline}   {LBEE_YAS::finish_line();}
{Blank}*{Return}*{Newline}      {LBEE_YAS::finish_line();}

{Blank}+                        ;
"$"+                            ;
{Instr}                         LBEE_YAS::add_instr(yytext);
{Reg}                           LBEE_YAS::add_reg(yytext);
[-]?{Digit}+                    LBEE_YAS::add_num(atoll(yytext));
"0"[xX]{Hex}+                   LBEE_YAS::add_num(strtoull(yytext, nullptr, 16));
[():,]                          LBEE_YAS::add_punct(*yytext);
{Ident}                         LBEE_YAS::add_ident(yytext);
{Char}                          {; BEGIN ERR;}
<ERR>{Char}*{Newline}           {LBEE_YAS::fail("Invalid line"); BEGIN 0;}
%%
