#include "CallLordThread.h"
#include "Player.h"

CallLordThread::CallLordThread(Player* player) :  //线程为play
    QThread(player)
{
    m_player = player;
}

void CallLordThread::run()         //叫地主线程开始
{
    msleep(600);                  //延时600毫秒
    m_player->ThinkForCallLord();
}
