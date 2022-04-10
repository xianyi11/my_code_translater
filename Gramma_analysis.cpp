#include "Gramma_analysis.h"
#include <windows.h>
#include <QMessageBox>
#include <QMainWindow>

using namespace std;

string GbkToUtf8(const char* src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    string strTemp = str;
    if (wstr) delete[] wstr;
    if (str) delete[] str;
    return strTemp;
}
ostream& operator << (ostream& os, const LR1Production* s)
{
    os << "[" << s->LeftSign.str <<" ->";
    for (int i = 0; i < (int)s->RightSign.size(); i++)
    {
        if (i == s->DotPosition)
            os << " .";
        os << " " << s->RightSign[i].str;
    }
    if ((int)s->RightSign.size() == s->DotPosition)
        os << " .";
    os << " , ";
    os << s->Foresee.str;
    os << " ]";
    return os;
}




ostream& operator << (ostream& os, const ProductionSet* s)
{
    os << "id = " << s->id << endl;
    for (int i = 0; i < (int)s->LR1Productions.size(); i++)
        os << s->LR1Productions[i] << endl;
    os << "next node :" << endl;
    for (auto i : s->MyMap)
    {
        os << "--" << i.first << "-->" << s->NextNode[i.second]->id<<"|";
    }
    os << endl;
    return os;
}

ostream& operator << (ostream& os, const ProduceForms* s)
{
    os << "[ " << s->LeftSign.str << "-->";
    for (int i = 0; i < (int)s->RightSign.size(); i++)
        os << s->RightSign[i].str << " ";
    os << " ]";
    return os;
}


fstream& operator << (fstream& os, const ProduceForms* s)
{
    os << "[ " << s->LeftSign.str << "-->";
    for (int i = 0; i < (int)s->RightSign.size(); i++)
        os << s->RightSign[i].str << " ";
    os << " ]";
    return os;
}

GrammaAnalysis::GrammaAnalysis(lexical_analysis* MyWordSets, const string _FilePath, const string _TableOutput, const string _GraphOutput,const string _GraphDotOutput,bool IsSematic)
{
    WordSets = MyWordSets;
    FilePath = _FilePath;
    TableOut = _TableOutput;
    GraphOut = _GraphOutput;
    GraphOutDot = _GraphDotOutput;
    MiddleVarIndex = 0;
    this->IsSematic = IsSematic;

    Level = 0;//层级
    ProcNo = 0;//目前的过程编号
    ProcNoStack.push_back(ProcNo);//过程栈

    semanticerror = None;
    ProduceFormsGenerate();
//    for (auto pf_p : ProduceFormsVec)
//    {
//        GetFirstSign(pf_p->LeftSign);
//    }
//    cout << "-------------------------------------------" << endl << "下面是first集" << endl;
//    for (auto i : first_table) {
//        cout << "first(" << i.first << ")={";
//        for (auto j : i.second) {
//            cout << j.str << " ";
//        }
//        cout << "}" << endl;
//    }
    BuildDFA();
    //for (int i = 0; i < (int)ProductionSetVec.size(); i++)
    //	cout << ProductionSetVec[i] << endl;
    BuildActionGo();
    PrintActionGoToFile();
}

Sign::Sign(string MyStr, SignType MyType)
{
    this->str = MyStr;
    this->type = MyType;
}
Sign::Sign(const Sign& s)
{
    this->str = s.str;
    this->type = s.type;
}

Sign::Sign()
{

}

ProduceForms::ProduceForms()
{

}

LR1Production::LR1Production(int MyDotPosition, Sign MyForesee, const ProduceForms& BaseProdction)
{
    this->DotPosition = MyDotPosition;
    this->Foresee = MyForesee;//重载了=，因为要将string对象深拷贝
    this->id = BaseProdction.id;
    this->LeftSign = BaseProdction.LeftSign;
    for (int i = 0; i < (int)BaseProdction.RightSign.size(); i++)
        this->RightSign.push_back(BaseProdction.RightSign[i]);
}

void LR1Production::PopSign()
{
    this->DotPosition++;
}

LR1Production* LR1Production::next_LR1Production(Sign InSign)//通过符号生成下一些项目
{
    if (this->DotPosition < (int)this->RightSign.size() && InSign.str == this->RightSign[this->DotPosition].str && InSign.type == this->RightSign[this->DotPosition].type)//说明可以转移
        return (LR1Production *)new LR1Production(this->DotPosition + 1, this->Foresee, (const ProduceForms)(*this));
    else
        return NULL;
}

ProductionSet::ProductionSet(vector<LR1Production> MyLR1Productions, int MyId)
{
    size_t len = MyLR1Productions.size();
    for (size_t i = 0; i < len; i++)
        this->LR1Productions.push_back(&MyLR1Productions[i]);
    this->id = MyId;
}

ProductionSet::ProductionSet()
{
    ;
}

void GrammaAnalysis::GetFull(ProductionSet& MyDFAState)
{
    int CurCheckPosition = 0;
    for (; CurCheckPosition < (int)MyDFAState.LR1Productions.size(); CurCheckPosition++)
    {
        //考虑已经要归约的式子，即点在最后
        LR1Production Check = *MyDFAState.LR1Productions[CurCheckPosition];
        if (Check.DotPosition == (int)Check.RightSign.size())//说明是归约串
            continue;
        for (int i = 0; i < (int)ProduceFormsVec.size(); i++)
        {
            if (ProduceFormsVec[i]->LeftSign.str == Check.RightSign[Check.DotPosition].str && ProduceFormsVec[i]->LeftSign.type == Check.RightSign[Check.DotPosition].type)
            {
                //先得到要的first集
                vector<Sign> FirstInSign;
                set<Sign> FirstSet;
                for (int j = Check.DotPosition + 1; j < (int)Check.RightSign.size(); j++)
                    FirstInSign.push_back(Check.RightSign[j]);
                FirstInSign.push_back(Check.Foresee);

                FirstSet = GetFirstSign(FirstInSign);
                //记录一下生成的first集合
                string FirstInSignStr;
                for(int z=0;z<(int)FirstInSign.size();z++)
                {
                    FirstInSignStr+=" ";
                    FirstInSignStr+=FirstInSign[z].str;
                }
                first_table[FirstInSignStr] = FirstSet;
                //通过first集以及筛选好的产生式来生成要加入集合的LR1项目
                set<Sign>::iterator iter;
                for (iter = FirstSet.begin(); iter != FirstSet.end(); ++iter)
                {
                    LR1Production *AddProduction = (LR1Production*)new LR1Production(0, *iter, *ProduceFormsVec[i]);
                    //查重
                    int IsFind = 0;
                    for (int k = 0; k < (int)MyDFAState.LR1Productions.size(); k++)
                    {
                        if (*MyDFAState.LR1Productions[k] == *AddProduction)
                            IsFind = 1;
                    }
                    if (!IsFind)//没有重复
                    {
                        MyDFAState.LR1Productions.push_back(AddProduction);
                    }
                }
            }
        }

    }
}


void GrammaAnalysis::BuildDFA()//建立DFA
{
    //1. 构造初始集合
    ProductionSet *InitSet = new ProductionSet();
    for (int i = 0; i < (int)ProduceFormsVec.size(); i++)
    {
        if (ProduceFormsVec[i]->LeftSign.type == StartTerminator)//找到开始符号的产生式了
        {
            LR1Production *StartPd = new LR1Production(0, Sign{ "#", Terminator }, *ProduceFormsVec[i]);
            //构造初始集合
            InitSet->id = 0;//初始集的id
            InitSet->LR1Productions.push_back(StartPd);
            GetFull(*InitSet);//得到闭包
            break;
        }
    }
    ProductionSetVec.push_back(InitSet);
    //2. 从初始集合出发开始构造DFA
    int SetId = 1;
    int CurCheckPosition = 0;
    //2.1 先得到可以用来转移的符号
    for (; CurCheckPosition < (int)ProductionSetVec.size(); CurCheckPosition++)//对于每一个集合
    {
        vector<Sign> TranSign;
        ProductionSet* CurSet = ProductionSetVec[CurCheckPosition];
        for (int j = 0; j < (int)CurSet->LR1Productions.size(); j++)//对于每一个LR1项目
        {
            int IsFind = 0;
            for (int k = 0; k < (int)TranSign.size(); k++)
                if (CurSet->LR1Productions[j]->DotPosition < (int)CurSet->LR1Productions[j]->RightSign.size() && TranSign[k] == CurSet->LR1Productions[j]->RightSign[CurSet->LR1Productions[j]->DotPosition])
                    IsFind = 1;
            if(!IsFind)
                if(CurSet->LR1Productions[j]->DotPosition < (int)CurSet->LR1Productions[j]->RightSign.size() && CurSet->LR1Productions[j]->RightSign[CurSet->LR1Productions[j]->DotPosition].str != "空")//对于非归约串
                    TranSign.push_back(CurSet->LR1Productions[j]->RightSign[CurSet->LR1Productions[j]->DotPosition]);//加入到集合
        }
        //2.2 通过每一个转移符号来构造项目集
        for (int k = 0; k < (int)TranSign.size(); k++)//对于每一个转移符号
        {
            ProductionSet *AddPdSet = new ProductionSet();//要添加的项目集
            for (int j = 0; j < (int)CurSet->LR1Productions.size(); j++)//对于每一个LR1项目
            {
                LR1Production* AddPd = CurSet->LR1Productions[j]->next_LR1Production(TranSign[k]);
                if (AddPd == NULL)//没有产生
                    continue;
                AddPdSet->LR1Productions.push_back(AddPd);//产生了，加入集合
            }
            GetFull(*AddPdSet);//得到闭包
            int IsFind = 0;
            for (int l = 0; l < (int)ProductionSetVec.size(); l++)
                if (*AddPdSet == *ProductionSetVec[l])
                {
                    delete AddPdSet;
                    AddPdSet = ProductionSetVec[l];
                    IsFind = l;
                    break;
                }
            if (!IsFind)
            {
                AddPdSet->id = SetId++;
                CurSet->MyMap[TranSign[k].str] = k;
                CurSet->NextNode.push_back(AddPdSet);//建立关系
                ProductionSetVec.push_back(AddPdSet);//加入项目集
            }
            else
            {
                CurSet->MyMap[TranSign[k].str] = k;
                CurSet->NextNode.push_back(AddPdSet);//建立关系
            }
        }
    }

}

int GrammaAnalysis::Forward(QStandardItemModel* ProcessSetData)//前向传播，生成语法树
{
    int NodeId = 0;//用来区分不同的Node
    //初始化符号栈和状态栈
    SignStack.push(TreeNode{"#","#",StartTerminator});
    StateStack.push(0);

    //输出dot文件
    ofstream GraphFile(GraphOutDot.c_str(), ios::in | ios::trunc);//存在则删除
    GraphFile << ("digraph G{") << endl;
    GraphFile << ("edge[fontname = \"FangSong\"];") << endl;
    GraphFile << ("node[shape = box, fontname = \"FangSong\" size = \"20,20\"];") << endl;
    //从词法分析器中取元素，然后判断
    TreeNode InSign;//输入的符号
    SemanticTreeNode SInSign;//语义栈的符号
    int TopState;//栈顶的状态
    int WordPos = 0;//词语的位置
    Actions Action;//动作

    //debug 变量
    vector<TreeNode> SignStackVec;
    vector<int> StateStackVec;
    int i =0 ;
    while (WordPos < (int)WordSets->token_vec.size())
    {
        //初始化要使用的对象
        if (WordSets->token_vec[WordPos].type == 1)//从词法分析器中取出元素
        {
            InSign = TreeNode{ "ID" ,WordSets->token_vec[WordPos].word + " id=" + to_string(NodeId++),Terminator };
            if(this->IsSematic)
            {
                SInSign.Name = WordSets->token_vec[WordPos].word;//初始化SInSign的终结符
                SInSign.type = Terminator;
            }
        }
        else if (WordSets->token_vec[WordPos].type == 2)
        {
            InSign = TreeNode{ "num" ,WordSets->token_vec[WordPos].word + " id=" + to_string(NodeId++),Terminator };
            if(this->IsSematic)
            {
                SInSign.Name = WordSets->token_vec[WordPos].word;
                SInSign.ValTpye = FLOAT1;
                sscanf(SInSign.Name.c_str(), "%d", &SInSign.ival);
                sscanf(SInSign.Name.c_str(), "%f", &SInSign.fval);//初始化SInSign的终结符
                SInSign.type = Terminator;
            }
        }
        else
        {
            InSign = TreeNode{ WordSets->token_vec[WordPos].word,WordSets->token_vec[WordPos].word + " id=" + to_string(NodeId++),Terminator };
            if(this->IsSematic)
            {
                SInSign.Name = WordSets->token_vec[WordPos].word;
                SInSign.type = Terminator;//初始化SInSign的终结符
            }
        }
        TopState = StateStack.top();

        stringstream StateStackStr;
        stringstream SignStackStr;
        while(!StateStack.empty())
        {
            StateStackVec.push_back(StateStack.top());
            StateStack.pop();
        }
        for(int i=(int)StateStackVec.size()-1;i>=0;i--)
        {
            StateStack.push(StateStackVec[i]);
            if(i==(int)StateStackVec.size()-1)
                StateStackStr<<StateStackVec[i];
            else
                StateStackStr<<","<<StateStackVec[i];
        }
        while(!SignStack.empty())
        {
            SignStackVec.push_back(SignStack.top());
            SignStack.pop();
        }
        for(int i=(int)SignStackVec.size()-1;i>=0;i--)
        {
            SignStack.push(SignStackVec[i]);
            if(i==(int)SignStackVec.size()-1)
                SignStackStr<<SignStackVec[i].str;
            else
                SignStackStr<<","<<SignStackVec[i].str;
        }
        ProcessSetData->setItem(i, 0, new QStandardItem(QString("%1").arg(SignStackStr.str().c_str())));
        ProcessSetData->setItem(i, 1, new QStandardItem(QString("%1").arg(StateStackStr.str().c_str())));
        ProcessSetData->setItem(i, 2, new QStandardItem(QString("%1").arg(InSign.str.c_str())));
        SignStackVec.clear();
        StateStackVec.clear();
        if(ActionTable.count(pair<int, Sign>{TopState, Sign{ InSign.str,InSign.type }}) == 0)//说明没有找到
        {
            Tips("grammar error!!");
            string ErrorPos,ErrorInfo;
            for(int i=min(5,WordPos)-1;i>0;i--)
            {
                ErrorPos+=" ";
                ErrorPos+=WordSets->token_vec[WordPos-i].word;
            }

            for(int i=0;i<min(5,(int)WordSets->token_vec.size() - WordPos);i++)
            {
                ErrorPos+=" ";
                ErrorPos+=WordSets->token_vec[i+WordPos].word;
            }
            ErrorInfo = "语句错误，出错位置:\n" + ErrorPos + "\n的 "+WordSets->token_vec[WordPos].word+" 附近，请重新检查语法文件!!!";
            QMessageBox::information(NULL, "错误",ErrorInfo.c_str(),
                                     QMessageBox::Yes, QMessageBox::Yes);
            return -1;
        }
        Action = ActionTable[pair<int, Sign>{TopState, Sign{ InSign.str,InSign.type }}];
        if (Action.Type == Acc)//接受
        {
            cout << "success!!!" << endl;
            ProcessSetData->setItem(i, 3, new QStandardItem(QString("%1").arg("success!!")));
            break;
        }
        else if (Action.Type == Resolution)//归结
        {
            ProduceForms* ResoProduce = ProduceFormsVec[Action.ProduceFormsId];//将要归结的语句找到，因为下标和id有一一对应关系
            //下面是语义动作
            if(this->IsSematic)
            {
                SemanticLeftSign = ResoProduce->LeftSign;
                SemanticAction(ResoProduce);
                if(semanticerror!=None)
                {
                    SDisError(WordPos,semanticerror);
                    return -1;
                }
            }
            //下面是语法分析中的归结
            int PopSize = ResoProduce->RightSign.size();
            if (ResoProduce->RightSign[0].str == "空")
                PopSize = 0;
            while (PopSize--)//符号栈顶和状态栈顶元素全部出栈
            {
                string OutStr = "\"" + SignStack.top().StrType + "\"" + " -> " + "\"" + ResoProduce->LeftSign.str + " id=" + to_string(NodeId) + "\"" + ";";
                GraphFile << (OutStr.c_str()) << endl;
                SignStack.pop();
                StateStack.pop();
            }
            SignStack.push(TreeNode{ ResoProduce->LeftSign.str,ResoProduce->LeftSign.str + " id=" + to_string(NodeId++) ,ResoProduce->LeftSign.type });//产生式左部入栈
            TopState = StateStack.top();
            StateStack.push(GoTable[pair<int, Sign>{TopState, ResoProduce->LeftSign}]);
            //cout << "归结 :" << ResoProduce << endl;
            stringstream TempOutput;
            TempOutput <<"归结 :" << ResoProduce << endl;
            ProcessSetData->setItem(i, 3, new QStandardItem(QString("%1").arg(TempOutput.str().c_str())));

        }
        else//移进
        {
            StateStack.push(Action.State);
            SignStack.push(InSign);
//            cout<<"SInSign.Name:"<<SInSign.Name<<endl;
//            if(SInSign.Name == "program")
//            {
//                cout<<"SInSign.Name:"<<SInSign.Name<<endl;
//            }
            if(this->IsSematic)
            {
                SemanticStack.push(SInSign);
            }
//            cout<<"SemanticStack.top().Name:"<<SemanticStack.top().Name<<endl;
            WordPos++;
            ProcessSetData->setItem(i, 3, new QStandardItem(QString("%1").arg("移进")));
        }
        i++;
    }
    GraphFile << ("}") << endl;
    GraphFile.close();//关闭文件

    return 0;
}

void GrammaAnalysis::SDisError(int WordPos,SemanticError ErrType)
{
    string ErrorPos,ErrorInfo;
    for(int i=min(5,WordPos)-1;i>0;i--)
    {
        ErrorPos+=" ";
        ErrorPos+=WordSets->token_vec[WordPos-i].word;
    }

    for(int i=0;i<min(5,(int)WordSets->token_vec.size() - WordPos);i++)
    {
        ErrorPos+=" ";
        ErrorPos+=WordSets->token_vec[i+WordPos].word;
    }

    ErrorInfo = "语义错误:";
    if(ErrType == NoFuncName)
    {
        ErrorInfo+="调用未定义的函数:";
        ErrorInfo+=ErrorFuncName;
        ErrorInfo+="\n";
    }
    else if(ErrType == FuncParLenError)
    {
        ErrorInfo+="函数:";
        ErrorInfo+=ErrorFuncName;
        ErrorInfo+="参数定义与调用不一致\n";
    }
    else if(ErrType ==NoMainName)
    {
        ErrorInfo+="未定义main函数\n";
    }
    else if(ErrType ==FuncReturnErr)
    {
        ErrorInfo+="函数:";
        ErrorInfo+=ErrorFuncName;
        ErrorInfo+="返回类型与声明类型不一致\n";
    }
    else if(ErrType ==NoVar){
        ErrorInfo+="未定义变量";
        ErrorInfo+=ErrorVarName;
        ErrorInfo+="\n";
        ErrorInfo+="出错位置";
        ErrorInfo+=ErrorPos;
        ErrorInfo+="\n";
    }
    else if(ErrType ==RedefineVar){
        ErrorInfo+="重定义变量";
        ErrorInfo+=ErrorVarName;
        ErrorInfo+="\n";
        ErrorInfo+="出错位置";
        ErrorInfo+=ErrorPos;
        ErrorInfo+="\n";
    }
    QMessageBox::information(NULL, "错误",ErrorInfo.c_str(),
                             QMessageBox::Yes, QMessageBox::Yes);
    ErrType = None;
}



GrammaAnalysis::~GrammaAnalysis()
{
    for(int i=0;i<(int)ProduceFormsVec.size();i++)
        delete ProduceFormsVec[i];
    for(int i=0;i<(int)ProductionSetVec.size();i++)
        delete ProductionSetVec[i];
}
