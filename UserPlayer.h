#ifndef USERPLAYER_H
#define USERPLAYER_H

#include "Player.h"

class UserPlayer : public Player    //继承player
{
    Q_OBJECT
public:
    UserPlayer(QObject* parent = NULL);

public:
    virtual void StartCallLord();    //  叫地主和出牌
    virtual void StartPlayHand();
};

#endif // USERPLAYER_H
