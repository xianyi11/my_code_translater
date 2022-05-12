#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <iostream>
#include <sstream>

extern char* int2class[15];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_window();
    init_connect();
//    lexical_analysis my_lexical_analysis("test.txt");
//    my_lexical_analysis.print_table();
//    GrammaAnalysis g(&my_lexical_analysis, "grammer.txt", "OutputTable.txt", "OutputGraph.dot");
//    g.Forward();
}

void MainWindow::init_window()
{
    deskdop = QApplication::desktop();
    MainWindowWidth = int(1000);
    MainWindowHeight = int(700);
    this->resize(MainWindowWidth,MainWindowHeight);
    this->move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);
    this->setWindowTitle("语法分析器");
    this->setObjectName("mainwindow");
    this->IsSematic = true;

    menuBar = new QMenuBar(this); //1.创建菜单栏
    menuBar->setGeometry(0,0,width(),25);   //设置大小

    QMenu *fileMenu = new QMenu("帮助",this);   //2.创建菜单
    //3.创建行为(Action)
    fileInfoAction = new QAction("软件说明",this);
    fileOperationAction = new QAction("操作说明",this);
    fileAuthorAction = new QAction("关于作者",this);
    //4.将行为添加到菜单
    fileMenu->addAction(fileInfoAction);
    fileMenu->addAction(fileOperationAction);
    fileMenu->addAction(fileAuthorAction);
    //5.将菜单添加到菜单栏
    menuBar->addMenu(fileMenu);


    // 词法分析器界面的初始化
    WordWhere = 0;
    LexicalWindow = new QWidget(this);
    LexicalWindow->setGeometry(20,40,700,650);

    ResultTitle = new QLabel(LexicalWindow);
    ResultTitle->setGeometry(400,0,400,30);
    ResultTitle->setText("词法分析器输出区域");
    ResultTitle->setStyleSheet("font-size:16px;");

    GetInputWord = new QPushButton(LexicalWindow);
    GetInputWord->setGeometry(0,610,150,30);
    GetInputWord->setText("请选择输入语句文件");
    GetInputWord->setStyleSheet("font-size:16px;");

    AutoFill = new QPushButton(LexicalWindow);
    AutoFill->setGeometry(180,610,150,30);
    AutoFill->setText("填入默认文件");
    AutoFill->setStyleSheet("font-size:16px;");


    LexicalSubmitBt = new QPushButton(LexicalWindow);
    LexicalSubmitBt->setGeometry(360,610,50,30);
    LexicalSubmitBt->setText("确认");
    LexicalSubmitBt->setStyleSheet("font-size:16px;");


    WordInput = new QTextEdit(LexicalWindow);
    WordInput->setGeometry(0,0,370,600);
    WordInput->setPlaceholderText("语句输入区域\n\n可以以文件方式输入，也可以以界面方式输入，优先以界面方式输入。");
    WordInput->setStyleSheet("font-size:14px;");

    LexicalResult = new QTableView(LexicalWindow);
    LexicalResult->setGeometry(400,30,300,570);
    LexicalData = new QStandardItemModel();
    LexicalData->setHorizontalHeaderLabels({"词语", "类型"});
    LexicalResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    LexicalResult->setModel(LexicalData);
    LexicalWindow->show();


    //语法分析器初始化
    GrammaBegin = new QPushButton(this);
    GrammaBegin->setGeometry(760,70,200,30);
    GrammaBegin->setText("开始语法分析");
    GrammaBegin->setStyleSheet("font-size:14px;");
    GrammaBegin->hide();

    SematicBt = new QRadioButton(this);
    SematicBt->setGeometry(760,120,200,30);
    SematicBt->setText("是否进行语义检查");
    SematicBt->setStyleSheet("font-size:14px;");
    SematicBt->hide();

    IsSematicTest = new QLabel(this);
    IsSematicTest->setGeometry(760,120,200,30);
    IsSematicTest->setStyleSheet("font-size:14px;");
    IsSematicTest->hide();


    LexicalResBt = new QPushButton(this);
    LexicalResBt->setGeometry(760,170,200,30);
    LexicalResBt->setText("词法分析结果/返回词法分析");
    LexicalResBt->setStyleSheet("font-size:14px;");
    LexicalResBt->hide();

    ToFirstSet = new QPushButton(this);
    ToFirstSet->setGeometry(760,220,200,30);
    ToFirstSet->setText("First集");
    ToFirstSet->setStyleSheet("font-size:14px;");
    ToFirstSet->hide();

    ToDFASet = new QPushButton(this);
    ToDFASet->setGeometry(760,270,200,30);
    ToDFASet->setText("DFA状态集合");
    ToDFASet->setStyleSheet("font-size:14px;");
    ToDFASet->hide();

    ToProcessSet = new QPushButton(this);
    ToProcessSet->setGeometry(760,320,200,30);
    ToProcessSet->setText("分析栈过程");
    ToProcessSet->setStyleSheet("font-size:14px;");
    ToProcessSet->hide();

    ToTablesSet = new QPushButton(this);
    ToTablesSet->setGeometry(760,370,200,30);
    ToTablesSet->setText("Action/Goto表");
    ToTablesSet->setStyleSheet("font-size:14px;");
    ToTablesSet->hide();

    OutputGraph = new QPushButton(this);
    OutputGraph->setGeometry(760,420,200,30);
    OutputGraph->setText("语法树输出");
    OutputGraph->setStyleSheet("font-size:14px;");
    OutputGraph->hide();

    ToMidCodeSet = new QPushButton(this);
    ToMidCodeSet->setGeometry(760,470,200,30);
    ToMidCodeSet->setText("中间代码表");
    ToMidCodeSet->setStyleSheet("font-size:14px;");
    ToMidCodeSet->hide();

    //first集界面
    FirstSetWindow = new QWidget(this);
    FirstSetWindow->setGeometry(20,40,700,650);
    FirstSetResult = new QTableView(FirstSetWindow);
    FirstSetResult->setGeometry(0,0,700,650);
    FirstSetData = new QStandardItemModel();
    FirstSetData->setHorizontalHeaderLabels({"句型", "first集"});
    FirstSetResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    FirstSetResult->setModel(FirstSetData);
    FirstSetWindow->hide();

    //DFA集界面
    DFASetWindow = new QWidget(this);
    DFASetWindow->setGeometry(20,40,700,650);
    DFASetResult = new QTableView(DFASetWindow);
    DFASetResult->setGeometry(0,0,700,650);
    DFASetData = new QStandardItemModel();
    DFASetData->setHorizontalHeaderLabels({"DFA集合内容", "NextNode"});
    DFASetResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    DFASetResult->setModel(DFASetData);
    DFASetWindow->hide();

    //栈过程界面
    ProcessSetWindow = new QWidget(this);
    ProcessSetWindow->setGeometry(20,40,700,650);
    ProcessSetResult = new QTableView(ProcessSetWindow);
    ProcessSetResult->setGeometry(0,0,700,650);
    ProcessSetData = new QStandardItemModel();
    ProcessSetData->setHorizontalHeaderLabels({"符号栈", "状态栈","输入符号","动作"});
    ProcessSetResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ProcessSetResult->setModel(ProcessSetData);
    ProcessSetWindow->hide();

    //GOTO表和Action表界面
    TablesSetWindow = new QWidget(this);
    TablesSetWindow->setGeometry(20,40,700,650);
    TablesSetResult = new QTableView(TablesSetWindow);
    TablesSetResult->setGeometry(0,0,700,650);
    TablesSetData = new QStandardItemModel();
    TablesSetWindow->hide();


    QutputTable = new QPushButton(this);
    QutputTable->setGeometry(250,220,120,30);
    QutputTable->setText("Action,Goto表");
    QutputTable->setStyleSheet("font-size:16px;");
    QutputTable->hide();

    //中间代码结果展示
    MidCodeSetWindow = new QWidget(this);
    MidCodeSetWindow->setGeometry(20,40,700,650);
    MidCodeSetResult = new QTableView(MidCodeSetWindow);
    MidCodeSetResult->setGeometry(0,0,700,650);
    MidCodeSetData = new QStandardItemModel();
    MidCodeSetData->setHorizontalHeaderLabels({"标号","操作", "操作数1","操作数2","结果"});
    MidCodeSetResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    MidCodeSetResult->setModel(MidCodeSetData);
    MidCodeSetWindow->hide();

    GrammaFile = QCoreApplication::applicationDirPath().toStdString()+"/grammar_1.txt";
    OutputTree = QCoreApplication::applicationDirPath().toStdString()+"/graph.png";
    OutputTable =QCoreApplication::applicationDirPath().toStdString()+"/table.csv";
    GraphExe = QCoreApplication::applicationDirPath().toStdString()+"/Graphviz/bin/dot";
    OutputTreeDot = QCoreApplication::applicationDirPath().toStdString()+"/graph.dot";
    LexicalOutput = QCoreApplication::applicationDirPath().toStdString()+"/LexicalOutput.txt";
    VarOut = QCoreApplication::applicationDirPath().toStdString()+"/VarTable.txt";
    FuncOut = QCoreApplication::applicationDirPath().toStdString()+"/FuncTable.txt";
    CodeOut = QCoreApplication::applicationDirPath().toStdString()+"/CodeTable.txt";
    //    QPushButton* QutputTable;//输出表文件 .csv

}



void MainWindow::AutoFillWord()
{
    WordInput->clear();
    string TestFile = QCoreApplication::applicationDirPath().toStdString()+"/testword.txt";
    ifstream fin(TestFile.c_str(),ios::in);
    string errinfo = "语法测试文件打开失败:"+TestFile;
    if(!fin.is_open())
        QMessageBox::critical(NULL, "错误", errinfo.c_str(), QMessageBox::Yes, QMessageBox::Yes);
    char line[1024] = {0};
    fin.clear();//清空错误标志
    while(!fin.eof())
    {
        fin.getline(line,1024);
        WordInput->append(line);
    }
}

void MainWindow::DisplayTree()
{
    QString hglpName = OutputTree.c_str();
    QString hglpPath = QString("%1").arg(hglpName);
    cout<<hglpPath.toStdString()<<endl;
    string cmd = hglpPath.toStdString();
    system(cmd.c_str());
}


void MainWindow::DisplayTable()
{
    QString hglpName = OutputTable.c_str();
    QString hglpPath = QString("%1").arg(hglpName);
    QFile bfilePath(hglpPath);
    std::cout<<hglpPath.toStdString()<<endl;
    if(!bfilePath.exists()){
        return;
    }
    QString filePath = "file:///" + hglpPath;   //打开文件夹用filse:///,打开网页用http://
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::init_connect()
{
    connect(GetInputWord,SIGNAL(clicked()), this, SLOT(FindInputWord()));
    connect(LexicalSubmitBt,SIGNAL(clicked()), this, SLOT(AnalysisLexical()));
    connect(OutputGraph,SIGNAL(clicked()), this, SLOT(DisplayTree()));
    connect(QutputTable,SIGNAL(clicked()), this, SLOT(DisplayTable()));
    connect(fileInfoAction,SIGNAL(triggered()),this,SLOT(InfoDisplay()));
    connect(fileOperationAction,SIGNAL(triggered()),this,SLOT(OperationInfo()));
    connect(fileAuthorAction,SIGNAL(triggered()),this,SLOT(AutherInfo()));
    connect(GrammaBegin,SIGNAL(clicked()),this,SLOT(AnalysisGrammar()));
    connect(LexicalResBt,SIGNAL(clicked()),this,SLOT(ShowLexicalRes()));
    connect(ToFirstSet,SIGNAL(clicked()),this,SLOT(ShowFirstSet()));
    connect(ToDFASet,SIGNAL(clicked()),this,SLOT(ShowDFASet()));
    connect(ToProcessSet,SIGNAL(clicked()),this,SLOT(ShowProcessSet()));
    connect(ToTablesSet,SIGNAL(clicked()),this,SLOT(ShowTablesSet()));
    connect(AutoFill,SIGNAL(clicked()),this,SLOT(AutoFillWord()));
    connect(ToMidCodeSet,SIGNAL(clicked()),this,SLOT(ShowMidCodeTable()));
}

void MainWindow::ShowMidCodeTable()
{
    FirstSetWindow->hide();
    LexicalWindow->hide();
    DFASetWindow->hide();
    TablesSetWindow->hide();
    ProcessSetWindow->hide();
    MidCodeSetWindow->show();
    if(HasMiddleCodeDis==0)
    {
        MidCodeSetResult->setWordWrap(true);
        QHeaderView* headerView = MidCodeSetResult->verticalHeader();
        headerView->setHidden(true);
        for(int i=0;i<(int)g->MiddleCodeTable.size();i++)
        {
            MidCodeSetData->setItem(i, 0, new QStandardItem(QString("%1").arg(to_string(i).c_str())));
            MidCodeSetData->setItem(i, 1, new QStandardItem(QString("%1").arg(g->MiddleCodeTable[i].op.c_str())));
            MidCodeSetData->setItem(i, 2, new QStandardItem(QString("%1").arg(g->MiddleCodeTable[i].op1.c_str())));
            MidCodeSetData->setItem(i, 3, new QStandardItem(QString("%1").arg(g->MiddleCodeTable[i].op2.c_str())));
            MidCodeSetData->setItem(i, 4, new QStandardItem(QString("%1").arg(g->MiddleCodeTable[i].linkres.c_str())));
        }
        MidCodeSetResult->setModel(MidCodeSetData);
        MidCodeSetResult->resizeRowsToContents();
        HasMiddleCodeDis = 1;
    }
}

void MainWindow::ShowTablesSet()
{
    FirstSetWindow->hide();
    LexicalWindow->hide();
    DFASetWindow->hide();
    ProcessSetWindow->hide();
    TablesSetWindow->show();
    MidCodeSetWindow->hide();
    if(HasTablesSet==0)
    {
        TablesSetResult->setWordWrap(true);
        for(auto iter1 = g->AllTerminator.begin();iter1!=g->AllTerminator.end();iter1++){
            if(iter1->str=="空"){
                ColTitles.append("#");
            }
            else
                ColTitles.append(iter1->str.c_str());
        }

        for(auto iter1 = g->AllNonTerminator.begin();iter1!=g->AllNonTerminator.end();iter1++)
            ColTitles.append(iter1->str.c_str());
        TablesSetData->setHorizontalHeaderLabels(ColTitles);
        pair<int, Sign>tableindex;
        TablesSetResult->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        for(int i=0;i<(int)g->ProductionSetVec.size();i++)
        {
            int j = 0;
            for(auto iter1 = g->AllTerminator.begin();iter1!=g->AllTerminator.end();iter1++,j++)
            {
                stringstream OutputStr;
                tableindex={i,*iter1};
                if(tableindex.second.str=="空"){
                    tableindex.second.str="#";
                }
                if(g->ActionTable.count(tableindex)!=0)
                {
                    Actions ThisAction = g->ActionTable[tableindex];
                    if(ThisAction.Type == MoveIn)
                        OutputStr<<"S"<<ThisAction.State;
                    else if(ThisAction.Type == Resolution)
                        OutputStr<<"R"<<ThisAction.ProduceFormsId;
                    else
                        OutputStr<<"ACC";
                    TablesSetData->setItem(i, j, new QStandardItem(QString("%1").arg(OutputStr.str().c_str())));
                }
            }
            for(auto iter1 = g->AllNonTerminator.begin();iter1!=g->AllNonTerminator.end();iter1++,j++)
            {
                if(g->GoTable.count(pair<int, Sign>{i,*iter1})!=0)
                {
                    stringstream OutputStr;
                    int ThisAction = g->GoTable[pair<int, Sign>{i,*iter1}];
                    OutputStr << ThisAction;
                    TablesSetData->setItem(i, j, new QStandardItem(QString("%1").arg(OutputStr.str().c_str())));
                }
            }
        }
        TablesSetResult->setModel(TablesSetData);
        TablesSetResult->resizeRowsToContents();
        HasTablesSet = 1;
    }
}


void MainWindow::ShowProcessSet()
{
    FirstSetWindow->hide();
    LexicalWindow->hide();
    DFASetWindow->hide();
    TablesSetWindow->hide();
    ProcessSetWindow->show();
    MidCodeSetWindow->hide();
    if(HasProcessSet==0)
    {
        ProcessSetResult->setWordWrap(true);
        ProcessSetResult->setModel(ProcessSetData);
        ProcessSetResult->resizeRowsToContents();
        HasProcessSet = 1;
    }
}

void MainWindow::ShowDFASet()
{
    FirstSetWindow->hide();
    LexicalWindow->hide();
    TablesSetWindow->hide();
    DFASetWindow->show();
    ProcessSetWindow->hide();
    MidCodeSetWindow->hide();
    if(HasDFASet==0)
    {
        DFASetResult->setWordWrap(true);
        for(int i= 0;i<(int)g->ProductionSetVec.size();i++)
        {
            stringstream SetContent;
            stringstream NextNode;
            for (int j = 0; j < (int)g->ProductionSetVec[i]->LR1Productions.size(); j++)
                SetContent << g->ProductionSetVec[i]->LR1Productions[j] << endl;
            for (auto j : g->ProductionSetVec[i]->MyMap)
                NextNode << "--" << j.first << "-->" << g->ProductionSetVec[i]->NextNode[j.second]->id<<"|";
            DFASetData->setItem(i, 0, new QStandardItem(QString("%1").arg(SetContent.str().c_str())));
            if(NextNode.str().length() == 0)
                DFASetData->setItem(i, 1, new QStandardItem(QString("%1").arg("NULL")));
            else
                DFASetData->setItem(i, 1, new QStandardItem(QString("%1").arg(NextNode.str().c_str())));
        }
        DFASetResult->setModel(DFASetData);
        DFASetResult->resizeRowsToContents();
        HasDFASet = 1;
    }

}
void MainWindow::ShowFirstSet()
{
    FirstSetWindow->show();
    LexicalWindow->hide();
    DFASetWindow->hide();
    TablesSetWindow->hide();
    ProcessSetWindow->hide();
    MidCodeSetWindow->hide();
    if(HasFirstSet==0)
    {
        int i = 0;
        for(auto iter = g->first_table.begin(); iter != g->first_table.end(); iter++,i++){
            FirstSetData->setItem(i, 0, new QStandardItem(QString("%1").arg(iter->first.c_str())));
            string OutStr;
            for(auto iterSet =iter->second.begin();iterSet != iter->second.end();iterSet++)
            {
                OutStr+=" ";
                OutStr+=iterSet->str;
            }
            FirstSetData->setItem(i, 1, new QStandardItem(QString("%1").arg(OutStr.c_str())));
        }
        FirstSetResult->setModel(FirstSetData);
        HasFirstSet = 1;
    }
}

void MainWindow::ShowLexicalRes()
{
    //注意要把其他的都关上
    FirstSetWindow->hide();
    DFASetWindow->hide();
    TablesSetWindow->hide();
    LexicalWindow->show();
    SematicBt->show();
    IsSematicTest->hide();
    ProcessSetWindow->hide();
    MidCodeSetWindow->hide();
}

void MainWindow::AnalysisGrammar()
{
    HasFirstSet = 0;
    FirstSetData->clear();
    HasDFASet = 0;
    DFASetData->clear();
    HasProcessSet = 0;
    ProcessSetData->clear();
    HasTablesSet = 0;
    TablesSetData->clear();
    HasMiddleCodeDis = 0;
    MidCodeSetData->clear();
    if(SematicBt->isChecked())
        IsSematic = true;
    else
        IsSematic = false;
    g = new GrammaAnalysis(MyLexical, GrammaFile, OutputTable, OutputTree, OutputTreeDot,IsSematic);
    int IsError = g->Forward(ProcessSetData);
    ofstream VarOutFile(VarOut,ios::out);
    if(!VarOutFile.is_open())
    {
        cout<<"open "<<VarOut<<" failed!!"<<endl;
    }
    ofstream FuncOutFile(FuncOut,ios::out);
    if(!FuncOutFile.is_open())
    {
        cout<<"open "<<FuncOut<<" failed!!"<<endl;
    }
    ofstream CodeOutFile(CodeOut,ios::out);
    if(!CodeOutFile.is_open())
    {
        cout<<"open "<<CodeOut<<" failed!!"<<endl;
    }
    VarOutFile<<g->VarTable<<endl;
    FuncOutFile<<g->FuncTable<<endl;
    CodeOutFile<<g->MiddleCodeTable<<endl;
    VarOutFile.close();
    FuncOutFile.close();
    CodeOutFile.close();
    if(IsError == -1)
        return;
    string cmd = GraphExe;
    cmd+=  " -Tpng ";
    cmd += g->GraphOutDot;
    cmd += " -o ";
    cmd += OutputTree;
    cout<<cmd<<endl;
    int j=system(cmd.c_str());
    cout<<"sys"<<j<<endl;
    if(IsSematic)
        QMessageBox::information(NULL, "提交成功","语句通过语法与语义分析检测",
                                 QMessageBox::Yes, QMessageBox::Yes);
    else
        QMessageBox::information(NULL, "提交成功","语句通过语法分析检测",
                                 QMessageBox::Yes, QMessageBox::Yes);
    LexicalWindow->hide();
    SematicBt->hide();
    if(IsSematic)
        IsSematicTest->setText("已进行语义分析");
    else
        IsSematicTest->setText("未进行语义分析");

    Optimizer temp_Optim(g->VarTable,g->FuncTable,g->MiddleCodeTable);

    IsSematicTest->show();
    ToFirstSet->show();
    HasFirstSet = 0;
    ToDFASet->show();
    HasDFASet = 0;
    ToProcessSet->show();
    HasProcessSet = 0;
    ToTablesSet->show();
    HasTablesSet = 0;
    OutputGraph->show();
    HasMiddleCodeDis = 0;
    if(IsSematic)
        ToMidCodeSet->show();
    else
        ToMidCodeSet->hide();

}
void MainWindow::InfoDisplay()
{
    QMessageBox::information(NULL, "软件信息","作者很懒，什么都还没写呢。",
                             QMessageBox::Yes, QMessageBox::Yes);
}
void MainWindow::OperationInfo()
{
    QMessageBox::information(NULL, "操作信息","作者很懒，什么都还没写呢。",
                             QMessageBox::Yes, QMessageBox::Yes);
}
void MainWindow::AutherInfo()
{
    int WindowHeight = 200;
    int WindowWidth = 300;
    QWidget * TempWindow = new QWidget(NULL);
    TempWindow->resize(WindowWidth,WindowHeight);
    TempWindow->move((deskdop->width()-TempWindow->width())/2, (deskdop->height()-TempWindow->height())/2);
    TempWindow->setWindowTitle("作者信息");
    QLabel* InfoDis = new QLabel(TempWindow);
    InfoDis->resize(110,100);
    InfoDis->move((TempWindow->width() - 110)/2,(TempWindow->height() - 100)/2);
    InfoDis->setText("1952735 游康\n1851964 武澳奇\n出品。\nkwd qaw qaq");
    InfoDis->setStyleSheet("font-size:16px;text-align:center;");
    TempWindow->show();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::AnalysisLexical()
{
    if(WordInput->toPlainText().size()!=0)
    {
        MyLexical = new lexical_analysis(WordInput->toPlainText().toStdString());
    }
    else if(WordWhere == 2)
        MyLexical = new lexical_analysis(LexicalFile,LexicalOutput);
    else
    {
        QMessageBox::critical(NULL, "错误", "没有输入语句", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(MyLexical->is_error()){
        QMessageBox::critical(NULL, "错误", MyLexical->error_string().c_str(), QMessageBox::Yes, QMessageBox::Yes);
        return ;
    }
    LexicalData->clear();
    for(int i=0;i<(int)MyLexical->token_vec.size();i++)
    {
        LexicalData->setItem(i, 0, new QStandardItem(QString("%1").arg(MyLexical->token_vec[i].word.c_str())));
        LexicalData->setItem(i, 1, new QStandardItem(QString("%1").arg(int2class[MyLexical->token_vec[i].type])));
    }
    LexicalResult->setModel(LexicalData);
    QMessageBox::information(NULL, "提示", "词法分析成功，可以进行语法分析了", QMessageBox::Yes, QMessageBox::Yes);
    SematicBt->show();
    GrammaBegin->show();
    LexicalResBt->show();
}

void MainWindow::FindInputWord()
{
    GetInputWordFile = new QFileDialog(this);//待检测文件输入
    GetInputWordFile->setGeometry(300,300,800,600);
    GetInputWordFile->setWindowTitle(tr("请选择输入语句文件"));
    GetInputWordFile->setDirectory("./");
    GetInputWordFile->show();
    if(GetInputWordFile->exec() == QDialog::Accepted) {
        QString path = GetInputWordFile->selectedFiles()[0];
        LexicalFile = path.toStdString();
        if(WordWhere != 1)
            WordWhere = 2;
    }
}



