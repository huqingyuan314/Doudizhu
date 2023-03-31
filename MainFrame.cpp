#pragma execution_character_set("utf-8") //防止出现中文乱码
#include "MainFrame.h"
#include "ui_MainFrame.h" //由MainFrame.ui转换而成的头文件

MainFrame::MainFrame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainFrame) //通过参数列表的方式给ui对象指针动态分配内存
{
    ui->setupUi(this); //setupUi()函数在ui_MainFrame.h中定义，this指向main()的w的地址

    //ui->pushButton->setText("--开始游戏--"); //修改pushButton的内容

    m_gamePanel = new GamePanel(this);

    this->resize(1200, 800); //设置主窗口的初始大小
    setWindowTitle(tr("JOJO斗地主")); //设置主窗口标题
}

MainFrame::~MainFrame() { delete ui; }

void MainFrame::resizeEvent(QResizeEvent* event) //主窗口响应函数
{
    m_gamePanel->setGeometry(rect()); //rect()调用setFixedSize()中的长和宽
    QDialog::resizeEvent(event); //窗口大小改变时自动调用，保持控件在主窗口居中
}
