#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include "./Gramma_analysis.h"

//lvar为左值，opl为左操作数，opr为右操作数
enum IPOS{lvar,opl,opr};
// == > >= < <= != EQ,GT,GE,LT,LE,NEQ
enum OPERATOR{NEG,ADD,SUB,MUL,DIV,MOV,EQ,GT,GE,LT,LE,NEQ};

//用来定位基本块中变量的位置
struct Pos{
    int lno;//行号
    IPOS ipos;//代码中的具体位置
};

//用来优化的变量结构体
struct OVar{
    string name;//变量名
    int procno;//属于的过程编号
    vector<Pos> pos;//代码块中的位置，从小到大
};

//基本块
struct CODE_BLOCK{
    int BeginIndex;//起始的代码序号
    vector<Code> code;//基本块中的中间代码
    vector<OVar> varlist;//变量列表
    vector<OVar> Outvarlist;//出基本块还需要使用的变量列表
    vector<CODE_BLOCK&> NextBlock;//下一个程序块，构造程序流图需要使用
};

//DAG图的结点
struct DAGNode{
    OVar Dval;//定值，结点右边的值
    vector<DAGNode &> Opval;//操作数
    OPERATOR op;//操作类型
};


class Optimizer{
private:
    vector<Var> VarTable;//变量表
    vector<Func> FuncTable;//函数表
    vector<Code> MiddleCodeTable;//中间代码表
    vector<CODE_BLOCK> CodeBlock;//代码基本块
    vector<Code> OptimCodeTable;//优化后的中间代码表
public:
    //初始化优化器，使用变量表，函数表以及中间代码表即可进行优化
    Optimizer(vector<Var> VarTable1,vector<Func> FuncTable1,vector<Code> MiddleCodeTable1);
    //划分基本块
    void Divblocks();
    //构建程序流图
    void CreateGraph();
    //出基本块还需要使用的变量列表
    void FindOutVar();
    //对每一个基本块构造DAG图
    void CreateDAG();
    //生成优化后的中间代码
    void GenOCode();
    //删除未被使用的变量的代码
    void DelUnuseCode();
};



#endif // OPTIMIZER_H
