#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<sstream>
#include<unordered_map>
#include<map>
#include <sstream>
#include "lexical_analysis.h"
#include <stack>
#include <iomanip>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
using namespace std;
enum SignType{ StartTerminator,Terminator , NonTerminator, Empty};
enum ActionType { Resolution,MoveIn ,Acc };
enum SemanticError {None,NoFuncName,FuncParLenError,FuncReturnErr, NoMainName,RedefineVar,NoVar};
enum NameTableKind { Varible, ConstV, Array, Proc };
enum NameTableType { INT1, FLOAT1,VOID1 };

class MainWindow;

//变量元素
struct Var {
    string name;//名字标识符
    NameTableType type;//名字的类型
    bool normal;//true为非形参，false为形参
    int ival;
    float fval;
    int level;//变量层级
    int quad;//记录哪条代码产生该变量 方便寻找未定义就使用
    int ProcNo;//过程编号
};

struct Par {
    NameTableType type;//参数类型
    string name;//参数名
};
//函数元素
struct Func {
    string name;//名字标识符
    NameTableType type;//函数的类型
    int adr;//过程的地址
    vector<Par> param;//参数
};

//数组元素
struct Array_ {
    string name;//名字标识符
    int len;//数组元素个数
};

//四元式
struct Code {
    string op;
    string op1;
    string op2;
    string linkres;
};


struct TreeNode {
    string str;//字符内容
    string StrType;//内部的表示
    SignType type;//类型
};

struct Sign
{
    string str;//字符内容
    SignType type;//类型
    Sign();
    Sign(const Sign& s);
    Sign(string MyStr, SignType MyType);
    Sign& operator=(const Sign& s) {
        if (this == &s) {
            return *this;
        }
        this->str = s.str;
        this->type = s.type;
        return *this;
    }
    //重载等于号 方便判断
    bool operator==(const Sign& s) {
        return (s.str == this->str && s.type == this->type);
    }
    //重载小于号 方便放入map set等数据结构
    bool operator<(const Sign& s)const {
        return ( this->str<s.str);
    }
};
struct ProduceForms
{
    int id;//这里添加了一个id，用于归结寻找产生式
    Sign LeftSign;//左部符号
    vector<Sign> RightSign;//右部符号列表
    ProduceForms();
    friend ostream& operator << (ostream& os, const ProduceForms* s);
    friend fstream& operator << (fstream& os, const ProduceForms* s);
    //重载等于 方便判断
    bool operator==(const ProduceForms& s) {
        if (s.RightSign.size() != this->RightSign.size())
            return false;
        if (!(this->LeftSign == s.LeftSign))
            return false;
        for (int i = 0; i < (int)RightSign.size(); i++)
        {
            if (!(this->RightSign[i] == s.RightSign[i]))
                return false;
        }
        return true;
    }
};
struct Actions
{
    int State;//跳转到的状态
    int ProduceFormsId;//产生式的标号
    ActionType Type;//是移进还是归约
    friend ostream& operator << (ostream& os, const Actions& s);
    friend ofstream& operator << (ofstream& os, const Actions& s);
    bool operator==(const Actions& s)const  {
        return State == s.State && ProduceFormsId == s.ProduceFormsId && Type == s.Type;
    }
    bool operator != (const Actions& s)const {
        return !(*this == s);
    }
};

class GrammaAnalysis;
class LR1Production :public ProduceForms
{
private:
    int DotPosition;//点的位置
    Sign Foresee;//展望
public:
    friend GrammaAnalysis;
    friend ostream& operator << (ostream& os, const LR1Production* s);
    LR1Production(int MyDotPosition, Sign MyForesee, const ProduceForms& BaseProdction);
    void PopSign();//出栈，点往后移一格
    LR1Production* next_LR1Production(Sign);//通过符号生成下一些项目
    bool operator==(LR1Production& s) {
        if(s.DotPosition != this->DotPosition)
            return false;
        if (!(this->Foresee == s.Foresee))
            return false;
        if (s.RightSign.size() != this->RightSign.size())
            return false;
        if (!(this->LeftSign == s.LeftSign))
            return false;
        for (int i = 0; i < (int)RightSign.size(); i++)
        {
            if (!(this->RightSign[i] == s.RightSign[i]))
                return false;
        }
        return true;
    }
};

class ProductionSet
{
private:
    vector<LR1Production*> LR1Productions;
    int id;//用于标识不同的状态
    vector<ProductionSet*>NextNode;//下一个状态
    unordered_map<string, int> MyMap;//使用string映射到map
public:
    friend MainWindow;
    friend GrammaAnalysis;
    friend ostream& operator << (ostream& os, const ProductionSet* s);
    ProductionSet(vector<LR1Production> MyLR1Productions, int MyId);
    ProductionSet();

    bool operator==(ProductionSet& s) {
        if (this->LR1Productions.size() != s.LR1Productions.size())
            return false;
        for (int i = 0; i < (int)LR1Productions.size(); i++)
            if (!(*this->LR1Productions[i] == *s.LR1Productions[i]))
                return false;
        return true;
    }

};

struct SemanticTreeNode
{
    string Name;//如果Name = num ，则ival和fval里面有值
    vector <SemanticTreeNode> content;//内容，快速记录，终结符的Name和content的str是一样的
    SignType type;//非终结符还是终结符
    vector<int> NextList;
    vector<int> FalseList;
    vector<int> TrueList;//这三个数组用来合并
    int Quad;//给M,N,A用
    int IsNull;//是否为空，快速判断
    NameTableKind Namekind;//名字的类型
    //如果是数值
    NameTableType ValTpye; //值类型
    int ival;
    float fval;//需要被储存
    //非终结符的一些属性
    bool normal;//是形参还是实参
    int Adr;//如果是函数，记录地址(这里的地址应该是四元式的编号)
    int len;//数组元素个数(暂时不用)
    vector<Par> param;//参数，如果是函数
};

class GrammaAnalysis
{
public:
    GrammaAnalysis(lexical_analysis* MyWordSets, const string _FilePath, const string _TableOutput, const string _GraphOutput ,const string _GraphDotOutput,bool IsSematic = true);
    ~GrammaAnalysis();
    int Forward(QStandardItemModel* ProcessSetData);//前向传播，生成语法树
    friend MainWindow;
    friend ofstream& operator << (ofstream& os,const vector<Var>& s);
    friend ofstream& operator << (ofstream& os,const vector<Func>& s);
    friend ofstream& operator << (ofstream& os,const Code& s);
private:    
    string FilePath, TableOut, GraphOut,GraphOutDot;
    lexical_analysis* WordSets;//词法分析器的结果，输入符号
    stack <TreeNode> SignStack;//符号栈
    stack <int> StateStack;//状态栈
    vector<ProduceForms*>ProduceFormsVec;//一个项目集
    vector<ProductionSet*>ProductionSetVec;//是项目集集合
    unordered_map<string, set<Sign>> first_table;//使用string映射到map
    set<Sign> GetFirstSign(vector<Sign> InSign);//输入连续字符串，得到first集
    set<Sign> GetFirstSign(Sign InSign);//输入某个符号，得到first集
    map< pair<int, Sign>, Actions>ActionTable;//action表
    map< pair<int, Sign>, int>GoTable;//goto表
    set<Sign>AllTerminator;//终结符集合 包括空
    set<Sign>AllNonTerminator;//非终结符集合
    //下面是语义分析的各种表
    bool IsSematic;//是否进行语义分析
    //语义分析debug文件名
    vector<Var> VarTable;//变量表
    vector<Func> FuncTable;//函数表
    vector<Array_> ArrayTable;//数组表
    vector<Code> MiddleCodeTable;//中间代码表
    stack <SemanticTreeNode> SemanticStack;//语义分析栈
    vector <SemanticTreeNode> StorePop;//储存pop出来的值

    //层级
    int Level;
    //目前的过程编号
    int ProcNo;
    vector<int> ProcNoStack;
    //下面是语义动作函数
    Sign SemanticLeftSign;
    //在调用下面的函数的时候，自己进栈出栈
    SemanticError semanticerror;
    //申请中间变量
    int MiddleVarIndex;
    string MallocVar();
    //出栈函数
    void GetStorePop(int len);

    // 实参列表 规约成实参列表的第一个产生式，下面含义同
    //<实参列表> ::=<表达式>| <表达式> , <实参列表>|空
    void ArgumentList_1();
    void ArgumentList_2();
    void ArgumentList_3();
    //因子
    //<因子> ::=num |  ( <表达式> )  |ID  |ID ( <实参列表> )
    void Factor_1();
    void Factor_2();
    void Factor_3();
    void Factor_4();
    //项
    //<项> ::= <因子>| <因子> * <项>|<因子> / <项>
    void Item_1();
    void Item_23();
    //加法表达式
    //<加法表达式> ::= <项> |<项> + <加法表达式>|<项> - <加法表达式>
    void AddExpr_1();
    void AddExpr_23();
    //relop
    //<relop>::=<|<=|>|>=|==|!=
    void Relop();
    //表达式
    //<表达式>::=<加法表达式>|<加法表达式> <relop> <加法表达式>
    void Expression_1();
    void Expression_2();
    //<Program> ::= <N> <声明串>
    void Program();
    //<N> ::=空
    void ProN();
    //除了出栈 入栈 没有其他操作的
    //<声明串> :: = <声明> | <声明> <声明串>
    void PopPush(int len);
    //<声明> ::=int  ID <M> <A> <声明类型> | void  ID <M> <A>  <函数声明>|float  ID <M> <A> <声明类型>
    void Statement();
    //<M> ::= 空
    void ProM();
    //<A> ::= 空
    void ProA();
    //<声明类型>:: = <变量声明> | <函数声明>
    void StateTypeF();
    //<变量声明> ::=  ;
    void VarStatement();
    //<函数声明> ::= ( <形参> ) <语句块>
    void FuncStatement();
    //<形参>::= <参数列表> | void
    void FormalParameters();
    //<参数列表>  ::= <参数> <逗号和参数>
    void ParametersList();
    //<逗号和参数> ::= , <参数> <逗号和参数>
    void CommaParameter1();
    //<逗号和参数> ::= 空
    void CommaParameter2();
    //<参数> ::= int  ID|float  ID
    void Parameter();
    //<语句块> ::= { <内部声明>  <语句串> }
    void StatementBlock();
    //<内部声明> ::= 空 | <内部变量声明> ; <内部声明>
    //直接调用void PopPush(int len);

    //<内部变量声明>::=int  ID|float  ID
    void InnerVarState();
    //<语句串> ::= <语句>|<语句> <M> <语句串>
    void SentenceList1();
    void SentenceList2();
    //<语句> ::= <if语句> |<while语句> | <return语句> | <赋值语句>
    void Sentence();
    //<赋值语句> ::=  ID = <表达式> ;
    void AssignMent();
    //<return语句> :: = return <表达式>; | return;
    void Return1();
    void Return2();
    //<while语句> ::= while <M> ( <表达式> ) <A> <语句块>
    void While();
    //<if语句> ::= if  ( <表达式> )  <A> <语句块>  |if  ( <表达式> ) <A>  <语句块> <N> else <M> <A> <语句块>
    void If1();
    void If2();
    //查找函数表 没有返回-1
    int FindFuncTable(const string&FuncName);
    //回填
    void BackPatch(vector<int> &BackList,int Addr);
    //归约时调用语义动作函数
    void SemanticAction(ProduceForms* ResoProduce);
    //语义分析报错函数
    //当前函数归约的语句块返回类型
    int ReType;
    string ErrorFuncName;
    string ErrorVarName;
    void SDisError(int WordPos,SemanticError ErrType);
    //查变量表
    int  CheckVarTable(const string&name);
    //删变量表
    void DeleteVarTable(int level);

    //下面是语法分析函数
    void GetFull(ProductionSet& MyDFAState);//通过当前LR1Productions中已有的状态求闭包
    void ProduceFormsGenerate();//产生一个包含ProduceForms的vector
    void BuildDFA();//建立DFA
    //这里还要有Action和Goto的函数的建立
    void BuildActionGo();
    void Conflict_Detection(const Actions& action, pair<int, Sign>& tableindex, int i, LR1Production* j);
    void PrintActionGoToFile();
    void PrintProduceFormsVec();
    void PrintActionGo();
    void Tips(const string& str);

};



ostream& operator << (ostream& os, const LR1Production* s);
ostream& operator << (ostream& os, const ProductionSet* s);
ostream& operator << (ostream& os, const ProduceForms* s);
fstream& operator << (fstream& os, const ProduceForms* s);
ofstream& operator << (ofstream& os,const Var& s);
ofstream& operator << (ofstream& os,const Func& s);
ofstream& operator << (ofstream& os,const Code& s);
ofstream& operator << (ofstream& os,const vector<Var>& s);
ofstream& operator << (ofstream& os,const vector<Func>& s);
ofstream& operator << (ofstream& os,const vector<Code>& s);
string GbkToUtf8(const char* src_str);


