/* Grammar for Y86-64 Assembler */
%{
#include "lexer.h"
#define YY_DECL int yaslex(YasLexer *lexer)
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

^{Char}*{Return}*{Newline}      {lexer->loadLine(yytext); REJECT;} /* Snarf input line */
#{Char}*{Return}*{Newline}      {lexer->processTokens();}
"//"{Char}*{Return}*{Newline}   {lexer->processTokens();}
"/*"{Char}*{Return}*{Newline}   {lexer->processTokens();}
{Blank}*{Return}*{Newline}      {lexer->processTokens();}

{Blank}+                        ;
"$"+                            ;
{Instr}                         lexer->addInstr(yytext);
{Reg}                           lexer->addReg(yytext);
[-]?{Digit}+                    lexer->addNum(atoll(yytext));
"0"[xX]{Hex}+                   lexer->addNum(strtoull(yytext, nullptr, 16));
[():,]                          lexer->addPunct(*yytext);
{Ident}                         lexer->addIdent(yytext);
{Char}                          {; BEGIN ERR;}
<ERR>{Char}*{Newline}           {lexer->error("Invalid line"); BEGIN 0;}
%%
