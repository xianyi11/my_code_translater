#include"lexical_analysis.h"
#include<sstream>
const int sig_class_len = 15;
const int sig_len = 15;
//enum WORD_TYPE{Keyword,Identifier, Integer, Operator_1, Operator_2};
const char* sig_table[sig_class_len][sig_len] = {
    {"int","float","void","if","else","while","return",nullptr},//关键字
    {nullptr},//标识符
    {nullptr},//数值
    {"+","-","*","/","=","==",">",">=","<","<=","!=",nullptr},//算符
    {";",nullptr},//界符
    {",",nullptr},//分隔符
    {"/*","*/","/**/","//",nullptr},//注释号
    {"(",nullptr},//左括号
    {")",nullptr},//右括号
    {"{",nullptr},//左大括号
    {"}",nullptr},//右大括号
    {"[",nullptr},//左中括号
    {"]",nullptr},//右中括号
    {"#",nullptr},//结束符
};
const char* int2class[15] = { "关键字","标识符","数值","算符","界符","分隔符","注释号","左括号","右括号","左大括号","右大括号","左中括号","右中括号","结束符" };
lexical_analysis::lexical_analysis(const string file, const string _file_out)
{
    IsError=0;
    fin.open(file, ios::in|ios::binary);
    if (fin.is_open() == 0) {
        cout << "can't open the file, file path = " << file << endl;
        exit(-1);
    }
    int row = 0, col = 0;//指示当前行列
    int ch;
    string current_str;
    token current_token;
    while (fin.peek()!=-1) {
        ch = fin.get();
        //文件尾
        if (ch == -1) {
            break;
        }
        ++col;
        current_str = "";
        //空白字符
        if (isspace(ch)) {
            if (ch == '\r') {
                ++row;
                if (fin.peek() == '\n') {
                    fin.get();
                }
                col = 0;
            }
            else if (ch == '\t') {
                col += 3;
            }
        }
        //标识符
        else if (isalpha(ch)) {
            //循环读字母和数字
            while (isalpha(ch) || isdigit(ch)) {
                current_str += ch;
                ch = fin.get();
                ++col;
            }
            //终止 回退
            fin.seekg(-1, ios::cur);
            --col;
            //查找保留字表
            current_token = check_reverse_table(current_str);
            if (current_token.type == -1) {
                current_token.type = 1;//标识符
            }
            token_vec.push_back(current_token);
        }
        //
        else if (isdigit(ch)) {
            //循环读字母和数字
            fin.seekg(-1, ios::cur);
            float y;
            fin >> y;
            stringstream sstream;
            sstream << y;
            current_str = sstream.str();
            /*if (fabs(y - int(y)) > 1e-5) {
                current_str= std::to_string(y);
            }
            else {
                current_str = std::to_string(int(y));
            }*/
            //while (isdigit(ch)) {
            //	current_str += ch;
            //	ch = fin.get();
            //	++col;
            //}
            ////终止 回退
            //fin.seekg(-1, ios::cur);
            //--col;
            col += current_str.length();
            current_token.type = 2;//数值
            current_token.word = current_str;
            token_vec.push_back(current_token);
        }
        //单行注释
        else if (ch == '/' && fin.peek() == '/') {
            while ((ch=fin.get()) != '\n'&&ch!='\r')
                ;
            ++row;
            col = 0;
        }
        //如果是多行注释
        else if (ch == '/' && fin.peek() == '*') {
            while (!((ch = fin.get()) == '*' && fin.peek() == '/')) {
                ++col;
                if (ch == '\n'||ch=='\r') {
                    ++row;
                    col = 0;
                }
                else if (ch == '\t')
                    col += 3;
            }
            fin.get();
            col += 2;
        }
        else {
            while (!isdigit(ch) && !isalpha(ch) && !isspace(ch)) {
                current_str += ch;
                ch = fin.get();
                if (ch == -1)
                    break;
                ++col;
            }
            //终止 回退
            fin.seekg(-1, ios::cur);
            --col;
            //由长到短搜索
            int c_str_len = current_str.length();
            current_token = check_reverse_table_change_len(current_str);
            if (current_token.type != -1) {
                token_vec.push_back(current_token);
                //回退剩余部分
                fin.seekg((long long )current_token.word.length()-c_str_len,ios::cur);
            }
            else {
                IsError=1;
                error_info="";
                error_info+="识别到无法识别的字符，在第";
                error_info+=to_string(row+1);
                error_info+="行，第";
                error_info+=to_string(col+1) ;
                error_info+="列" ;
                return;
            }
        }

    }
    current_token.word = "#";
    current_token.type = 13;//结束符
    token_vec.push_back(current_token);
    fin.close();
    file_out = _file_out;
}
lexical_analysis::lexical_analysis(const string &in)
{
    IsError=0;
    istringstream fin(in);
    int row = 0, col = 0;//指示当前行列
    int ch;
    string current_str;
    token current_token;
    while (fin.peek()!=-1) {
        ch = fin.get();
        //文件尾
        if (ch == -1) {
            break;
        }
        ++col;
        current_str = "";
        //空白字符
        if (isspace(ch)) {
            if (ch == '\n') {
                ++row;
                if (fin.peek() == '\n') {
                    fin.get();
                }
                col = 0;
            }
            else if (ch == '\t') {
                col += 3;
            }
        }
        //标识符
        else if (isalpha(ch)) {
            //循环读字母和数字
            while (isalpha(ch) || isdigit(ch)) {
                current_str += ch;
                ch = fin.get();
                ++col;
            }
            //终止 回退
            fin.seekg(-1, ios::cur);
            --col;
            current_token = check_reverse_table(current_str);
            if (current_token.type == -1) {
                current_token.type = 1;//标识符
            }
            token_vec.push_back(current_token);
        }
        //
        else if (isdigit(ch)) {
            //循环读字母和数字
            fin.seekg(-1, ios::cur);
            float y;
            fin >> y;
            stringstream sstream;
            sstream << y;
            current_str = sstream.str();
            /*if (fabs(y - int(y)) > 1e-5) {
                current_str= std::to_string(y);
            }
            else {
                current_str = std::to_string(int(y));
            }*/
            //while (isdigit(ch)) {
            //	current_str += ch;
            //	ch = fin.get();
            //	++col;
            //}
            ////终止 回退
            //fin.seekg(-1, ios::cur);
            //--col;
            col += current_str.length();
            current_token.type = 2;//数值
            current_token.word = current_str;
            token_vec.push_back(current_token);
        }
        //单行注释
        else if (ch == '/' && fin.peek() == '/') {
            while ((ch=fin.get()) != '\n'&&ch!='\r')
                ;
            ++row;
            col = 0;
        }
        //如果是多行注释
        else if (ch == '/' && fin.peek() == '*') {
            while (!((ch = fin.get()) == '*' && fin.peek() == '/')) {
                ++col;
                if (ch == '\n'||ch=='\r') {
                    ++row;
                    col = 0;
                }
                else if (ch == '\t')
                    col += 3;
            }
            fin.get();
            col += 2;
        }
        else {
            while (!isdigit(ch) && !isalpha(ch) && !isspace(ch)) {
                current_str += ch;
                ch = fin.get();
                if (ch == -1)
                    break;
                ++col;
            }
            //终止 回退
            fin.seekg(-1, ios::cur);
            --col;
            //由长到短搜索
            int c_str_len = current_str.length();
            current_token = check_reverse_table_change_len(current_str);
            if (current_token.type != -1) {
                token_vec.push_back(current_token);
                fin.seekg((long long )current_token.word.length()-c_str_len,ios::cur);
            }
            else {
                IsError=1;
                error_info="";
                error_info+="识别到无法识别的字符，在第";
                error_info+=to_string(row+1);
                error_info+="行，第";
                error_info+=to_string(col+1) ;
                error_info+="列" ;
                return;
            }
        }

    }
    current_token.word = "#";
    current_token.type = 13;//结束符
    token_vec.push_back(current_token);
}
token lexical_analysis::check_reverse_table(const string& str)
{
    for (int i = 0; i < sig_class_len; i++)
        for (int j = 0; j < sig_len; j++)
        {
            if (sig_table[i][j] == nullptr)
            {
                break;
            }
            if (str==sig_table[i][j])//找到了
            {
                return { str , i };
            }
        }
    return { str , -1 };
}
token lexical_analysis::check_reverse_table_change_len(string& str)
{
    token t;
    while (str.length()) {
        t = check_reverse_table(str);
        if (t.type == -1) {
            str.erase(str.end() - 1);
        }
        else
            return t;
    }
    return { str , -1 };
}
void lexical_analysis::print_table()//打印出符号表
{
    for (int i = 0; i < int(token_vec.size()); i++)
    {
        cout << "(" << token_vec[i].word<< ", " << int2class[token_vec[i].type] << ")" << endl;
    }
}
void lexical_analysis::print_table_to_file()//打印出符号表
{
    ofstream fout;
    fout.open(file_out, ios::out);
    if (fout.is_open() == 0) {
        cout << "can't open the file, file path = " << file_out << endl;
        exit(-1);
    }
    for (int i = 0; i < int(token_vec.size()); i++)
    {
        fout << "(" << token_vec[i].word << ", " << int2class[token_vec[i].type] << ")" << endl;
    }
    fout.close();
}
lexical_analysis::~lexical_analysis()
{
}
