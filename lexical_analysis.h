#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include<set>
#include<cstring>
using namespace std;

class GrammaAnalysis;
class MainWindow;
struct token
{
    string word;//token的内容
    int type;//token的类型
};
class lexical_analysis
{
public:
    friend MainWindow;
    friend GrammaAnalysis;
    lexical_analysis() = delete;
    lexical_analysis(const string file,const string _file_out);
    lexical_analysis(const string &in);
    void print_table();
    void print_table_to_file();
    int is_error(){
        return IsError;
    }
    string error_string(){
        return error_info;
    }
    ~lexical_analysis();

private:
    ifstream fin;//输入文件流
    string file_out;//输出文件
    string error_info;//错误信息
    int IsError;//错误状态
    vector<token>token_vec;//返回词法分析结果
    token check_reverse_table(const string& str);//检查一串字符是否包含关键字
    token check_reverse_table_change_len(string& str);
    
};
