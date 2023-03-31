/*
 *<主窗口类>
 */

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QDialog>
#include "GamePanel.h"
#include <QResizeEvent>

//Pimpl手法：解开类的使用接口和实现的耦合，减少各个源文件之间的联系（其他Ui模块类同理）
//namespace为自动生成：提前声明告诉编译器，此namespace在别处有定义
namespace Ui {
class MainFrame;
}

//ui_MainFrame.h中表明：MainFrame类公有继承自Ui_MainFrame类
class MainFrame : public QDialog
{
    Q_OBJECT //有关信号槽机制的宏声明

public:
    explicit MainFrame(QWidget *parent = 0);  //explicit说明符：指定构造函数为显式（即它不能用于隐式转换和复制初始化）
    ~MainFrame();

protected:
    virtual void resizeEvent(QResizeEvent* event); //主窗口响应函数

private:
    Ui::MainFrame *ui; //ui对象指针，所属类为Ui命名空间中的MainFrame
    GamePanel* m_gamePanel;
};

#endif // MAINFRAME_H
