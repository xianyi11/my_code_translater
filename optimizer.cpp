#include "./optimizer.h"
using namespace std;

Optimizer::Optimizer(vector<Var> VarTable1,vector<Func> FuncTable1,vector<Code> MiddleCodeTable1)
{
    this->VarTable = VarTable1;
    this->FuncTable = FuncTable1;
    this->MiddleCodeTable = MiddleCodeTable1;
}


void Optimizer::Divblocks()
{
    vector<int> EntryEentence;//入口语句集合
    bool is_last_jump = false;
    for(int i =0;i<(int)MiddleCodeTable.size();i++)
    {
        if(i == 0)//首条语句
            EntryEentence.push_back(i);
        if(is_last_jump)//跳转语句之后的第一条语句
            EntryEentence.push_back(i);
        if(MiddleCodeTable[i].op == "j" || MiddleCodeTable[i].op == "j<" || MiddleCodeTable[i].op == "j<=" || MiddleCodeTable[i].op == "j>"
         ||MiddleCodeTable[i].op == "j>=" || MiddleCodeTable[i].op == "j==" || MiddleCodeTable[i].op == "j!=")
        {
            is_last_jump = true;
            EntryEentence.push_back(atoi(MiddleCodeTable[i].linkres.c_str()));//跳转的目标语句
        }
        else
            is_last_jump = false;
    }
    sort(EntryEentence.begin(),EntryEentence.end());//排序入口语句
    for(int i=0;i<(int)EntryEentence.size();i++)
    {
        CODE_BLOCK ThisBlock;
        ThisBlock.BeginIndex = EntryEentence[i];
        int curi = ThisBlock.BeginIndex;
        while(curi < min(EntryEentence[i+1],(int)MiddleCodeTable.size()))
        {
            ThisBlock.code.push_back(MiddleCodeTable[curi]);
            curi++;
        }
        CodeBlock.push_back(ThisBlock);
    }




    return;
}
