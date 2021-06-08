#include "tree.hpp"
using namespace std;

void printTree(Node* r){
    if(r!=NULL){
        cout<<r->node_type<<endl;
    }
    if(r->children[0]!=NULL){
        for(int i=0;i<10;i++){
            if(r->children[i]!=NULL){
                printTree(r->children[i]);
            }
        }
    }
    if(r->mem.size()!=0){
        int len=r->mem.size();
        for(int i=0;i<len;i++){
            printTree(r->mem[i]);
        }
    }
}

int get_index(Node* func, string name){
    int len=func->local_symtab.size();
    for(int i=0;i<len;i++){
        if(name==func->local_symtab[i]->name){
            return i;
        }
    }
    //cout<<158<<" "<<name<<endl;
    cout<<"error: no such variable"<<endl;
    exit(-1);
    //return 0;
}

void init_symtab(Node* r,vector<Symbol*>&global){
    //从root开始
    int len=r->mem.size();
    for(int i=0;i<len;i++){
        if(r->mem[i]->node_type==NODE_TYPE_DECLARE){
            //cout<<"here"<<endl;
            Symbol* sym=new Symbol();
            if(r->mem[i]->children[1]==NULL){
                // var t0
                sym->name=r->mem[i]->children[0]->name;
                sym->is_global=1;
                sym->global_index=global_var_num;
                global_var_num++;
                sym->is_array=0;
                global.push_back(sym);
            }else{
                // var 40 t0
                sym->name=r->mem[i]->children[0]->name;
                sym->is_global=1;
                sym->global_index=global_var_num;
                global_var_num++;
                sym->is_array=1;
                sym->array_size=r->mem[i]->children[1]->val;
                global.push_back(sym);
            }
        }else if(r->mem[i]->node_type==NODE_TYPE_FUNCDEF){
            //funcdef
            Node* func=r->mem[i];
            int len=global.size();
            for(int i=0;i<len;i++){
                Symbol* sym=new Symbol();
                sym->name=global[i]->name;
                sym->is_global=1;
                sym->global_index=global[i]->global_index;
                sym->stack_location=func->stack_size;
                sym->is_array=global[i]->is_array;
                sym->array_size=global[i]->array_size;
                sym->param_index=0;
                sym->reg=0;
                func->stack_size+=1;
                func->local_symtab.push_back(sym);
            }
            int param_num=func->children[1]->val;
            for(int i=0;i<param_num;i++){
                Symbol* sym=new Symbol();
                sym->name=string("p")+to_string(i);
                sym->is_global=0;
                sym->stack_location=func->stack_size;
                sym->param_index=i+1; //不能为0
                func->stack_size+=1;
                sym->reg=0;
                func->local_symtab.push_back(sym);
            }
            //处理完了全局变量和函数参数，现在开始处理函数体
            Node* statements=func->children[2];
            if(statements->mem.size()==0){
                continue;
            }
            else{
                int len2=statements->mem.size();
                for(int i=0;i<len2;i++){
                    if(statements->mem[i]->node_type==NODE_TYPE_DECLARE){
                        Symbol* sym=new Symbol();
                        sym->name=statements->mem[i]->children[0]->name;
                        sym->is_global=0;
                        if(statements->mem[i]->children[1]==NULL){
                            sym->stack_location=func->stack_size;
                            func->stack_size+=1;
                            sym->is_array=0;
                        }else{
                            sym->stack_location=func->stack_size;
                            sym->is_array=1;
                            sym->array_size=statements->mem[i]->children[1]->val;
                            func->stack_size+=sym->array_size/4;
                        }
                        func->local_symtab.push_back(sym);
                    }
                }
            }
        }
    }
}


void codeGen(Node* r,vector<Symbol*>&global){
    int len=r->mem.size();
    for(int i=0;i<len;i++){
        if(r->mem[i]->node_type==NODE_TYPE_DECLARE){
            codeGen_global_declare(r->mem[i],global);
            continue;
        }
        if(r->mem[i]->node_type==NODE_TYPE_FUNCDEF){
            Node* func=r->mem[i];
            Node* statements=func->children[2];
            int len2=statements->mem.size();
            memset(record,-1,sizeof(record));
            int sym_num=func->local_symtab.size();
            for(int j=0;j<sym_num;j++){
                if(func->local_symtab[j]->param_index>0){
                    allocate(func,j);
                }
            }//先把参数都处理掉
            cout<<func->children[0]->name<<" ["<<func->children[1]->val<<"]["<<func->stack_size<<"]"<<endl;
            cout<<endl;
            for(int j=0;j<len2;j++){
                if(statements->mem[j]->node_type==NODE_TYPE_DECLARE){
                    continue;
                }
                Node* st=statements->mem[j];
                /*
                for(int k=0;k<sym_num;k++){
                    if(st->live[k]==1 && func->local_symtab[k]->reg==0){
                        //cout<<"dijiju:"<<j<<endl;
                        int alloc_reg=allocate(func,k);
                        if(alloc_reg>=9 && alloc_reg<=11){
                            break;
                        }
                        if(func->local_symtab[k]->is_global==1){
                            if(func->local_symtab[k]->array_size==0){
                                cout<<"load v"<<func->local_symtab[k]->global_index<<" "<<regs[alloc_reg]<<endl;
                            }else{
                                cout<<"loadaddr v"<<func->local_symtab[k]->global_index<<" "<<regs[alloc_reg]<<endl;
                            }
                        }else{
                            if(func->local_symtab[k]->array_size==0){
                                cout<<"load "<<func->local_symtab[k]->stack_location<<" "<<regs[alloc_reg]<<endl;
                            }else{
                                cout<<"loadaddr "<<func->local_symtab[k]->stack_location<<" "<<regs[alloc_reg]<<endl;
                            }
                        }
                    }
                }
                delete_reg(func,j,record[9]);
                delete_reg(func,j,record[10]);
                delete_reg(func,j,record[11]);
                */
                if(st->exp_type==1){
                    codeGen_1(func,st,j,global);
                }else if(st->exp_type==2){
                    codeGen_2(func,st,j,global);
                }else if(st->exp_type==3){
                    codeGen_3(func,st,j,global);
                }else if(st->exp_type==4){
                    codeGen_4(func,st,j,global);
                }else if(st->exp_type==5){
                    codeGen_5(func,st,j,global);
                }else if(st->exp_type==6){
                    codeGen_6(func,st,j,global);
                }else if(st->exp_type==7){
                    codeGen_7(func,st,j,global);
                }else if(st->exp_type==8){
                    codeGen_8(func,st,j,global);
                }else if(st->exp_type==9){
                    codeGen_9(func,st,j,global);
                }else if(st->exp_type==10){
                    codeGen_10(func,st,j,global);
                }else if(st->exp_type==11){
                    codeGen_11(func,st,j,global);
                }else if(st->exp_type==12){
                    codeGen_12(func,st,j,global);
                }else if(st->exp_type==13){
                    codeGen_13(func,st,j,global);
                }
            }
            cout<<"end "+func->children[0]->name<<endl;
        }
    }
}

void codeGen_global_declare(Node* r,vector<Symbol*>&global){
    int i=0;
    int len=global.size();
    for(i;i<len;i++){
        if(global[i]->name==r->children[0]->name){
            break;
        }
    }
    if(r->children[1]==NULL){
        cout<<"v"<<global[i]->global_index<<" = 0"<<endl;
    }else{
        cout<<"v"<<global[i]->global_index<<" = malloc "<<global[i]->array_size<<endl;
    }
    cout<<endl;
}

int allocate(Node* r, int id){
    if(r->local_symtab[id]->param_index>0){
        //是参数
        int index=r->local_symtab[id]->param_index+19;
        record[index]=id;
        r->local_symtab[id]->reg=index;
        return index;
    }
    for(int i=13;i<20;i++){
        if(record[i]==-1){
            record[i]=id;
            return i;
        }
    }
    for(int i=1;i<12;i++){
        if(record[i]==-1){
            record[i]=id;
            return i;
        }
    }
    cout<<"bukeneng!"<<endl;
    return 0;
}

void clear(){
    for(int i=0;i<20;i++){
        record[i]=-1;
    }
}

void codeGen_13(Node* func, Node* st, int index,vector<Symbol*>&global){
    cout<<"return"<<endl;
}


void codeGen_1(Node* func, Node* st, int index,vector<Symbol*>&global){
    //ident T_ASSIGN rightval binop rightval
    int i0=get_index(func,st->children[0]->name);
    int reg0=func->local_symtab[i0]->reg;
    if(reg0==0){ // 说明不是参数，可能是全局变量或者局部变量
        reg0=allocate(func,i0);
        if(func->local_symtab[i0]->is_global==1){
            cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
        }else{
            cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
        }
    }
    string op=st->children[2]->name;
    if(st->children[1]->rv_type==1 && st->children[3]->rv_type==1){
        //ident
        int i1=get_index(func,st->children[1]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        if(reg1==0){ // 说明不是参数，可能是全局变量或者局部变量
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        int i2=get_index(func,st->children[3]->children[0]->name);
        int reg2=func->local_symtab[i2]->reg;
        if(reg2==0){ // 说明不是参数，可能是全局变量或者局部变量
            reg2=allocate(func,i2);
            if(func->local_symtab[i2]->is_global==1){
                cout<<"load v"<<func->local_symtab[i2]->global_index<<" "<<regs[reg2]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i2]->stack_location<<" "<<regs[reg2]<<endl;
            }
        }
        cout<<regs[reg0]<<" = "<<regs[reg1]<<" "<<op<<" "<<regs[reg2]<<endl;
    }else if(st->children[1]->rv_type==1 && st->children[3]->rv_type==2){
        int i1=get_index(func,st->children[1]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        if(reg1==0){ // 说明不是参数，可能是全局变量或者局部变量
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        cout<<"s11 = "<<st->children[3]->children[0]->val<<endl;
        cout<<regs[reg0]<<" = "<<regs[reg1]<<" "<<op<<" "<<"s11"<<endl;
        
    }else if(st->children[1]->rv_type==2 && st->children[3]->rv_type==1){
        int i2=get_index(func,st->children[3]->children[0]->name);
        int reg2=func->local_symtab[i2]->reg;
        if(reg2==0){
            reg2=allocate(func,i2);
            if(func->local_symtab[i2]->is_global==1){
                cout<<"load v"<<func->local_symtab[i2]->global_index<<" "<<regs[reg2]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i2]->stack_location<<" "<<regs[reg2]<<endl;
            }
        }
        cout<<"s11 = "<<st->children[1]->children[0]->val<<endl;
        cout<<regs[reg0]<<" = s11 "<<op<<" "<<regs[reg2]<<endl;
    }else if(st->children[1]->rv_type==2 && st->children[3]->rv_type==2){
        cout<<"s10 = "<<st->children[1]->children[0]->val<<endl;
        cout<<"s11 = "<<st->children[3]->children[0]->val<<endl;
        cout<<regs[reg0]<<" = s10 "<<op<<" s11"<<endl;
    }
    clear();
    if(func->local_symtab[i0]->is_global==1){
        cout<<"loadaddr v"<<func->local_symtab[i0]->global_index<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }else if(reg0>=1&&reg0<=19){
        cout<<"loadaddr "<<func->local_symtab[i0]->stack_location<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }
    cout<<endl;
}

void codeGen_2(Node* func, Node* st, int index,vector<Symbol*>&global){
    //ident T_ASSIGN op rightval
    int i0=get_index(func,st->children[0]->name);
    int reg0=func->local_symtab[i0]->reg;
    if(reg0==0){ // 说明不是参数，可能是全局变量或者局部变量
        reg0=allocate(func,i0);
        if(func->local_symtab[i0]->is_global==1){
            cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
        }else{
            cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
        }
    }
    string op=st->children[1]->name;
    if(st->children[2]->rv_type==1){
        int i1=get_index(func,st->children[2]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        if(reg1==0){
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        cout<<regs[reg0]<<" = "<<op<<" "<<regs[reg1]<<endl;
    }else{
        int temp=0;
        if(st->children[1]->name=="!"){
            temp=(int)(!st->children[2]->children[0]->val);
        }else if(st->children[1]->name=="-"){
            temp=(int)(-st->children[2]->children[0]->val);
        }
        cout<<regs[reg0]<<" = "<<temp<<endl;
    }
    clear();
    if(func->local_symtab[i0]->is_global==1){
        cout<<"loadaddr v"<<func->local_symtab[i0]->global_index<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }else if(reg0>=1&&reg0<=19){
        cout<<"loadaddr "<<func->local_symtab[i0]->stack_location<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }
    cout<<endl;
}

void codeGen_3(Node* func, Node* st, int index,vector<Symbol*>&global){
    //ident T_ASSIGN rightval
    int i0=get_index(func,st->children[0]->name);
    int reg0=func->local_symtab[i0]->reg;
    if(reg0==0){ // 说明不是参数，可能是全局变量或者局部变量
        reg0=allocate(func,i0);
        if(func->local_symtab[i0]->is_global==1){
            cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
        }else{
            cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
        }
    }
    if(st->children[1]->rv_type==1){
        int i1=get_index(func,st->children[1]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        if(reg1==0){
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        cout<<regs[reg0]<<" = "<<regs[reg1]<<endl;
    }else{
        int temp=st->children[1]->children[0]->val;
        cout<<regs[reg0]<<" = "<<temp<<endl;
    }
    clear();
    if(func->local_symtab[i0]->is_global==1){
        cout<<"loadaddr v"<<func->local_symtab[i0]->global_index<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }else if(reg0>=1&&reg0<=19){
        cout<<"loadaddr "<<func->local_symtab[i0]->stack_location<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }
    cout<<endl;
}

void codeGen_4(Node* func, Node*st, int index,vector<Symbol*>&global){
    //ident T_LFANGKUOHAO rightval T_RFANGKUOHAO T_ASSIGN rightval
    int i0=get_index(func,st->children[0]->name);
    int reg0=func->local_symtab[i0]->reg;
    if(reg0==0){
        reg0=allocate(func,i0);
        if(func->local_symtab[i0]->is_global==1){
            cout<<"loadaddr v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
        }else{
            cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
        }//注意！！
    }

    if(st->children[1]->rv_type==1&&st->children[2]->rv_type==1){
        int i1=get_index(func,st->children[1]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        int i2=get_index(func,st->children[2]->children[0]->name);
        int reg2=func->local_symtab[i2]->reg;
        if(reg1==0){
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        if(reg2==0){
            reg2=allocate(func,i2);
            if(func->local_symtab[i2]->is_global==1){
                cout<<"load v"<<func->local_symtab[i2]->global_index<<" "<<regs[reg2]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i2]->stack_location<<" "<<regs[reg2]<<endl;
            }
        }

        cout<<"s11 = "<<regs[reg0]<<" + "<<regs[reg1]<<endl;
        cout<<"s11[0] = "<<regs[reg2]<<endl;
    }else if(st->children[1]->rv_type==2&&st->children[2]->rv_type==1){
        int i2=get_index(func,st->children[2]->children[0]->name);
        int reg2=func->local_symtab[i2]->reg;
        if(reg2==0){
            reg2=allocate(func,i2);
            if(func->local_symtab[i2]->is_global==1){
                cout<<"load v"<<func->local_symtab[i2]->global_index<<" "<<regs[reg2]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i2]->stack_location<<" "<<regs[reg2]<<endl;
            }
        }
        cout<<regs[reg0]<<"["<<st->children[1]->children[0]->val<<"] = "<<regs[reg2]<<endl;
    }else if(st->children[1]->rv_type==1&&st->children[2]->rv_type==2){
        int i1=get_index(func,st->children[1]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        if(reg1==0){
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        cout<<"s11 = "<<regs[reg0]<<" + "<<regs[reg1]<<endl;
        cout<<"s11[0] = "<<st->children[2]->children[0]->val<<endl;
    }else if(st->children[1]->rv_type==2&&st->children[2]->rv_type==2){
        cout<<"s11 = "<<st->children[2]->children[0]->val<<endl;
        cout<<regs[reg0]<<"["<<st->children[1]->children[0]->val<<"] = s11"<<endl;
    }
    clear();
    cout<<endl;
}

void codeGen_5(Node* func, Node* st, int index,vector<Symbol*>&global){
    //ident T_ASSIGN ident T_LFANGKUOHAO rightval T_RFANGKUOHAO
    int i0=get_index(func,st->children[0]->name);
    int reg0=func->local_symtab[i0]->reg;
    if(reg0==0){ // 说明不是参数，可能是全局变量或者局部变量
        reg0=allocate(func,i0);
        if(func->local_symtab[i0]->is_global==1){
            cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
        }else{
            cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
        }
    }
    int i1=get_index(func,st->children[1]->name);
    int reg1=func->local_symtab[i1]->reg;
    if(reg1==0){
        reg1=allocate(func,i1);
        if(func->local_symtab[i1]->is_global==1){
            cout<<"loadaddr v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
        }else{
            cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
        }//注意！
    }

    if(st->children[2]->rv_type==1){
        int i2=get_index(func,st->children[2]->children[0]->name);
        int reg2=func->local_symtab[i2]->reg;
        if(reg2==0){
            reg2=allocate(func,i2);
            if(func->local_symtab[i2]->is_global==1){
                cout<<"load v"<<func->local_symtab[i2]->global_index<<" "<<regs[reg2]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i2]->stack_location<<" "<<regs[reg2]<<endl;
            }
        }
        cout<<"s11 = "<<regs[reg1]<<" + "<<regs[reg2]<<endl;
        cout<<regs[reg0]<<" = s11[0]"<<endl;
    }else if(st->children[2]->rv_type==2){
        cout<<regs[reg0]<<" = "<<regs[reg1]<<"["<<st->children[2]->children[0]->val<<"]"<<endl;
    }
    clear();
    if(func->local_symtab[i0]->is_global==1){
        cout<<"loadaddr v"<<func->local_symtab[i0]->global_index<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }else if(reg0>=1&&reg0<=19){
        cout<<"loadaddr "<<func->local_symtab[i0]->stack_location<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }
    cout<<endl;
}

void codeGen_6(Node* func, Node* st, int index,vector<Symbol*>&global){
    //T_IF rightval logicop rightval T_GOTO label
    string op=st->children[1]->name;
    string label=st->children[3]->name;
    if(st->children[0]->rv_type==1&&st->children[2]->rv_type==1){
        int i0=get_index(func,st->children[0]->children[0]->name);
        int reg0=func->local_symtab[i0]->reg;
        if(reg0==0){ // 说明不是参数，可能是全局变量或者局部变量
            reg0=allocate(func,i0);
            if(func->local_symtab[i0]->is_global==1){
                cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
            }
        }
        int i1=get_index(func,st->children[2]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        if(reg1==0){ // 说明不是参数，可能是全局变量或者局部变量
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        cout<<"if "<<regs[reg0]<<" "<<op<<" "<<regs[reg1]<<" goto "<<label<<endl;
    }else if(st->children[0]->rv_type==2&&st->children[2]->rv_type==1){
        int i1=get_index(func,st->children[2]->children[0]->name);
        int reg1=func->local_symtab[i1]->reg;
        if(reg1==0){ // 说明不是参数，可能是全局变量或者局部变量
            reg1=allocate(func,i1);
            if(func->local_symtab[i1]->is_global==1){
                cout<<"load v"<<func->local_symtab[i1]->global_index<<" "<<regs[reg1]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i1]->stack_location<<" "<<regs[reg1]<<endl;
            }
        }
        cout<<"s11 = "<<st->children[0]->children[0]->val<<endl;
        cout<<"if s11 "<<op<<" "<<regs[reg1]<<" goto "<<label<<endl;
    }else if(st->children[0]->rv_type==1&&st->children[2]->rv_type==2){
        int i0=get_index(func,st->children[0]->children[0]->name);
        int reg0=func->local_symtab[i0]->reg;
        if(reg0==0){ // 说明不是参数，可能是全局变量或者局部变量
            reg0=allocate(func,i0);
            if(func->local_symtab[i0]->is_global==1){
                cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
            }
        }
        cout<<"s11 = "<<st->children[2]->children[0]->val<<endl;
        cout<<"if "<<regs[reg0]<<" "<<op<<" s11 goto "<<label<<endl;
    }else if(st->children[0]->rv_type==1&&st->children[2]->rv_type==2){
        cout<<"s11 = "<<st->children[2]->children[0]->val<<endl;
        cout<<"s10 = "<<st->children[0]->children[0]->val<<endl;
        cout<<"if s10 "<<op<<" s11 goto "<<label<<endl;
    }
    clear();
    cout<<endl;
}

void codeGen_7(Node* func, Node* st, int index,vector<Symbol*>&global){
    cout<<"goto "<<st->children[0]->name<<endl;
    cout<<endl;
}

void codeGen_8(Node* func, Node* st, int index,vector<Symbol*>&global){
    cout<<st->children[0]->name<<":"<<endl;
    cout<<endl;
}

void codeGen_9(Node* func, Node* st, int index,vector<Symbol*>&global){
    //T_PARAM rightval
    if(param_count==0){
        for(int i=20;i<=27;i++){
            if(record[i]!=-1){
                cout<<"store "<<regs[i]<<" "<<func->local_symtab[record[i]]->stack_location<<endl;
            }
        }
    }

    if(st->children[0]->rv_type==1){
        int i0=get_index(func, st->children[0]->children[0]->name);
        int reg0=func->local_symtab[i0]->reg;
        if(reg0==0){
            reg0=allocate(func,i0);
            if(func->local_symtab[i0]->is_global==1){
                if(func->local_symtab[i0]->is_array==0){
                    cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
                }else{
                    cout<<"loadaddr v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
                }
            }else{
                if(func->local_symtab[i0]->is_array==0){
                    cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
                }else{
                    cout<<"loadaddr "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
                }
            }
        }
        if(reg0>=20&&reg0<=27){//没想清楚
            cout<<"load "<<func->local_symtab[record[reg0]]->stack_location<<" "<<regs[reg0]<<endl;
        }else{
            cout<<regs[param_count+20]<<" = "<<regs[reg0]<<endl;
        }
    }else{
        cout<<regs[param_count]<<" = "<<st->children[0]->children[0]->val<<endl;
    }
    clear();
    param_count++;
    cout<<endl;
}

void codeGen_10(Node* func, Node* st, int index,vector<Symbol*>&global){
    //T_CALL funcname
    param_count=0;
    cout<<"call "<<st->children[0]->name<<endl;
    cout<<endl;
}

void codeGen_11(Node* func, Node* st, int index,vector<Symbol*>&global){
    param_count=0;
    //ident=call funcname
    int len=func->local_symtab.size();
   
    int i0=get_index(func,st->children[0]->name);
    int reg0=func->local_symtab[i0]->reg;
    if(reg0==0){ // 说明不是参数，可能是全局变量或者局部变量
        reg0=allocate(func,i0);
        if(func->local_symtab[i0]->is_global==1){
            cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
        }else{
            cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
        }
    }
    cout<<"call "<<st->children[1]->name<<endl;
    cout<<regs[reg0]<<" = a0"<<endl;

    if(func->local_symtab[i0]->is_global==1){
        cout<<"loadaddr v"<<func->local_symtab[i0]->global_index<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }else if(reg0>=1&&reg0<=19){
        cout<<"loadaddr "<<func->local_symtab[i0]->stack_location<<" s11"<<endl;
        cout<<"s11[0] = "<<regs[reg0]<<endl;
    }
    clear();
    
    for(int i=20;i<=27;i++){
        if(record[i]!=-1){
            cout<<"load "<<" "<<func->local_symtab[record[i]]->stack_location<<" "<<regs[i]<<endl;
        }
    }
    
    cout<<endl;
}

void codeGen_12(Node* func, Node*st, int index,vector<Symbol*>&global){
    //return rv
    if(st->children[0]->rv_type==2){
        cout<<"a0 = "<<st->children[0]->children[0]->val<<endl;
    }else{
        int i0=get_index(func,st->children[0]->children[0]->name);
        int reg0=func->local_symtab[i0]->reg;
        if(reg0==0){
            reg0=allocate(func,i0);
            if(func->local_symtab[i0]->is_global==1){
                cout<<"load v"<<func->local_symtab[i0]->global_index<<" "<<regs[reg0]<<endl;
            }else{
                cout<<"load "<<func->local_symtab[i0]->stack_location<<" "<<regs[reg0]<<endl;
            }
        }
        cout<<"a0 = "<<regs[reg0]<<endl;
    }
    clear();
    cout<<"return"<<endl;
    cout<<endl;
}

