#ifndef __TREE_H__
#define __TREE_H__

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <bitset>
#include <string.h>
#include "sym.hpp"

using namespace std;

#define NUM_CHILDREN_MAX 10

#define NODE_TYPE_PROGRAM 100
#define NODE_TYPE_DECLARE 101
#define NODE_TYPE_INITIAL 102
#define NODE_TYPE_FUNCDEF 103
#define NODE_TYPE_NUM 104
#define NODE_TYPE_IDENT 105
#define NODE_TYPE_FUNCNAME 106
#define NODE_TYPE_STATEMENTS 107
#define NODE_TYPE_EXPRESSION 109
#define NODE_TYPE_RIGHTVAL 110
#define NODE_TYPE_BINOP 111
#define NODE_TYPE_OP 112
#define NODE_TYPE_LOGICOP 113
#define NODE_TYPE_LABEL 114


static int record[28]={0};
static string regs[28]={
    "x0",  /* 0 */
    "s0","s1","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11",/* 1-12  */
    "t0","t1","t2","t3","t4","t5","t6", /* 13-19*/
    "a0","a1","a2","a3","a4","a5","a6","a7" /*20-27*/
};





class Node {
public:
    int rv_type;
    bitset<500> use;      
    bitset<500> def;
    bitset<500> live;
    Node* next1;
    Node* next2;
    Node* pre;
    int stack_size;
    vector<Symbol*>local_symtab;
    int exp_type;
    int node_type;
    int num_children;
    string name;
    int val;
    Node* parent;
    Node* my_func;
    vector<Node*>mem;
    Node* children[NUM_CHILDREN_MAX];
    Node()
    {
        stack_size=0;//for funcdef
        val = -1;
        node_type = -1;
        num_children = 0;
        parent = NULL;
        mem = vector<Node*>();
        local_symtab=vector<Symbol*>();
        my_func = NULL;
    }

};

void printTree(Node*);
void init_symtab(Node*,vector<Symbol*>&);

static int global_var_num=0;
static int param_count=0;

void init_link(Node*);
void liveness(Node*);
int get_index(Node*,string); 
void delete_reg(Node*, int,int);
int allocate(Node*,int);
void codeGen(Node*,vector<Symbol*>&);
void codeGen_global_declare(Node*,vector<Symbol*>&);
void codeGen_1(Node*, Node*, int,vector<Symbol*>&);
void codeGen_2(Node*, Node*, int,vector<Symbol*>&);
void codeGen_3(Node*, Node*, int,vector<Symbol*>&);
void codeGen_4(Node*, Node*, int,vector<Symbol*>&);
void codeGen_5(Node*, Node*, int,vector<Symbol*>&);
void codeGen_6(Node*, Node*, int,vector<Symbol*>&);
void codeGen_7(Node*, Node*, int,vector<Symbol*>&);
void codeGen_8(Node*, Node*, int,vector<Symbol*>&);
void codeGen_9(Node*, Node*, int,vector<Symbol*>&);
void codeGen_10(Node*, Node*, int,vector<Symbol*>&);
void codeGen_11(Node*, Node*, int,vector<Symbol*>&);
void codeGen_12(Node*, Node*, int,vector<Symbol*>&);
void codeGen_13(Node*, Node*, int,vector<Symbol*>&);






class Node_Program:public Node {
public:
    Node_Program()
    {
        val = -1;
        node_type = NODE_TYPE_PROGRAM;
        num_children = 0;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Declare: public Node {
public:
    Node_Declare()
    {
        val = -1;
        node_type = NODE_TYPE_DECLARE;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Initial: public Node {
public:
    Node_Initial()
    {
        val = -1;
        node_type = NODE_TYPE_INITIAL;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_FuncDef: public Node {
public:
    Node_FuncDef()
    {
        val = -1;
        node_type = NODE_TYPE_FUNCDEF;
        num_children = 8;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Num : public Node{
public:
    Node_Num()
    {
        val = -1;
        node_type = NODE_TYPE_NUM;
        num_children = 0;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Ident : public Node{
public:
    Node_Ident()
    {
        val = -1;
        node_type = NODE_TYPE_IDENT;
        num_children = 0;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Funcname: public Node {
public:
    Node_Funcname()
    {
        val = -1;
        node_type = NODE_TYPE_FUNCNAME;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Statements: public Node {
public:
    Node_Statements()
    {
        val = -1;
        node_type = NODE_TYPE_STATEMENTS;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Expression : public Node{
public:
    Node_Expression()
    {
        val = -1;
        node_type = NODE_TYPE_EXPRESSION;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Rightval : public Node{
public:
    Node_Rightval()
    {
        val = -1;
        node_type = NODE_TYPE_RIGHTVAL;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Binop : public Node{
public:
    Node_Binop()
    {
        val = -1;
        node_type = NODE_TYPE_BINOP;
        num_children = 0;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Logicop: public Node {
public:
    Node_Logicop()
    {
        val = -1;
        node_type = NODE_TYPE_LOGICOP;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Op : public Node{
public:
    Node_Op()
    {
        val = -1;
        node_type = NODE_TYPE_OP;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

class Node_Label : public Node {
public:
    Node_Label()
    {
        val = -1;
        node_type = NODE_TYPE_LABEL;
        num_children = 3;
        parent = NULL;
        mem = vector<Node*>();
        my_func = NULL;
    }

};

#endif