#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Gramma_analysis.h"
#include "lexical_analysis.h"
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QDesktopWidget>
#include <QApplication>
#include <QTextEdit>
#include <QTableWidget>
#include <QFrame>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QButtonGroup>
#include <QRadioButton>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void init_window();
    void init_connect();
    ~MainWindow();
private slots:
    void FindInputWord();
    void AutoFillWord();
    void AnalysisLexical();
    void AnalysisGrammar();
    void DisplayTree();
    void DisplayTable();
    void InfoDisplay();
    void OperationInfo();
    void AutherInfo();
    void ShowLexicalRes();
    void ShowFirstSet();
    void ShowDFASet();
    void ShowProcessSet();
    void ShowTablesSet();
    void ShowMidCodeTable();

private:
    Ui::MainWindow *ui;
    int MainWindowWidth;
    int MainWindowHeight;

    QFileDialog* GetInputWordFile;//待检测文件输入
    QFileDialog* GetInputGrammaFile;//语法文件输入
    QPushButton* GetInputWord;//输出图文件 .png
    QPushButton* GetInputGramma;//输出表文件 .csv
    QPushButton* QutputTable;//输出表文件 .csv
    string LexicalFile;
    string GrammaFile;
    string OutputTree;
    string OutputTable;
    string OutputTreeDot;
    string LexicalOutput;
    string GraphExe;//输出文件的路径
    bool IsSematic;//是否进行语义分析
    //语义文件输出路径
    string VarOut,FuncOut,CodeOut;//变量表，函数表，中间代码表
    QMenuBar *menuBar;//目录
    QDesktopWidget *deskdop;//桌面信息

    //菜单的actions
    QAction *fileInfoAction;
    QAction *fileOperationAction;
    QAction *fileAuthorAction;

    //词法分析器界面
    int WordWhere;// 1从界面输入，2从文件输入
    QLabel *ResultTitle;
    QWidget* LexicalWindow;//词法分析器界面
    QTextEdit* WordInput;//界面输入的词法分析器内容
    QPushButton* LexicalSubmitBt;//确认分析按钮
    QTableView* LexicalResult;//通过表格展示词法分析器的结果
    QStandardItemModel* LexicalData;//储存数据的类型
    lexical_analysis * MyLexical;//词法分析器
    QPushButton* AutoFill;//一键填入

    //语法分析器界面
    QPushButton* GrammaBegin;
    QPushButton* LexicalResBt;
    QRadioButton* SematicBt;
    QLabel* IsSematicTest;

    GrammaAnalysis* g;
    //first集界面
    int HasFirstSet;//是否已经展示了
    QPushButton* ToFirstSet;
    QWidget* FirstSetWindow;
    QTableView* FirstSetResult;
    QStandardItemModel* FirstSetData;

    //DAF状态机界面
    int HasDFASet;//是否已经展示了
    QPushButton* ToDFASet;
    QWidget* DFASetWindow;
    QTableView* DFASetResult;
    QStandardItemModel* DFASetData;


    //栈过程输出界面
    int HasProcessSet;//是否已经展示了
    QPushButton* ToProcessSet;
    QWidget* ProcessSetWindow;
    QTableView* ProcessSetResult;
    QStandardItemModel* ProcessSetData;


    //Goto表和Action表界面
    int HasTablesSet;//是否已经展示了
    QStringList ColTitles;
    QPushButton* ToTablesSet;
    QWidget* TablesSetWindow;
    QTableView* TablesSetResult;
    QStandardItemModel* TablesSetData;

    //语法树的图
    QPushButton* OutputGraph;//输出图文件 .png

    //中间代码结果展示
    int HasMiddleCodeDis;
    QPushButton* ToMidCodeSet;
    QWidget* MidCodeSetWindow;
    QTableView* MidCodeSetResult;
    QStandardItemModel* MidCodeSetData;


};
#endif // MAINWINDOW_H
