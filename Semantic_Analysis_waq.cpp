#include "Gramma_analysis.h"
using namespace std;
//查找函数表 没有返回-1
int GrammaAnalysis::FindFuncTable(const string& FuncName)
{
    for (int i = 0; i < int(FuncTable.size()); i++)
    {
        if (FuncTable[i].name == FuncName)
        {
            return i;
        }
    }
    return -1;
}
//<Program> :: = <N> <声明串>
//$语义 将main的地址回填到N的nextlist
void GrammaAnalysis::Program()
{
    int FuncIndex = FindFuncTable("main");
    if (FuncIndex == -1) {
        semanticerror = NoMainName;
        return;
    }
    int RightLen = 2;
    GetStorePop(RightLen);
    SemanticTreeNode program;
    program.Name = SemanticLeftSign.str;
    SemanticTreeNode& N = StorePop[1];
    BackPatch(N.NextList, FuncTable[FuncIndex].adr);
    SemanticStack.push(program);
}
//<N> :: = 空
//$语义 N.nextlist = { nextquad }
//$语义 emit(j, _, _, -1)
void GrammaAnalysis::ProN()
{
    SemanticTreeNode N;
    N.Name = SemanticLeftSign.str;
    N.NextList.push_back(MiddleCodeTable.size());
    MiddleCodeTable.push_back(Code{ "j","-" ,"-","-1" });
    SemanticStack.push(N);
}
//除了出栈 入栈 没有其他操作的
void GrammaAnalysis::PopPush(int len)
{
    SemanticTreeNode NothingToDo;
    NothingToDo.Name = SemanticLeftSign.str;
    GetStorePop(len);
    SemanticStack.push(NothingToDo);
}
//<声明> :: = int  ID <M> <A> <声明类型> | void  ID <M> <A>  <函数声明> | float  ID <M> <A> <声明类型>
//$语义 对于声明类型 如果是变量声明 此处形成一个完整的变量声明 将变量名 类型 （变量层次）加入变量表
//$语义 对于声明类型 如果是函数声明 此处形成一个完整的函数声明 将函数名 类型 参数表 函数地址加入函数表
//$语义 对于函数声明 此处形成一个完整的函数声明 将函数名 类型 参数表 函数地址加入函数表
void GrammaAnalysis::Statement()
{
    int RightLen = 5;
    GetStorePop(RightLen);
    SemanticTreeNode State;
    State.Name = SemanticLeftSign.str;
    SemanticTreeNode& Type = StorePop[4];
    SemanticTreeNode& ID = StorePop[3];
    SemanticTreeNode& M = StorePop[2];
    SemanticTreeNode& A = StorePop[1];
    SemanticTreeNode& StateType = StorePop[0];
    //函数
    if (StateType.Namekind== Proc) {
        NameTableType ValTpye = Type.Name == "int"? INT1:Type.Name == "float"?FLOAT1:VOID1;
        if((ValTpye==VOID1&&ReType!=VOID1)||(ValTpye!=VOID1&&ReType==VOID1)){
            semanticerror = FuncReturnErr;
            ErrorFuncName=ID.Name;
            return;
        }
        FuncTable.push_back(Func{ ID.Name,ValTpye,M.Quad,StateType.param });
    }
    else {
        //问题很大
        NameTableType ValTpye = Type.Name == "int"? INT1:FLOAT1;
        VarTable.push_back(Var{ ID.Name,ValTpye,Varible,Type.ival,Type.fval,ProcNoStack[int(ProcNoStack.size()-1)]});
    }
    SemanticStack.push(State);
}
void GrammaAnalysis::ProM()
{
    SemanticTreeNode M;
    M.Name=SemanticLeftSign.str;
    M.Quad = MiddleCodeTable.size();
    SemanticStack.push(M);
}
//<A> ::= 空
void GrammaAnalysis::ProA()
{
    SemanticTreeNode A;
    A.Name=SemanticLeftSign.str;
    ProcNo++;
    ProcNoStack.push_back(ProcNo);
    SemanticStack.push(A);
}
//<声明类型>:: = <变量声明> | <函数声明>
//$语义 对于变量声明 记录当前声明类型为变量
//$语义 对于函数声明 直接传递参数表等属性
void GrammaAnalysis::StateTypeF()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode State= StorePop[0];
    State.Name = SemanticLeftSign.str;
    SemanticStack.push(State);
}

//<变量声明> :: = ;
//$语义  此处类型为变量 之后归约不用特判
void GrammaAnalysis::VarStatement()
{
    ProcNoStack.pop_back();
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode VarState;
    VarState.Name = SemanticLeftSign.str;
    VarState.Namekind = Varible;
    SemanticStack.push(VarState);
}
//<函数声明> :: = (<形参>) <语句块>
//$语义 将左侧的形参列表置为<形参>的形参列表 类型为函数声明
void GrammaAnalysis::FuncStatement()
{
    int RightLen = 4;
    GetStorePop(RightLen);
    SemanticTreeNode List = StorePop[2];
    List.Name = SemanticLeftSign.str;
    List.Namekind = Proc;

    SemanticStack.push(List);
}
//<形参>:: = <参数列表> | void
//$语义 对于参数列表 将左侧的形参列表置为<参数列表>的形参列表
//$语义 对于void 将左侧的形参列表置为空
void GrammaAnalysis::FormalParameters()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode fp = StorePop[0];
    fp.Name= SemanticLeftSign.str;
    for(auto &item:fp.param){
        VarTable.push_back(Var{ item.name,item.type,Varible,0,0,ProcNoStack[int(ProcNoStack.size()-1)]});
    }
    SemanticStack.push(fp);
}
//<参数列表>  :: = <参数> <逗号和参数>
//$语义 将左侧的参数列表置为右侧的参数的合并 包括数据类型等信息
void GrammaAnalysis::ParametersList()
{
    int RightLen = 2;
    GetStorePop(RightLen);
    SemanticTreeNode fp = StorePop[1];
    fp.Name = SemanticLeftSign.str;
    fp.param.insert(fp.param.end(), StorePop[0].param.begin(), StorePop[0].param.end());//将参数表合并
    SemanticStack.push(fp);
}
//<逗号和参数> ::= , <参数> <逗号和参数>
//$语义 将左侧的参数列表置为右侧的参数的合并 包括数据类型等信息
void GrammaAnalysis::CommaParameter1()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode fp = StorePop[1];
    fp.Name = SemanticLeftSign.str;
    fp.param.insert(fp.param.end(), StorePop[0].param.begin(), StorePop[0].param.end());//将参数表合并
    SemanticStack.push(fp);
}
//<逗号和参数> ::= 空
//$语义 无 压栈出栈即可
void GrammaAnalysis::CommaParameter2()
{
    SemanticTreeNode fp;
    fp.Name = SemanticLeftSign.str;
    SemanticStack.push(fp);
}
//<参数> :: = int  ID | float  ID
//$语义 将左侧的形参列表置为一个参数 类型为 int | float 名为 ID
void GrammaAnalysis::Parameter()
{
    int RightLen = 2;
    GetStorePop(RightLen);
    SemanticTreeNode p;
    p.Name= SemanticLeftSign.str;
    if (StorePop[1].Name == "int")
        p.param.push_back({ INT1, StorePop[0].Name });
    else
        p.param.push_back({ FLOAT1, StorePop[0].Name });
    SemanticStack.push(p);
}
//<语句块> :: = { <内部声明>  <语句串> }
//$语义 nowLevel--
//$语义 <语句块>nextlist = <语句串>nextlist
void GrammaAnalysis::StatementBlock()
{
    ProcNoStack.pop_back();
    int RightLen = 4;
    GetStorePop(RightLen);
    SemanticTreeNode statement_block= StorePop[1];
    statement_block.Name= SemanticLeftSign.str;
    SemanticStack.push(statement_block);
}

//<内部变量声明>:: = int  ID | float  ID
//$语义 此处形成一个完整的变量声明 将变量名 类型 （变量层次）加入变量表
void GrammaAnalysis::InnerVarState()
{
    int RightLen = 2;
    GetStorePop(RightLen);
    SemanticTreeNode InnerVar;
    InnerVar.Name = SemanticLeftSign.str;
    SemanticTreeNode& Type = StorePop[1];
    SemanticTreeNode& ID = StorePop[0];
    //问题很大
    int index = 0;
    if((index = CheckVarTable(ID.Name))!=-1 && index == ProcNoStack[int(ProcNoStack.size())-1])
    {
        semanticerror = RedefineVar;
        ErrorVarName=ID.Name;
        return;
    }
    NameTableType ValTpye = Type.Name == "int" ? INT1 : FLOAT1;
    VarTable.push_back(Var{ ID.Name,ValTpye,Varible,Type.ival,Type.fval,ProcNoStack[int(ProcNoStack.size()-1)]});
    SemanticStack.push(InnerVar);
}
//<语句串> :: = <语句> | <语句> <M> <语句串>
//$语义 <语句> 将右侧 nextList 传递给左侧
//$语义 <语句> <M> <语句串> 将右侧语句串 nextList 传递给左侧 用M回填语句nextlist
void GrammaAnalysis::SentenceList1()
{
    int RightLen = 2;
    GetStorePop(RightLen);
    SemanticTreeNode s= StorePop[1];
    BackPatch(s.NextList,StorePop[0].Quad);
    s.Name = SemanticLeftSign.str;
    SemanticStack.push(s);
}
void GrammaAnalysis::SentenceList2()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode &sentence=StorePop[2];
    SemanticTreeNode sentence_block = StorePop[0];
    SemanticTreeNode& M = StorePop[1];
    sentence_block.Name = SemanticLeftSign.str;
    BackPatch(sentence.NextList, M.Quad);
    BackPatch(sentence.NextList, M.Quad);
    SemanticStack.push(sentence_block);
}
//<语句> ::= <if语句> |<while语句> | <return语句> | <赋值语句>
void GrammaAnalysis::Sentence()
{
    int RightLen = 1;
    GetStorePop(RightLen);
    SemanticTreeNode s = StorePop[0];
    s.Name = SemanticLeftSign.str;
    SemanticStack.push(s);
}
//<赋值语句> ::=  ID = <表达式> ;
//$语义 生成赋值四元式
void GrammaAnalysis::AssignMent()
{
    int RightLen = 4;
    GetStorePop(RightLen);
    SemanticTreeNode Assign;
    Assign.Name = SemanticLeftSign.str;
    SemanticTreeNode &Exp = StorePop[1];
    SemanticTreeNode &ID = StorePop[3];
    if(CheckVarTable(Exp.Name)!=-1||isdigit(Exp.Name[0]))
        ;
    else{
        ErrorVarName=Exp.Name;
        semanticerror = NoVar;
        return;
    }
    if(CheckVarTable(ID.Name)!=-1)
    {
        string VIDName = GetMiddleName(ID.Name,ID.dems);
        string VExpName = GetMiddleName(Exp.Name,Exp.dems);
        MiddleCodeTable.push_back(Code{ "=",VExpName ,"-",VIDName });
    }
    else{
        ErrorVarName=ID.Name;
        semanticerror = NoVar;
        return;
    }
    SemanticStack.push(Assign);
}
//<return语句> :: = return <表达式> ; | return ;
//$语义 生成返回四元式
void GrammaAnalysis::Return1()
{
    int RightLen = 3;
    GetStorePop(RightLen);
    SemanticTreeNode re;
    re.Name=  SemanticLeftSign.str;
    SemanticTreeNode& Exp = StorePop[1];
    ReType=FLOAT1;
    //可以考虑赋值returnplace
    string VExpName = GetMiddleName(Exp.Name,Exp.dems);
    MiddleCodeTable.push_back({ "return",VExpName,"-","-" });
    SemanticStack.push(re);
}
void GrammaAnalysis::Return2()
{
    int RightLen = 2;
    GetStorePop(RightLen);
    SemanticTreeNode re;
    re.Name = SemanticLeftSign.str;

    ReType=VOID1;
    //可以考虑赋值returnplace
    MiddleCodeTable.push_back({ "return","-","-","-" });
    SemanticStack.push(re);
}
//<while语句> ::= while <M> ( <表达式> ) <A> <语句块>
//$语义 M.quad 回填到语句块 nextList
//$语义 左侧 nextList = 表达式 falselist
//$语义 生成 jump 四元式
void GrammaAnalysis::While()
{
    int RightLen = 7;
    GetStorePop(RightLen);
    SemanticTreeNode& M = StorePop[5];
    SemanticTreeNode& Exp = StorePop[3];
    SemanticTreeNode& S_block = StorePop[0];
    //backpatch
    BackPatch(S_block.NextList, M.Quad);
    //nextlist
    SemanticTreeNode while_sentence;
    while_sentence.NextList = Exp.FalseList;
    while_sentence.Name = SemanticLeftSign.str;
    MiddleCodeTable.push_back(Code{ "j","-","-",to_string(M.Quad) });
    SemanticStack.push(while_sentence);
}
//<if语句> ::= if  ( <表达式> )  <A> <语句块>  |if  ( <表达式> ) <A>  <语句块> <N> else <M> <A> <语句块>
//$语义 第一个产生式
//$语义 表达式 falseList  插入 语句块 nextList
//$语义 If 语句 nextList 设置为表达式 falseList
//$语义 第2个产生式前面一致
//$语义 M.quad 回填到表达式 falseList
//$语义 Merge nextList 作为 if 语句的 nextList
void GrammaAnalysis::If1()
{
    int RightLen = 6;
    GetStorePop(RightLen);
    SemanticTreeNode& Exp= StorePop[3];
    SemanticTreeNode& S_block = StorePop[0];
    SemanticTreeNode if_sentence;
    if_sentence.Name = SemanticLeftSign.str;
    //MERGE
    Exp.FalseList.insert(Exp.FalseList.end(), S_block.NextList.begin(), S_block.NextList.end());//将list合并
    if_sentence.NextList = Exp.FalseList;
    SemanticStack.push(if_sentence);
}
//<if语句> ::= if  ( <表达式> )  <A> <语句块>  |if  ( <表达式> ) <A>  <语句块> <N> else <M> <A> <语句块>
void GrammaAnalysis::If2()
{
    int RightLen = 11;
    GetStorePop(RightLen);
    SemanticTreeNode& Exp = StorePop[8];
    SemanticTreeNode& S_block1 = StorePop[5];
    SemanticTreeNode& N = StorePop[4];
    SemanticTreeNode& M = StorePop[2];
    SemanticTreeNode& S_block2 = StorePop[0];

    //backpatch
    BackPatch(Exp.FalseList, M.Quad);
    //MERGE
    SemanticTreeNode if_sentence;
    if_sentence.NextList = N.NextList;
    if_sentence.Name = SemanticLeftSign.str;
    auto& List = if_sentence.NextList;
    List.insert(List.end(), S_block1.NextList.begin(), S_block1.NextList.end());
    List.insert(List.end(), S_block2.NextList.begin(), S_block2.NextList.end());
//    BackPatch(List,(int)MiddleCodeTable.size());
    SemanticStack.push(if_sentence);
}
//回填
void GrammaAnalysis::BackPatch(vector<int>& BackList, int Addr)
{
    for (auto back : BackList) {
        MiddleCodeTable[back].linkres = to_string(Addr);
    }
}
//查变量表
int GrammaAnalysis::CheckVarTable(const string&name)
{
    for(int i=int(VarTable.size())-1;i>=0;--i){
        if(VarTable[i].name==name){
            for(int j=0;j<(int)ProcNoStack.size();j++)
                if(VarTable[i].ProcNo == ProcNoStack[j])
                    return ProcNoStack[j];
        }
    }
    return -1;
}

int GrammaAnalysis::CheckArrayTable(const string&name,vector<int>&dims)
{
    for(int i=int(VarTable.size())-1;i>=0;--i){
        if(VarTable[i].name==name){
            for(int j=0;j<(int)ProcNoStack.size();j++)
                if(VarTable[i].ProcNo == ProcNoStack[j])
                {
                    if(VarTable[i].dims.size()!=dims.size())
                        return -2;
                    for(int i=0;i<(int)VarTable[i].dims.size();i++)
                        if(VarTable[i].dims[i]<=dims[i])
                            return -2;
                    return ProcNoStack[j];
                }
        }
    }
    return -1;

}

Var GrammaAnalysis::FindVarTable(string name)
{
    for(int i = (int)VarTable.size()-1;i>=0;i--)
    {
        if(VarTable[i].name == name)
        {
            return VarTable[i];
        }
    }
    Var temp;
    temp.ProcNo = -1;
    return temp;
}

string GrammaAnalysis::GetMiddleName(string Name,vector<int>dims)
{
    string name = Name;
    Var VNode = FindVarTable(Name);
    if(VNode.ProcNo == -1)
        return name;
    name = name + " " + to_string(VNode.ProcNo);
    if(VNode.valtype == Array)
    {
        for(int i=0;i<(int)dims.size();i++)
        {
            cout<<"dims[i]:"<<dims[i]<<" ";
            name = name+" "+to_string(dims[i]);
        }
        cout<<endl;
    }
    return name;
}

//删变量表
//void GrammaAnalysis::DeleteVarTable(int level)
//{
//    int start=-1,end=-1;
//    for(int i=0;i<int(VarTable.size());++i){
//        if(VarTable[i].level==level){
//            if(start==-1){
//                start=i;
//            }
//            end=i;
//        }
//    }
//    VarTable.erase(VarTable.begin()+start,VarTable.begin()+end);
//}
