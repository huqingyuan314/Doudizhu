#ifndef BGMCONTROL_H
#define BGMCONTROL_H

#include "Cards.h"

#include <QObject>
#include <QSound>
#include <QtMultimedia>

class BGMControl : public QObject
{
    Q_OBJECT
public:
    BGMControl();
    void play();
private:
    QMediaPlayer *player;
};


#endif // BGMCONTROL_H
