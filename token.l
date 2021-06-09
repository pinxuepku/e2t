%{
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "tree.hpp"
#include "grammar.tab.hpp"
static FILE* yyparse_file_ptr;
%}

%option noyywrap yylineno

digit       [0-9]
number      "-"?{digit}+
label       l{number}
letter      [a-zA-Z_]
identifier  {letter}({letter}|{digit})*
variable    [Ttp]{number}
function    f_{identifier}


%%
{label}       {yylval.str=yytext; return T_LABEL; }
{number}      {yylval.str=yytext; return T_NUM;}
{variable}    {yylval.str=yytext; return T_IDENT;}
{function}    {yylval.str=yytext; return T_FUNC; }
"if"	      {return T_IF;}
"var"         {return T_VAR;}
"return"      {return T_RETURN;} 
"goto"	      {return T_GOTO;}
"call"        {return T_CALL;}
"end"	      {return T_END;}
"param"	      {return T_PARAM;}
[ \t\n]+      ;
"!="          {return T_NE;} 
"=="          {return T_EQ;} 
"="           {return T_ASSIGN;} 
"+"           {return T_ADD;}
"-"           {return T_SUB;}
"*"           {return T_MUL;}
"/"           {return T_DIV;}
"%"           {return T_MOD;}
"&&"          {return T_AND;}
"||"          {return T_OR;}
">"           {return T_GT;}
"<"           {return T_LT;}
">="          {return T_GE;}
"<="          {return T_LE;}
"!"           {return T_NOT;}
":"           {return T_MAOHAO;}
"["           {return T_LFANGKUOHAO;}
"]"           {return T_RFANGKUOHAO;}
"//"[^\n]*    {}
%%


