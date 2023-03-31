/*
 *<计分板类>
 *Qt特性的详细注释与MainFrame.h类似
 */

#ifndef MARKBOARD_H
#define MARKBOARD_H

#include <QWidget>

namespace Ui {
class MarkBoard;
}

class MarkBoard : public QWidget
{
    Q_OBJECT

public:
    explicit MarkBoard(QWidget *parent = 0);
    ~MarkBoard();

    void SetMarks(int me, int left, int right); //分数设置函数
private:
    Ui::MarkBoard *ui;
};

#endif // MARKBOARD_H


/*
 *<bug>
 *分数面板全为0时，不发生显示不全
 *分数面板为其他数值时，部分显示不全
 *
 *在系统分辨率较低时，开局分数面板就显示不全
 */
