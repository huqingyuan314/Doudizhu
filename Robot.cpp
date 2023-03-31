#include "Robot.h"
#include "CallLordThread.h"
#include "PlayHandThread.h"
#include "Strategy.h"

Robot::Robot(QObject *parent)
{
    m_type = Player::Robot;
}

void Robot::StartCallLord()//开始叫地主
{
    CallLordThread* thread = new CallLordThread(this);
    thread->start();
}

void Robot::StartPlayHand()    //开始出牌
{
    PlayHandThread* thread = new PlayHandThread(this);
    thread->start();
}

void Robot::ThinkForCallLord()    //计算机器人叫地主叫分情况
{
    int weight = 0;

    Strategy st(this, m_cards);
    weight += st.GetRangeCards(Card_SJ, Card_BJ).Count() * 6;
    weight += m_cards.PointCount(Card_2) * 3;

    QVector<Cards> optSeq = st.PickOptimalSeqSingles();
    weight += optSeq.size() * 5;

    Cards left = m_cards;
    left.Remove(optSeq);
    Strategy stLeft(this, left);

    QVector<Cards> bombs = stLeft.FindCardsByCount(4);
    weight += bombs.size() * 5;

    QVector<Cards> triples = stLeft.FindCardsByCount(3);
    weight += triples.size() * 4;

    QVector<Cards> pairs = stLeft.FindCardsByCount(2);
    weight += pairs.size() * 1;

    if (weight >= 22)                            //叫3分
    {
        CallLord(3);
    }
    else if (weight < 22 && weight >= 18)          //叫2分
    {
        CallLord(2);
    }
    else if (weight > 18 && weight >= 10)        //叫1分
    {
        CallLord(1);
    }
    else                                       //叫0分
    {
        CallLord(0);
    }
}

void Robot::ThinkForPlayHand()          //  出牌
{
    Strategy strategy(this, m_cards);
    PlayHand(strategy.MakeStrategy());
}

