#include "Gramma_analysis.h"
using namespace std;
static string& ClearAllSpace(string& str)
{
    size_t index = 0;
    if (!str.empty())
    {
        while ((index = str.find(' ', index)) != string::npos)
        {
            str.erase(index, 1);
        }
    }
    return str;
}
// right 可能是 <> {} '' [] |
static void analysis_right(vector<Sign>& RightSign,const string&str)
{
    istringstream sin(str);
    string s;
    Sign sign;
    while (sin >> sign.str) {
        sign.type = (sign.str[0] == '<'&& sign.str[sign.str.length()-1] == '>' ? NonTerminator:  Terminator);
        RightSign.push_back(sign);
    }
}

ostream& operator << (ostream& os, const Actions& s)
{
    if (s.Type == MoveIn) {
        os << "s" << s.State;
    }
    else if (s.Type == Resolution) {
        os << "r" << s.ProduceFormsId;
    }
    else {
        os << "acc";
    }
    return os;
}
ofstream& operator << (ofstream& os, const Actions& s)
{
    if (s.Type == MoveIn) {
        os << "s" << s.State;
    }
    else if (s.Type == Resolution) {
        os << "r" << s.ProduceFormsId;
    }
    else {
        os << "acc";
    }
    return os;
}
//GrammaAnalysis::GrammaAnalysis(lexical_analysis* MyWordSets, const string _FilePath, const string _TableOutput, const string _GraphOutput)
//{
//	WordSets = MyWordSets;
//	FilePath = _FilePath;
//	TableOut = _TableOutput;
//	GraphOut = _GraphOutput;
//	ProduceFormsGenerate();
//	for (auto pf_p : ProduceFormsVec)
//	{
//		GetFirstSign(pf_p->LeftSign);
//	}
//	cout << "-------------------------------------------" << endl << "下面是first集" << endl;
//	for (auto i : first_table) {
//		cout << "first(" << i.first << ")={";
//		for (auto j : i.second) {
//			cout << j.str << " ";
//		}
//		cout <<"}"<< endl;
//	}
//}
//输出提示
void GrammaAnalysis::Tips(const string& str)
{
    cout << "--------------------------------------------" << endl;
    cout << str << endl;
    cout << "--------------------------------------------" << endl;
}
//输出ProduceFormsVec
void GrammaAnalysis::PrintProduceFormsVec()
{
    Tips("All ProduceForms");
    for (auto i : ProduceFormsVec) {
        cout << i->LeftSign.str << " " << (i->LeftSign.type == Terminator ? "终结符" : "非终结符") << "->";
        for (auto j : i->RightSign) {
            cout << j.str << " " << (j.type == Terminator ? "终结符" : "非终结符") << " ";
        }
        cout << endl;
    }
    Tips("AllNonTerminator");
    for (auto i : AllNonTerminator) {
        cout << i.str << endl;
    }
    Tips("AllTerminator");
    for (auto i : AllTerminator) {
        cout << i.str << endl;
    }
}
//产生一个包含ProduceForms的vector
void GrammaAnalysis::ProduceFormsGenerate()
{
    ifstream fin;
    fin.open(FilePath, ios::in);//故意十进制打开,getline行尾统一处理
    if (!fin.is_open()) {
        cerr << "can't open " << FilePath << endl;
        exit(-1);
    }
    string str;
    int i = 0,j=0;
    while (1) {
        fin.clear();
        getline(fin, str);
        ++i;
        if (!str.length()) {
            if (fin.eof()) {
                break;
            }
            else {
                continue;
            }
        }
        //注释
        if (str[0] == '$') {
            continue;
        }
        auto pos=str.find("::=");
        if (pos == str.npos) {
            cerr << "error at line" << i <<" str:"<< str << endl;
            exit(-1);//想改成带返回值的函数
        }
        ++j;//记录有效行 寻找开始符
        ProduceForms pf;
        //左部直接取 认为第一个为开始符
        pf.LeftSign.str = str.substr(0, pos);
        ClearAllSpace(pf.LeftSign.str);
        pf.LeftSign.type = (j == 1 ? StartTerminator : NonTerminator);
        string right_s=str.substr(pos+3);
        //每个|是一个右部
        while ((pos=right_s.find("|")) != string::npos) {
            ProduceForms*ppf = new ProduceForms;
            *ppf = pf;
            ppf->id = j - 1;
            ++j;
            string right_s_one= right_s.substr(0, pos);
            right_s= right_s.substr(pos+1);
            analysis_right(ppf->RightSign,right_s_one);
            ProduceFormsVec.push_back(ppf);
        }
        //最后一个
        ProduceForms* ppf = new ProduceForms;
        *ppf = pf;
        ppf->id = j - 1;
        analysis_right(ppf->RightSign, right_s);
        ProduceFormsVec.push_back(ppf);
    }
    for (auto i : ProduceFormsVec) {
        AllNonTerminator.insert(i->LeftSign);
        for (auto j : i->RightSign) {
            if (j.type == Terminator|| j.type==Empty) {
                AllTerminator.insert(j);
            }
        }
    }
}


//求某个符号的FIRST集
//对于终结符，FIRST集就是自己

//对于非终结符
    /*
    若X ∈VN，且有产生式X→a…,a∈VT，则把a加入到FIRST（X）中，若有X→ε，则把ε加入FIRST（X);
    若X∈VN , 且X→Y … , Y∈VN, 则把FIRST (Y) - {ε}加到FIRST (X)中
    若X→Y1Y2 … Yk,Y1, Y2, … ,Yi-1 ∈ VN,ε∈FIRST(Yj)
    则把(1<= j <= i -1)FIRST (Yi) - {ε}加到FIRST (X)中。
    特别地，若ε∈FIRST (Yj)(1<=j <= k )，则ε∈FIRST(X)
    */
set<Sign> GrammaAnalysis::GetFirstSign(Sign InSign)
{
    set<Sign> sign_vec;
    if (InSign.type == Terminator) {
        sign_vec.insert(InSign);
        return sign_vec;
    }
    //存在就返回
    if (!first_table[InSign.str].empty()) {
        return first_table[InSign.str];
    }
    //遍历产生式
    for (auto pf_p : ProduceFormsVec) {
        //左部相等
        if (pf_p->LeftSign == InSign) {
            //第一个是Terminator
            if (pf_p->RightSign[0].type == Terminator) {
                first_table[InSign.str].insert(pf_p->RightSign[0]);
            }
            //不是的话 遍历右部
            else {
                size_t length=0;
                for (auto& right : pf_p->RightSign) {
                    auto right_set = GetFirstSign(right);
                    int zero = 0;
                    for (auto& right_set_sign : right_set)
                    {
                        //去除first-{ε}
                        if (right_set_sign.str == "空") {
                            zero = 1;
                            continue;
                        }
                        //非空插入
                        first_table[InSign.str].insert(right_set_sign);
                    }
                    //说明没有{ε} 不必加入下一个的first
                    if (!zero) {
                        break;
                    }
                    ++length;
                }
                //遍历完右部都有{ε} 将{ε}加入first
                if (length == pf_p->RightSign.size()) {
                    first_table[InSign.str].insert(Sign("空",Terminator));
                }
            }
        }
    }

    return first_table[InSign.str];
}
//输入连续字符串，得到first集
//1.对于符号串α= X1X2… Xn，构造 FIRST (α)
//2.若对所有的 Xj ,1<=j<= i -1, ε∈FIRST (Xj), 则把FIRST(Xi) -{ε}加到FIRST(α)中
//3.若对所有的 Xj, 1 <= j <= n, ε∈FIRST(Xj), 则把ε加到FIRST(α)中
set<Sign> GrammaAnalysis::GetFirstSign(vector<Sign> InSign)
{
    set<Sign> set_sign;
    size_t i;
    for (i = 0; i < InSign.size(); ++i) {
        auto right_set = GetFirstSign(InSign[i]);
        int zero = 0;
        for (auto& right_set_sign : right_set)
        {
            //去除first-{ε}
            if (right_set_sign.str == "空") {
                zero = 1;
                continue;
            }
            //非空插入
            set_sign.insert(right_set_sign);
        }
        //说明没有{ε} 不必加入下一个的first
        if (!zero) {
            break;
        }
    }
    if (i == InSign.size()) {
        set_sign.insert(Sign("空", Terminator));
    }
    return set_sign;
}
void GrammaAnalysis::Conflict_Detection(const Actions&action, pair<int, Sign>&tableindex,int i, LR1Production*j)
{
    if (ActionTable.count(tableindex)) {
        if (ActionTable[tableindex] != action) {
            cerr << "error 归约冲突!已经有action[" << i << "," << j->Foresee.str << "]=" << action << endl;
            exit(-1);
        }
    }
}
//这里还要有Action和Goto的函数的建立
void GrammaAnalysis::BuildActionGo()
{
    Tips("BuildActionGo");
    //goto表
    for (int i = 0; (int)i < ProductionSetVec.size(); i++) {
        for (auto& sign : AllNonTerminator) {
            if (ProductionSetVec[i]->MyMap.count(sign.str)) {
                auto ProductionSet_p = ProductionSetVec[i]->NextNode[ProductionSetVec[i]->MyMap[sign.str]];
                GoTable[{i, sign}] = ProductionSet_p->id;
            }
        }
    }
    pair<int, Sign>tableindex;
    Actions action;
    //action表
    for (int i = 0; (int)i < ProductionSetVec.size(); i++) {
        for (auto j : ProductionSetVec[i]->LR1Productions) {
            //归约
            if (j->DotPosition == j->RightSign.size()) {
                if (j->Foresee.str != "#"||j->id!=0) {
                    tableindex = { i, j->Foresee };
                    action= Actions{ -1,j->id,Resolution };
                    Conflict_Detection(action, tableindex, i, j);
                    ActionTable[tableindex] = action;
                }
                else {
                    tableindex = { i, j->Foresee };
                    action = Actions{ -1,j->id,Acc };
                    Conflict_Detection(action, tableindex, i, j);
                    ActionTable[tableindex] = action;
                }

            }
            else
            {
                auto& sign = j->RightSign[j->DotPosition];
                if (sign.type == Terminator) {
                    if (sign.str == "空") {
                        tableindex = { i, j->Foresee };
                        action = Actions{ -1,j->id,Resolution };
                        Conflict_Detection(action, tableindex, i, j);
                        ActionTable[tableindex] = action;
                    }
                    else {
                        if (ProductionSetVec[i]->MyMap.count(sign.str)) {
                            auto ProductionSet_p = ProductionSetVec[i]->NextNode[ProductionSetVec[i]->MyMap[sign.str]];
                            tableindex = { i, sign };
                            action = Actions{ ProductionSet_p->id,-1,MoveIn };
                            Conflict_Detection(action, tableindex, i, j);
                            ActionTable[tableindex] = action;
                        }
                    }

                }
                //归约
                //else if (sign.type == Empty) {
                //	tableindex = { i, j->Foresee };
                //	action = Actions{ -1,j->id,Resolution };
                //	if (ActionTable.count(tableindex)) {
                //		if (ActionTable[tableindex] != action) {
                //			cerr << "error 归约冲突!已经有action[" << i << "," << j->Foresee.str << "]=" << action << endl;
                //			//exit(-1);
                //		}

                //	}
                //	ActionTable[tableindex] = action;
                //}
            }
        }
    }
    Tips("BuildActionGo Success");

}
void  GrammaAnalysis::PrintActionGo()
{
    Tips("PrintActionGo");
    for (auto& i : GoTable) {
        cout << "goto[" << i.first.first << "," << i.first.second.str << "]=" << i.second << endl;
    }
    for (auto& i : ActionTable) {
        cout << "action[" << i.first.first << "," << i.first.second.str << "]=" << i.second << endl;
    }
}
//打印action和goto
void GrammaAnalysis::PrintActionGoToFile()
{
    Tips("PrintActionGoToFile");
    ofstream fout;
    fout.open(TableOut, ios::out);
    if (!fout.is_open())
    {
        cerr << "open" << TableOut << "error" << endl;
        exit(-1);
    }
    pair<int, Sign>tableindex;
    Actions action;
    fout << "状态,";
    for (auto& terminator : AllTerminator) {
        if (terminator.str == ",")
            fout << "\",\",";
        else if(terminator.str == "空")
            fout<< "#,";
        else
            fout << terminator.str << ",";
    }

    for (auto& terminator : AllNonTerminator) {
        fout << terminator.str << ",";
    }
    fout << "\n";
    for (int i = 0; i < (int)ProductionSetVec.size(); i++) {
        fout << i << ",";
        for (auto& terminator : AllTerminator) {
            tableindex = { i, terminator };
            if (tableindex.second.str == "空") {
                tableindex.second.str = "#";
            }
            if (ActionTable.count(tableindex)) {
                action = ActionTable[tableindex];
                fout << action << ",";
            }
            else {
                fout << " ,";
            }
        }
        int id;
        for (auto &terminator : AllNonTerminator) {
            tableindex = { i, terminator };
            if (GoTable.count(tableindex)) {
                id = GoTable[tableindex];
                fout << id << ",";
            }
            else {
                fout << " ,";
            }

        }
        fout << "\n";
    }
    fout.close();
    Tips("PrintActionGoToFile Success");
}
