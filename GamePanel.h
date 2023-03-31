/*
 *<游戏面板类>
 *Qt特性的详细注释与MainFrame.h类似
 *
 *注意称呼的区别：用户-User-操作电脑的人；玩家-Player-所有三个参与者
 *处理：GamePanel对相应操作的响应
 *基牌：发牌时正中间的牌
 */

#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QWidget>
#include <QResizeEvent>
#include <QMap>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include "GameControl.h"
#include "CardPic.h"
#include "MarkBoard.h"

class GamePanel : public QWidget
{
    Q_OBJECT

public:
    explicit GamePanel(QWidget *parent = 0); //构造函数

public slots: //信号槽
    //1.用户按钮的响应
    void OnStartBtnClicked(); //按下“开始游戏”按钮时
    void OnContinueBtnClicked(); //按下“再来一局”按钮时

    void OnUserNoBet(); //按下“不叫”按钮时
    void OnUserBet1(); //按下“一分”按钮时
    void OnUserBet2(); //按下“两分”按钮时
    void OnUserBet3(); //按下“三分”按钮时

    void OnUserPass(); //按下“不要”按钮时
    void OnUserPlayHand(); //按下“出牌”按钮时

    //2.“发牌”阶段的响应
    void StartPickCard(); //“发牌”操作
    void ShowPickingCardStep(Player* player, int step); //发牌时移动一格的动画效果
    void OnCardPicking(); //发牌时动画效果

    //3.“选牌”阶段的响应
    void OnCardPicSelected(Qt::MouseButton mouseButton); //选牌时的处理

    //4.玩家操作的响应
    void OnPlayerCalledLord(Player* player, int bet); //玩家叫分处理
    void OnPlayerPlayHand(Player* player, const Cards& cards); //玩家出牌处理
    void OnPlayerPickCards(Player* player, const Cards& cards); //玩家选牌处理

    void HidePlayerLastCards(Player* player); //隐藏玩家上一次出的牌
    void UpdatePlayerCards(Player* player); //更新玩家卡牌

    //5.不同状态的响应
    void OnPlayerStatus(Player* player, GameControl::PlayerStatus playerStatus); //玩家状态的面板处理
    void ProcessGameStatus(GameControl::GameStatus gameStatus); //游戏状态面板处理

protected:
    virtual void paintEvent(QPaintEvent* event); //设置面板背景图片
    virtual void resizeEvent(QResizeEvent* event); //设置大小与位置

    void InitCardPicMap(); //初始化卡牌图片
    void CutCardPic(const QPixmap& cardsPic, int x, int y, Card card); //在卡片图左上角为(x, y)处抠下指定尺寸，与card对应起来
    void InitControls(); //初始化流程控制
    void InitPlayerContext(); //初始化玩家背景

    void UpdateMarkBoard(); //更新分数面板


protected:
    enum CardsAlign //卡牌排列方向
    {
        Horizontal, //横向
        Vertical //纵向
    };

    struct PlayerContext //玩家背景属性
    {
        QRect cardsRect; //卡牌的矩形框
        QRect playHandRect; //手牌的矩形框
        CardsAlign cardsAlign; //卡牌排列方向
        bool isFrontSide; //牌正反面
        Cards lastCards; //上一次出的牌
        QLabel* info; //玩家文字提示信息
        QLabel* rolePic; //玩家图片
    };

    QMap<Player*, PlayerContext> m_playerContextMap; //玩家背景属性匹配


private:
    GameControl* m_gameControl; //游戏控制类对象m_gameControl

    //发牌动画时
    CardPic* m_baseCard; //基牌图片
    CardPic* m_movingCard; //移动牌图片

    QVector<CardPic*> m_restThreeCards; //最后3张底牌

    QPixmap m_cardBackPic; //牌背面的图像

    QSize m_cardSize; //卡牌大小
    QPoint m_baseCardPos; //基牌位置坐标

    QMap<Card, CardPic*> m_cardPicMap; //每张卡片对应一个图片
    QSet<CardPic*> m_selectedCards;	 //用户出牌前选择的牌图片

    GameControl::GameStatus m_gameStatus; //游戏状态

    QTimer* m_pickCardTimer; //发牌计时器
    int m_curMoveStep; //移动一步的距离

    //界面和按钮
    QFrame* m_userTool; //用户按钮矩形框
    QPushButton* m_startButton; //“开始游戏”按钮
    QPushButton* m_continueButton; //“再来一局”按钮
    QPushButton* m_playHandButton; //“出牌”按钮
    QPushButton* m_passButton; //“不要”按钮
    QPushButton* m_bet0Button; //“不叫”按钮
    QPushButton* m_bet1Button; //“一分”按钮
    QPushButton* m_bet2Button; //“两分”按钮
    QPushButton* m_bet3Button; //“三分”按钮

    MarkBoard* m_markBoard; //计分板
};

#endif // GAMEPANEL_H
