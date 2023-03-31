#ifndef CALLLORDTHREAD_H
#define CALLLORDTHREAD_H

#include <QThread>  //   QThread类提供了一个与平台无关的管理线程的方法

class Player;

class CallLordThread : public QThread
{
    Q_OBJECT
public:
    explicit CallLordThread(Player* player);   //指定构造函数或转换函数为显式, 即它不能用于隐式转换和复制初始化.

protected:
    virtual void run();

private:
    Player* m_player;
};

#endif // CALLLORDTHREAD_H
