#ifndef ROBOT_H
#define ROBOT_H

#include "Player.h"

class Robot : public Player          //继承player
{
    Q_OBJECT
public:
    Robot(QObject* parent = NULL);

public:
    virtual void StartCallLord();    //  叫地主和出牌
    virtual void StartPlayHand();

    virtual void ThinkForCallLord();
    virtual void ThinkForPlayHand();
};

#endif // ROBOT_H
