#include "bgmcontrol.h"
#include "Cards.h"

BGMControl::BGMControl()
{
}

void BGMControl::play() //播放背景音乐（但没声音，还在调试中）
{
    player = new QMediaPlayer(this);
    player->setMedia(QUrl("qrc:///res/landlord_main_bgm.mp3"));
    player->setVolume(80);//音量
    player->play();
}
