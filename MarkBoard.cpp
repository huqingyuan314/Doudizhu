#include "MarkBoard.h"
#include "ui_MarkBoard.h"

MarkBoard::MarkBoard(QWidget *par) : //构造函数
    QWidget(par),
    ui(new Ui::MarkBoard)
{
    ui->setupUi(this);
}

MarkBoard::~MarkBoard() //析构函数
{
    delete ui;
}

void MarkBoard::SetMarks(int me, int left, int right) //设置分数
{
    QString str = tr("%1 point(s)"); //第1个arg函数参数变量转换后的字符串填充到“%1”位置

    ui->myMark->setText(str.arg(me));
    ui->leftMark->setText(str.arg(left));
    ui->rightMark->setText(str.arg(right));
}
