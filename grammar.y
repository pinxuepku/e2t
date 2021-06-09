%{
	#include "tree.hpp"
	#include <iostream>
	#include <stdio.h>
	#include <string>
	#include <vector>
	#include <stdlib.h>
	#include <string.h>


	using namespace std;
	Node* root;
	extern int yylex();
	extern FILE* yyin;
	extern FILE* yyout;
	extern int yylineno;
	void yyerror(const char* s)
	{
		printf("Error: %s\n", s);
	}
	


%}
%union
{
	Node* block;
	int token;
	char* str;
}

%token <token> T_LABEL T_NUM T_IDENT T_FUNC T_IF  T_VAR T_RETURN T_GOTO
%token <token> T_CALL T_RFANGKUOHAO T_LFANGKUOHAO T_PARAM T_END T_NE T_EQ T_ASSIGN
%token <token> T_ADD T_SUB T_MUL T_DIV T_MOD T_AND T_OR T_GT T_LT T_NOT T_MAOHAO T_GE T_LE

%type <block> program declare initial funcdef num ident funcname statements expression
%type <block> rightval binop op logicop label 

%start program

%%
program :   program declare{
					$$=$1;
					$2->parent=$$;
					$$->mem.push_back($2);
					root=$$;
				}
			
		|	program initial{
					$$=$1;
					$2->parent=$$;
					$$->mem.push_back($2);
					root=$$;
				}
		|	program funcdef{
					$$=$1;
					$2->parent=$$;
					$$->mem.push_back($2);
					root=$$;
				}
		|	declare{
					$$=new Node_Program();
					$1->parent=$$;
					$$->mem.push_back($1);
					root=$$;
				}
		|	initial{
					$$=new Node_Program();
					$1->parent=$$;
					$$->mem.push_back($1);
					root=$$;
				}
		|	funcdef{
					$$=new Node_Program();
					$1->parent=$$;
					$$->mem.push_back($1);
					root=$$;
				}
		|    {
				$$=new Node_Program();
				root=$$;
			}	
		
		;

declare : T_VAR ident{
			$$=new Node_Declare();
			$2->parent=$$;
			$$->children[0]=$2;
			}
		| T_VAR num ident{
			$$=new Node_Declare();
			$2->parent=$$;
			$3->parent=$$;
			$$->children[0]=$3;
			$$->children[1]=$2;
			}
		;
	
initial : ident T_ASSIGN num{
			$$=new Node_Initial();
			$1->parent=$$;
			$3->parent=$$;
			$$->children[0]=$1;
			$$->children[1]=$3;
			}
		| ident T_LFANGKUOHAO num T_RFANGKUOHAO T_ASSIGN num{
			$$=new Node_Initial();
			$1->parent=$$;
			$3->parent=$$;
			$6->parent=$$;
			}
		;

funcdef : funcname T_LFANGKUOHAO num T_RFANGKUOHAO statements T_END funcname{
			$$=new Node_FuncDef();
			$1->parent=$$;
			$3->parent=$$;
			$5->parent=$$;
			$7->parent=$$;
			$$->children[0]=$1;
			$$->children[1]=$3;
			$$->children[2]=$5;
			$$->children[3]=$7;
		}
		;

statements : statements declare{
			$$=$1;
			$2->parent=$$;
			int len=$1->mem.size();
			$2->pre=$1->mem[len-1];
			$$->mem.push_back($2);
		}
		| statements expression{
			$$=$1;
			$2->parent=$$;
			int len=$1->mem.size();
			$2->pre=$1->mem[len-1];
			$$->mem.push_back($2);
		}
		| expression{
			$$=new Node_Statements();
			$1->parent=$$;
			$1->pre=NULL;
			$$->mem.push_back($1);
		}
		| declare{
			$$=new Node_Statements();
			$1->parent=$$;
			$1->pre=NULL;
			$$->mem.push_back($1);
		}
		| {}
		;

expression :  ident T_ASSIGN rightval binop rightval{
			$$=new Node_Expression();
			$1->parent=$$;
			$3->parent=$$;
			$4->parent=$$;
			$5->parent=$$;
			$$->children[0]=$1;
			$$->children[1]=$3;
			$$->children[2]=$4;
			$$->children[3]=$5;
			$$->exp_type=1;
		}
		| ident T_ASSIGN op rightval{
			$$=new Node_Expression();
			$1->parent=$$;
			$3->parent=$$;
			$4->parent=$$;
			$$->children[0]=$1;
			$$->children[1]=$3;
			$$->children[2]=$4;
			$$->exp_type=2;
		}
		| ident T_ASSIGN rightval{
			$$=new Node_Expression();
			$1->parent=$$;
			$3->parent=$$;
			$$->children[0]=$1;
			$$->children[1]=$3;
			$$->exp_type=3;
		}
		| ident T_LFANGKUOHAO rightval T_RFANGKUOHAO T_ASSIGN rightval{
			$$=new Node_Expression();
			$1->parent=$$;
			$3->parent=$$;
			$6->parent=$$;
			$$->children[0]=$1;
			$$->children[1]=$3;
			$$->children[2]=$6;
			$$->exp_type=4;
		}
		| ident T_ASSIGN ident T_LFANGKUOHAO rightval T_RFANGKUOHAO{
			$$=new Node_Expression();
			$1->parent=$$;
			$3->parent=$$;
			$5->parent=$$;
			$$->children[0]=$1; 
			$$->children[1]=$3;
			$$->children[2]=$5;
			$$->exp_type=5;
		} 
		|T_IF rightval logicop rightval T_GOTO label{
			$$=new Node_Expression();
			$2->parent=$$;
			$3->parent=$$;
			$4->parent=$$;
			$6->parent=$$;
			$$->children[0]=$2; 
			$$->children[1]=$3;
			$$->children[2]=$4;
			$$->children[3]=$6;
			$$->exp_type=6;
		} 
		|T_GOTO label{
			$$=new Node_Expression();
			$2->parent=$$;
			$$->children[0]=$2;
			$$->exp_type=7;
		}
		| label T_MAOHAO{
			$$=new Node_Expression();
			$1->parent=$$;
			$$->children[0]=$1;
			$$->exp_type=8;
		} 
		|T_PARAM rightval{
			$$=new Node_Expression();
			$2->parent=$$;
			$$->children[0]=$2;
			$$->exp_type=9;
		}
		| T_CALL funcname{
			$$=new Node_Expression();
			$2->parent=$$;
			$$->children[0]=$2;
			$$->exp_type=10;
		}
		| ident T_ASSIGN T_CALL funcname{
			$$=new Node_Expression();
			$1->parent=$$;
			$4->parent=$$;
			$$->children[0]=$1;
			$$->children[1]=$4;
			$$->exp_type=11;
		}
		| T_RETURN rightval{
			$$=new Node_Expression();
			$2->parent=$$;
			$$->children[0]=$2;
			$$->exp_type=12;
		}
		| T_RETURN{
			$$=new Node_Expression();
			$$->exp_type=13;
		} 
		;

rightval : ident{
			$$=new Node_Rightval();
			$$->children[0]=$1;
			$1->parent=$$;
			$$->rv_type=1;
		}
		| num{
			$$=new Node_Rightval();
			$$->children[0]=$1;
			$1->parent=$$;
			$$->rv_type=2;
			//cout<<"rv"<<endl;
			//cout<<$1->val<<endl;
		}
		;

ident : T_IDENT{
			$$=new Node_Ident();
			$$->name=string(yylval.str);
		}
		;
binop : op{
			$$=new Node_Binop();
			$$->name=$1->name;
		}
		| logicop{
			$$=new Node_Binop();
			$$->name=$1->name;
		}
		;
op : T_NOT{
			$$=new Node_Op();
			$$->name=string("!");
		}	
		| T_SUB{
			$$=new Node_Op();
			$$->name=string("-");	
		}
		| T_ADD{
			$$=new Node_Op();
			$$->name=string("+");
		}
		| T_MUL{
			$$=new Node_Op();
			$$->name=string("*");
		}
		| T_DIV{
			$$=new Node_Op();
			$$->name=string("/");
		}
		| T_MOD{
			$$=new Node_Op();
			$$->name=string("%");
		}
		;
logicop : T_AND{
			$$=new Node_Logicop();
			$$->name=string("&&");
		}
		| T_OR{
			$$=new Node_Logicop();
			$$->name=string("||");
		} 
		| T_EQ{
			$$=new Node_Logicop();
			$$->name=string("==");
		}
		| T_NE{
			$$=new Node_Logicop();
			$$->name=string("!=");
		}
		| T_GT{
			$$=new Node_Logicop();
			$$->name=string(">");
		}
		| T_LT{
			$$=new Node_Logicop();
			$$->name=string("<");
		}
		| T_GE{
			$$=new Node_Logicop();
			$$->name=string(">=");
		}
		| T_LE{
			$$=new Node_Logicop();
			$$->name=string("<=");
		}
		;

num : T_NUM{
				$$=new Node_Num();
				$$->val=atoi(yylval.str);
				//cout<<"num"<<endl;
			}
		;

funcname : T_FUNC{
			//cout<<"here"<<endl;
			$$=new Node_Funcname();
			$$->name=string(yylval.str);
		}
		;
label : T_LABEL{
			$$=new Node_Label();
			$$->name=string(yylval.str);
		}
		;



%%
int main(int argc, char** argv){
	yyin=fopen(argv[3],"r");
	freopen(argv[5],"w",stdout);
	//cout<<-1<<endl;
	yyparse();
	//printTree(root);
	//cout<<"here"<<endl;
	//cout<<0<<endl;
	init_symtab(root,global);
	//cout<<1<<endl;
	codeGen(root,global);
	//cout<<2<<endl;
	return 0;
}