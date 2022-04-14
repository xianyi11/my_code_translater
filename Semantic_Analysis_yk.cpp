#include "Gramma_analysis.h"
#include <windows.h>
using namespace std;


//在调用下面的函数的时候，产生式的右部已经pop出来并且存在了StorePop数组中

void GrammaAnalysis::GetStorePop(int len)
{
    StorePop.clear();
    for (int i = 0; i < len; i++)
    {
        StorePop.push_back(SemanticStack.top());
        SemanticStack.pop();
    }
}
////变量元素
//struct Var {
//	string name;//名字标识符
//	NameTableType type;//名字的类型
//	bool normal;//true为非形参，false为形参
//	int ival;
//	float fval;
//};

string GrammaAnalysis::MallocVar()
{
    Var temp;
    char name[50] = { 0 };
    sprintf(name, "T%d", MiddleVarIndex);
    MiddleVarIndex++;
    temp.name = name;
    temp.type = FLOAT1;
    temp.fval = 0;
    temp.ival = 0;
    temp.ProcNo = ProcNoStack[int(ProcNoStack.size())-1];
    VarTable.push_back(temp);
    return temp.name;
}



void GrammaAnalysis::Factor_1()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode Factor;
    SemanticTreeNode &Num = StorePop[0];
    Factor.Name = Num.Name;
    Factor.type = NonTerminator;
    Factor.normal = true;
    Factor.IsNull = 0;
    //Factor.content.push_back(Num);
//    Factor.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(Factor);
}

void GrammaAnalysis::Factor_2()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode Factor;
//    SemanticTreeNode Right_p= StorePop[0];
    SemanticTreeNode &Expr = StorePop[1];
//    SemanticTreeNode Left_p = StorePop[2];
    Factor.Name = Expr.Name;
    Factor.type = NonTerminator;
    Factor.normal = true;
    Factor.IsNull = 0;
    //Factor.content.push_back(Left_p);
    //Factor.content.push_back(Expr);
    //Factor.content.push_back(Right_p);
//    Factor.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(Factor);
}
void GrammaAnalysis::Factor_3()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode Factor;
    SemanticTreeNode &ID = StorePop[0];
    Factor.Name = ID.Name;
    Factor.type = NonTerminator;
    Factor.normal = true;
    Factor.IsNull = 0;
    //Factor.content.push_back(ID);
//    Factor.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(Factor);
}

void GrammaAnalysis::Factor_4()
{
    SemanticTreeNode Factor;
    int RightLen = 4;
    GetStorePop(RightLen);
    for (int i = RightLen - 1; i >= 0; i--)
        Factor.content.push_back(StorePop[i]);
    //检查函数名是否对应
    bool IsFind = false;
    int FuncIndex = 0;
    for (int i = 0; i < (int)FuncTable.size(); i++)
    {
        if (FuncTable[i].name == Factor.content[0].Name)
        {
            IsFind = true;
            FuncIndex = i;
            break;
        }
    }
    if (!IsFind)
    {
        semanticerror = NoFuncName;
        ErrorFuncName = Factor.content[0].Name;
        return;
    }
    //检查参数数量是否一致
    if (FuncTable[FuncIndex].param.size() != Factor.content[2].param.size())
    {
        ErrorFuncName = Factor.content[0].Name;
        semanticerror = FuncParLenError;
    }
    for (int i = (int)Factor.content[2].param.size() - 1; i >= 0; i--)//生成参数压栈中间代码
    {
        string VFactorName = GetMiddleName(Factor.content[2].param[i].name,Factor.content[2].param[i].dims);
        MiddleCodeTable.push_back(Code{"Par",VFactorName ,"-","-"});
    }
    string Temp = MallocVar();//申请中间变量，填到表中
    MiddleCodeTable.push_back(Code{ "=","@BackReturn","-",GetMiddleName(Temp) });

    MiddleCodeTable.push_back(Code{ "call",GetMiddleName(Factor.content[0].Name),"-","-" });

    Factor.Name = Temp;
    Factor.type = NonTerminator;
    Factor.normal = true;
    Factor.IsNull = 0;
//    Factor.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(Factor);
}

//<因子> ::= ID <数组>
//$语义: 检查下标是否正确
//$语义: 检查数组变量是否存在
void GrammaAnalysis::Factor_5()
{
    int RightLen = 2;
    GetStorePop(RightLen);
    SemanticTreeNode Factor;
    SemanticTreeNode &ID = StorePop[1];
    SemanticTreeNode &ArrayExpr = StorePop[0];
    Factor.Name = ID.Name;
    Factor.type = NonTerminator;
    Factor.normal = true;
    Factor.IsNull = 0;
    Factor.dems = ArrayExpr.dems;
    int error_num = CheckArrayTable(ID.Name,ArrayExpr.dems);
    if(error_num == -1){
        ErrorVarName=ID.Name;
        semanticerror = NoVar;
        return;
    }
    else if(error_num == -2)
    {
        ErrorVarName=ID.Name;
        semanticerror = ArrayIndexError;
        return;
    }
    SemanticStack.push(Factor);
}



void GrammaAnalysis::ArgumentList_1()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode ArgumentList;
    SemanticTreeNode &Expr = StorePop[0];
    ArgumentList.Name = "<实参列表>";
    ArgumentList.normal = true;
    ArgumentList.type = NonTerminator;
    ArgumentList.IsNull = 0;
    //ArgumentList.content.push_back(Expr);
    ArgumentList.param.push_back(Par{ FLOAT1,Expr.Name,Expr.dems });
//    ArgumentList.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(ArgumentList);
}

void GrammaAnalysis::ArgumentList_2()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode ArgumentList;
    SemanticTreeNode &Expr = StorePop[2];
//    SemanticTreeNode Dot = StorePop[1];
    SemanticTreeNode &Parlist = StorePop[0];

    ArgumentList.Name = "<实参列表>";
    ArgumentList.type = NonTerminator;
    ArgumentList.normal = true;
    ArgumentList.IsNull = 0;
    //ArgumentList.content.push_back(Expr);//无关紧要
    ArgumentList.param = Parlist.param;
    ArgumentList.param.push_back(Par{ FLOAT1,Expr.Name,Expr.dems });
//    ArgumentList.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(ArgumentList);
}


void GrammaAnalysis::ArgumentList_3()
{
    SemanticTreeNode ArgumentList;
    ArgumentList.Name = "<实参列表>";
    ArgumentList.IsNull = 1;
    SemanticStack.push(ArgumentList);
}

void GrammaAnalysis::Item_1()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode Item;
    SemanticTreeNode &Factor = StorePop[0];
    Item.Name = Factor.Name;
    Item.IsNull = 0;
    //Item.content.push_back(Factor);
    Item.normal = true;
    Item.type = NonTerminator;
    Item.dems = Factor.dems;
//    Item.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(Item);
}

void GrammaAnalysis::Item_23()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode Item;
    SemanticTreeNode &Factor = StorePop[2];
    SemanticTreeNode &CalSign = StorePop[1];
    SemanticTreeNode &LastItem = StorePop[0];
    Item.Name = MallocVar();//分配中间变量
    Item.IsNull = 0;
    //Item.content.push_back(Factor);
    //Item.content.push_back(Muti);
    //Item.content.push_back(LastItem);
    Item.type = NonTerminator;
    Item.normal = true;

    MiddleCodeTable.push_back(Code{ CalSign.Name,GetMiddleName(Factor.Name,Factor.dems),GetMiddleName(LastItem.Name,LastItem.dems),GetMiddleName(Item.Name,Item.dems) });
//    Item.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(Item);
}


void GrammaAnalysis::AddExpr_1()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode AddExpr;
    SemanticTreeNode &Factor = StorePop[0];
    AddExpr.Name = Factor.Name;
    AddExpr.IsNull = 0;
    AddExpr.normal = true;
    AddExpr.type = NonTerminator;
    AddExpr.dems = Factor.dems;
//    AddExpr.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(AddExpr);
}


void GrammaAnalysis::AddExpr_23()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode AddExpr;
    SemanticTreeNode &Factor = StorePop[2];
    SemanticTreeNode &CalSign = StorePop[1];
    SemanticTreeNode &LastAddExpr = StorePop[0];
    AddExpr.Name = MallocVar();//分配中间变量
    AddExpr.IsNull = 0;
    AddExpr.type = NonTerminator;
    AddExpr.normal = true;

    MiddleCodeTable.push_back(Code{ CalSign.Name,GetMiddleName(Factor.Name,Factor.dems),GetMiddleName(LastAddExpr.Name,LastAddExpr.dems),GetMiddleName(AddExpr.Name,AddExpr.dems) });
//    AddExpr.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(AddExpr);
}


void GrammaAnalysis::Relop()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode RelopExpr;
    SemanticTreeNode &CalSign = StorePop[0];
    RelopExpr.Name = CalSign.Name;
    RelopExpr.IsNull = 0;
    RelopExpr.type = NonTerminator;
    RelopExpr.normal = true;
    SemanticStack.push(RelopExpr);
}

void GrammaAnalysis::Expression_1()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode Expr;
    SemanticTreeNode &AddExpr = StorePop[0];
    Expr.Name = AddExpr.Name;
    Expr.IsNull = 0;
    Expr.type = NonTerminator;
//    Expr.NextList.push_back((int)MiddleCodeTable.size());
    Expr.normal = true;
    Expr.dems = AddExpr.dems;
    SemanticStack.push(Expr);
}
void GrammaAnalysis::Expression_2()
{
    unordered_map<string,string>reverse;
    reverse["=="]="!=";
    reverse["<="]=">";
    reverse[">="]="<";
    reverse["!="]="==";
    reverse[">"]="<=";
    reverse["<"]=">=";
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode Expr;
    SemanticTreeNode &AddExpr1 = StorePop[2];
    SemanticTreeNode &Relop = StorePop[1];
    SemanticTreeNode &AddExpr2 = StorePop[0];
    string TempVar = MallocVar();
    Expr.Name = TempVar;
    Expr.IsNull = 0;
    Expr.type = NonTerminator;
    Expr.normal = true;
    Expr.FalseList.push_back((int)MiddleCodeTable.size());

    MiddleCodeTable.push_back(Code{ ("j" + reverse[Relop.Name]),GetMiddleName(AddExpr1.Name,AddExpr1.dems),GetMiddleName(AddExpr2.Name,AddExpr2.dems),"-1" });
//    Expr.NextList.push_back((int)MiddleCodeTable.size());
    SemanticStack.push(Expr);
}
//<数组> ::= [ num ]
//$语义: 需要记录num的值
void GrammaAnalysis::ArrayFactor1()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode ArrayFactor;
    SemanticTreeNode &dimennum = StorePop[1];
    ArrayFactor.dems.push_back(dimennum.ival);
    SemanticStack.push(ArrayFactor);
}

//<数组> ::= [ num ] <数组>
//$语义: 需要记录num的值
void GrammaAnalysis::ArrayFactor2()
{
    int RightLen = 4;
    GetStorePop(RightLen);
    SemanticTreeNode ArrayFactor;
    SemanticTreeNode &PreArrayFactor = StorePop[0];
    SemanticTreeNode &dimennum = StorePop[2];
    ArrayFactor.dems = PreArrayFactor.dems;
    ArrayFactor.dems.push_back(dimennum.ival);
    SemanticStack.push(ArrayFactor);
}
//<内部数组声明> ::= int ID <数组>
//$语义: 记录变量表中数组类型变量的维度，数组的名称，以及数组所在的过程位置
void GrammaAnalysis::ArrayStatement()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode ArrayState;
    SemanticTreeNode &ArrayFactor = StorePop[0];
    SemanticTreeNode &ID = StorePop[1];
    SemanticTreeNode &type = StorePop[2];
    ArrayState.dems = ArrayFactor.dems;
    ArrayState.Namekind = Array;
    ArrayState.Name = ID.Name;
    VarTable.push_back(Var{ ID.Name,INT1,Array,0,0,ProcNoStack[int(ProcNoStack.size()-1)],ArrayFactor.dems});
    SemanticStack.push(ArrayState);
}

//<赋值语句> ::= ID <数组> = <表达式> ;
//$语义: 检查下标是否正确
//$语义: 检查数组变量是否存在
void GrammaAnalysis::AssignMent1()
{
    int RightLen = 5;
    GetStorePop(RightLen);
    SemanticTreeNode AssignState;
    AssignState.Name = SemanticLeftSign.str;
    SemanticTreeNode &Exp = StorePop[1];
    SemanticTreeNode &ArrayExpr = StorePop[3];
    SemanticTreeNode &ID = StorePop[4];
    if(CheckVarTable(Exp.Name)!=-1||isdigit(Exp.Name[0]))
        ;
    else{
        ErrorVarName=Exp.Name;
        semanticerror = NoVar;
        return;
    }
    int error_num = CheckArrayTable(ID.Name,ArrayExpr.dems);
    if(error_num == -1){
        ErrorVarName=ID.Name;
        semanticerror = NoVar;
        return;
    }
    else if(error_num == -2)
    {
        ErrorVarName=ID.Name;
        semanticerror = ArrayIndexError;
        return;
    }
    else
    {
        Var VID = FindVarTable(ID.Name);
        string VIDNo = VID.ProcNo != -1?to_string(VID.ProcNo):"";//需要区分中间变量表中的每一个变量
        ID.Name = ID.Name+" "+VIDNo;
        for(int i = int(ArrayExpr.dems.size()-1);i>=0;i--)
        {
            ID.Name = ID.Name+" "+to_string(ArrayExpr.dems[i]);
        }
        MiddleCodeTable.push_back(Code{ "=",GetMiddleName(Exp.Name,Exp.dems) ,"-",ID.Name });
    }
    SemanticStack.push(AssignState);
}



//语义动作调用函数
//思路：根据id顺序调用
void GrammaAnalysis::SemanticAction(ProduceForms* ResoProduce)
{
    switch (ResoProduce->id)
    {
    case 0:
        break;
    case 1: //[ <Program>--><N> <声明串>  ]
        Program();
        break;
    case 2: //[ <N>-->空  ]
        ProN();
        break;
    case 3: //[ <声明串>--><声明>  ]
    case 4: //[ <声明串>--><声明> <声明串>  ]
        PopPush((int)ResoProduce->RightSign.size());
        break;
    case 5://[ <声明>-->int ID <M> <A> <声明类型>  ]
    case 6://[ <声明>-->void ID <M> <A> <函数声明>  ]
    case 7://[ <声明>-->float ID <M> <A> <声明类型>  ]
        Statement();
        break;
    case 8://[ <M>-->空  ]
        ProM();
        break;
    case 9://[ <A>-->空  ]
        ProA();
        break;
    case 10://[ <声明类型>--><变量声明>  ]
    case 11://[ <声明类型>--><函数声明>  ]
        StateTypeF();
        break;
    case 12://[ <变量声明>-->;  ]
        VarStatement();
        break;
    case 13://[ <函数声明>-->( <形参> ) <语句块>  ]
        FuncStatement();
        break;
    case 14://[ <形参>--><参数列表>  ]
    case 15://[ <形参>-->void  ]
        FormalParameters();
        break;
    case 16://[ <参数列表>--><参数> <逗号和参数>  ]
        ParametersList();
        break;
    case 17://[ <逗号和参数>-->, <参数> <逗号和参数>  ]
        CommaParameter1();
        break;
    case 18://[ <逗号和参数>-->空  ]
        CommaParameter2();
        break;
    case 19://[ <参数>-->int ID  ]
    case 20://[ <参数>-->float ID  ]
        Parameter();
        break;
    case 21://[ <语句块>-->{ <内部声明> <语句串> }  ]
        StatementBlock();
        break;
    case 22://[ <内部声明>-->空  ]
        PopPush(0);
        break;
    case 23://[ <内部声明>--><内部变量声明> ; <内部声明>  ]
        PopPush((int)ResoProduce->RightSign.size());
        break;
    case 24://[ <内部变量声明>-->int ID  ]
    case 25://[ <内部变量声明>-->float ID  ]
        InnerVarState();
        break;
    case 26://[ <语句串>--><语句>  ]
        SentenceList1();
        break;
    case 27://[ <语句串>--><语句> <M> <语句串>  ]
        SentenceList2();
        break;
    case 28://[ <语句>--><if语句>  ]
    case 29://[ <语句>--><while语句>  ]
    case 30://[ <语句>--><return语句>  ]
    case 31://[ <语句>--><赋值语句>  ]
        Sentence();
        break;
    case 32://[ <赋值语句>-->ID = <表达式> ;  ]
        AssignMent();
        break;
    case 33://[ <return语句>-->return <表达式> ;  ]
        Return1();
        break;
    case 34://[ <return语句>-->return ;  ]
        Return2();
        break;
    case 35://[ <while语句>-->while <M> ( <表达式> ) <A> <语句块>  ]
        While();
        break;
    case 36://[ <if语句>-->if ( <表达式> ) <A> <语句块>  ]
        If1();
        break;
    case 37://[ <if语句>-->if ( <表达式> ) <A> <语句块> <N> else <M> <A> <语句块>  ]
        If2();
        break;
    case 38://[ <表达式>--><加法表达式>  ]
        Expression_1();
        break;
    case 39://[ <表达式>--><加法表达式> <relop> <加法表达式>  ]
        Expression_2();
        break;
    case 40://[ <relop>--><  ]
    case 41://[ <relop>--><=  ]
    case 42://[ <relop>-->>  ]
    case 43://[ <relop>-->>=  ]
    case 44://[ <relop>-->==  ]
    case 45://[ <relop>-->!=  ]
        Relop();
        break;
    case 46://[ <加法表达式>--><项>  ]
        AddExpr_1();
        break;
    case 47://[ <加法表达式>--><项> + <加法表达式>  ]
    case 48://[ <加法表达式>--><项> - <加法表达式>  ]
        AddExpr_23();
        break;
    case 49://[ <项>--><因子>  ]
        Item_1();
        break;
    case 50://[ <项>--><因子> * <项>  ]
    case 51://[ <项>--><因子> / <项>  ]
        Item_23();
        break;
    case 52://[ <因子>-->num  ]
        Factor_1();
        break;
    case 53://[ <因子>-->( <表达式> )  ]
        Factor_2();
        break;
    case 54://[ <因子>-->ID  ]
        Factor_3();
        break;
    case 55://[ <因子>-->ID ( <实参列表> )  ]
        Factor_4();
        break;
    case 56://[ <实参列表>--><表达式>  ]
        ArgumentList_1();
        break;
    case 57://[ <实参列表>--><表达式> , <实参列表>  ]
        ArgumentList_2();
        break;
    case 58://[ <实参列表>-->空  ]
        ArgumentList_3();
        break;
    case 59://<内部声明> ::= <内部数组声明> ; <内部声明>
        PopPush((int)ResoProduce->RightSign.size());
        break;
    case 60://<内部数组声明> ::= int ID <数组>
        ArrayStatement();
        break;
    case 61://<数组> ::= [ num ]
        ArrayFactor1();
        break;
    case 62://<数组> ::= [ num ] <数组>
        ArrayFactor2();
        break;
    case 63://<赋值语句> ::= ID <数组> = <表达式> ;
        AssignMent1();
        break;
    case 64://<因子> ::= ID <数组>
        Factor_5();
        break;

    default:
        break;
    }
}

//下面是一些文件输出函数(debug)
ofstream& operator << (ofstream& os,const Var& s)
{
    os<<s.name<<": "<<" s.fval["<<s.fval<<"] s.ival["<<s.ival<<"] s.type["<<s.type<<"]"<<" s.ProcNo["<<s.ProcNo<<"]";
    return os;
}

ofstream& operator << (ofstream& os,const Func& s)
{
    os<<s.name<<":";
    for(int i=0;i<(int)s.param.size();i++)
    {
        os<<" param"<<i+1<<"["<<s.param[i].name<<" "<<s.param[i].type<<"]";
    }
    os<<" s.type["<<s.type<<"]";
    return os;
}

ofstream& operator << (ofstream& os,const Code& s)
{
    os<<s.op<<", "<<s.op1<<", "<<s.op2<<", "<<s.linkres;
    return os;
}


ofstream& operator << (ofstream& os,const vector<Var>& s)
{
    os<<"============================变量表============================="<<endl;
    for(int i=0;i<(int)s.size();i++)
        os<<s[i]<<endl;
    os<<"=============================================================="<<endl;
    return os;
}

ofstream& operator << (ofstream& os,const vector<Func>& s)
{
    os<<"============================函数表============================="<<endl;
    for(int i=0;i<(int)s.size();i++)
        os<<s[i]<<endl;
    os<<"=============================================================="<<endl;
    return os;
}


ofstream& operator << (ofstream& os,const vector<Code>& s)
{
    os<<"============================中间代码表============================="<<endl;
    for(int i=0;i<(int)s.size();i++)
    {
        os<<i<<":";
        os<<"(";
        os<<s[i]<<")"<<endl;
    }
    os<<"=============================================================="<<endl;
    return os;
}

