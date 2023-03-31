#include "GamePanel.h"
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QHBoxLayout>
#include "Hand.h"
#include "Strategy.h"

//构造函数
GamePanel::GamePanel(QWidget *parent) :
    QWidget(parent)
{
    m_gameControl = new GameControl(this); //创建游戏控制对象m_gameControl
    m_gameControl->Init(); //初始化m_gameControl

    m_cardSize = QSize(80, 105); //指定卡牌大小

    m_markBoard = new MarkBoard(this); //创建分数面板对象m_markBoard
    m_markBoard->show(); //显示m_markBoard
    UpdateMarkBoard(); //更新m_markBoard

    //初始化
    InitCardPicMap(); //初始化卡牌图片
    InitControls(); //初始化流程控制
    InitPlayerContext(); //初始化玩家背景

    //建立GameControl(信号)与GamePanel(槽)的连接
    connect(m_gameControl, &GameControl::NotifyPlayerCalledLord, this, &GamePanel::OnPlayerCalledLord);
    connect(m_gameControl, &GameControl::NotifyPlayerPlayHand, this, &GamePanel::OnPlayerPlayHand);
    connect(m_gameControl, &GameControl::NotityPlayerPickCards, this, &GamePanel::OnPlayerPickCards);
    connect(m_gameControl, &GameControl::NotifyGameStatus, this, &GamePanel::ProcessGameStatus);
    connect(m_gameControl, &GameControl::NotifyPlayerStatus, this, &GamePanel::OnPlayerStatus);

    m_baseCard = new CardPic(this); //创建基牌
    m_movingCard = new CardPic(this); //创建移动牌
    m_baseCard->SetPic(m_cardBackPic, m_cardBackPic); //设置基牌图片
    m_movingCard->SetPic(m_cardBackPic, m_cardBackPic); //设置移动牌图片

    //最后3张底牌，依次加入m_restThreeCards向量容器
    CardPic* restCardPic = new CardPic(this);
    m_restThreeCards << restCardPic;
    restCardPic = new CardPic(this);
    m_restThreeCards << restCardPic;
    restCardPic = new CardPic(this);
    m_restThreeCards << restCardPic;

    //发牌效果所用的计时器
    m_curMoveStep = 0; //初始化移动距离为0
    m_pickCardTimer = new QTimer(this); //创建发牌计时器
    m_pickCardTimer->setInterval(10); //每次移动时间间隔(毫秒)
    connect(m_pickCardTimer, &QTimer::timeout, this, &GamePanel::OnCardPicking); //将计时器和OnCardPicking()连接
}

void GamePanel::InitPlayerContext() //初始化玩家背景
{
    PlayerContext context;

    //左机器人
    context.cardsAlign = Vertical; //卡牌垂直排列
    context.isFrontSide = false; //背面朝上
    m_playerContextMap.insert(m_gameControl->GetLeftRobot(), context); //建立关联

    //右机器人
    context.cardsAlign = Vertical; //卡牌垂直排列
    context.isFrontSide = false; //背面朝上
    m_playerContextMap.insert(m_gameControl->GetRightRobot(), context); //建立关联

    //用户
    context.cardsAlign = Horizontal; //卡牌水平排列
    context.isFrontSide = true; //正面朝上
    m_playerContextMap.insert(m_gameControl->GetUserPlayer(), context); //建立关联

    QMap<Player*, PlayerContext>::Iterator it = m_playerContextMap.begin(); //玩家背景关联迭代器it
    for (; it != m_playerContextMap.end(); it++)
    {
        //初始化info
        it->info = new QLabel(this);
        it->info->resize(100, 50);
        it->info->setObjectName("info");
        it->info->hide();
        //初始化rolePic
        it->rolePic = new QLabel(this);
        it->rolePic->resize(84, 120);
        it->rolePic->hide();
    }
}

void GamePanel::InitControls() //初始化流程控制
{
    m_userTool = new QFrame(this);
    QHBoxLayout* horLayout = new QHBoxLayout(m_userTool); //QHBoxLayout：水平按钮布局
    horLayout->setSpacing(10); //水平按钮间距
    horLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum)); //QSpacerItem：空白区

    //“开始游戏”按钮
    m_startButton = new QPushButton(m_userTool); //新建用户按钮矩形框
    m_startButton->setObjectName("button");
    m_startButton->setFocusPolicy(Qt::NoFocus); //无焦点事件
    m_startButton->setText(tr("Start Game")); //文字标签
    m_startButton->show(); //默认显示按钮
    m_startButton->setFixedSize(89, 32); //按钮大小
    horLayout->addWidget(m_startButton); //加入水平按钮布局
    connect(m_startButton, &QPushButton::clicked, this, &GamePanel::OnStartBtnClicked); //建立点击与响应的关联

    //“再来一局”按钮
    m_continueButton = new QPushButton(m_userTool);
    m_continueButton->setObjectName("button");
    m_continueButton->setFocusPolicy(Qt::NoFocus);
    m_continueButton->setText(tr("Continue Game"));
    m_continueButton->hide();
    m_continueButton->setFixedSize(89, 32);
    horLayout->addWidget(m_continueButton);
    connect(m_continueButton, &QPushButton::clicked, this, &GamePanel::OnContinueBtnClicked);

    //“不要”按钮
    m_passButton = new QPushButton(m_userTool);
    m_passButton->setObjectName("button");
    m_passButton->setFocusPolicy(Qt::NoFocus);
    m_passButton->setText(tr("btn_pass"));
    m_passButton->hide();
    m_passButton->setFixedSize(89, 32);
    horLayout->addWidget(m_passButton);
    connect(m_passButton, &QPushButton::clicked, this, &GamePanel::OnUserPass);

    //“出牌”按钮
    m_playHandButton = new QPushButton(m_userTool);
    m_playHandButton->setObjectName("button");
    m_playHandButton->setFocusPolicy(Qt::NoFocus);
    m_playHandButton->setText(tr("play hand"));
    m_playHandButton->hide();
    m_playHandButton->setFixedSize(89, 32);
    QFont font = m_playHandButton->font(); //创建字体对象
    font.setBold(true); //加粗
    font.setPixelSize(20); //字号20
    m_playHandButton->setFont(font); //对“出牌”按钮设置字体
    horLayout->addWidget(m_playHandButton);
    connect(m_playHandButton, &QPushButton::clicked, this, &GamePanel::OnUserPlayHand);

    //“不叫”按钮
    m_bet0Button = new QPushButton(m_userTool);
    m_bet0Button->setObjectName("button");
    m_bet0Button->setFocusPolicy(Qt::NoFocus);
    m_bet0Button->setText(tr("bet0"));
    m_bet0Button->hide();
    m_bet0Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet0Button);
    connect(m_bet0Button, &QPushButton::clicked, this, &GamePanel::OnUserNoBet);

    //“一分”按钮
    m_bet1Button = new QPushButton(m_userTool);
    m_bet1Button->setObjectName("button");
    m_bet1Button->setFocusPolicy(Qt::NoFocus);
    m_bet1Button->setText(tr("bet1"));
    m_bet1Button->hide();
    m_bet1Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet1Button);
    connect(m_bet1Button, &QPushButton::clicked, this, &GamePanel::OnUserBet1);

    //“两分”按钮
    m_bet2Button = new QPushButton(m_userTool);
    m_bet2Button->setObjectName("button");
    m_bet2Button->setFocusPolicy(Qt::NoFocus);
    m_bet2Button->setText(tr("bet2"));
    m_bet2Button->hide();
    m_bet2Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet2Button);
    connect(m_bet2Button, &QPushButton::clicked, this, &GamePanel::OnUserBet2);

    //“三分”按钮
    m_bet3Button = new QPushButton(m_userTool);
    m_bet3Button->setObjectName("button");
    m_bet3Button->setFocusPolicy(Qt::NoFocus);
    m_bet3Button->setText(tr("bet3"));
    m_bet3Button->hide();
    m_bet3Button->setFixedSize(89, 32);
    horLayout->addWidget(m_bet3Button);
    connect(m_bet3Button, &QPushButton::clicked, this, &GamePanel::OnUserBet3);

    horLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

void GamePanel::CutCardPic(const QPixmap& cardsPic,int x, int y, Card card) //在卡片图左上角为(x, y)处抠下指定尺寸，与card对应起来
{
    QPixmap pic = cardsPic.copy(x, y, m_cardSize.width(), m_cardSize.height()); //将cardsPic的指定矩形框复制给pic

    CardPic* cardPic = new CardPic(this); //创建cardPic对象
    cardPic->hide();
    cardPic->SetPic(pic, m_cardBackPic); //设置cardPic正反面图像
    cardPic->SetCard(card); //设置cardPic关联的卡牌对象

    //添加关联
    m_cardPicMap.insert(card, cardPic);
    connect(cardPic, &CardPic::NotifySelected, this, &GamePanel::OnCardPicSelected);
}

void GamePanel::InitCardPicMap() //初始化卡牌图片
{
    QPixmap cardsPic(":/res/card.png"); //卡牌图片文件

    //卡牌背面图片：（第5行，第3列）
    m_cardBackPic = cardsPic.copy(2 * m_cardSize.width(), 4 * m_cardSize.height(),
        m_cardSize.width(), m_cardSize.height());

    //4种花色的卡牌（52张）图片
    for (int suit = Suit_Begin + 1, i = 0; suit < Suit_End; suit++, i++)
    {
        for (int pt = Card_Begin + 1, j = 0; pt < Card_SJ; pt++, j++)
        {
            Card card; //创建卡牌对象card
            card.point = (CardPoint)pt; //点数赋值
            card.suit = (CardSuit)suit; //花色赋值

            CutCardPic(cardsPic, j * m_cardSize.width(), i * m_cardSize.height(), card); //扣下卡牌图片
        }
    }

    //大小王较特殊，分别单独处理
    Card card;
    //小王
    card.point = Card_SJ;
    card.suit = Suit_Begin;
    CutCardPic(cardsPic, 0, 4 * m_cardSize.height(), card);
    //大王
    card.point = Card_BJ;
    card.suit = Suit_Begin;
    CutCardPic(cardsPic, m_cardSize.width(), 4 * m_cardSize.height(), card);
}

void GamePanel::paintEvent(QPaintEvent* event) //设置面板背景图片
{
    static QPixmap bk(":/res/table.png");
    QPainter painter(this);
    painter.drawPixmap(rect(), bk);
}

void GamePanel::resizeEvent(QResizeEvent* event) //设置大小与位置
{
    QWidget::resizeEvent(event);

    m_markBoard->setGeometry(rect().right() - 150, 0, 130, 80); //设置记分板位置与大小

    m_baseCardPos = QPoint((width() - m_cardSize.width()) / 2, height() / 2 - 100); //设置基牌坐标
    m_baseCard->move(m_baseCardPos); //将基牌移到坐标处
    m_movingCard->move(m_baseCardPos); //将基牌图片移到坐标处

    QMap<Player*, PlayerContext>::Iterator it; //玩家背景属性迭代器it

    //左机器人
    it = m_playerContextMap.find(m_gameControl->GetLeftRobot()); //匹配玩家背景属性
    it->cardsRect = QRect(10, 180, 100, height() - 200); //卡牌矩形框位置（左，上，宽度，高度）
    it->playHandRect = QRect(180, 150, 100, 100); //手牌矩形框位置
    it->rolePic->move(it->cardsRect.left(), it->cardsRect.top() - 100); //玩家图片矩形框位置

    //右机器人
    it = m_playerContextMap.find(m_gameControl->GetRightRobot());
    it->cardsRect = QRect(rect().right() - 110, 180, 100, height() - 200);
    it->playHandRect = QRect(rect().right() - 280, 150, 100, 100);
    it->rolePic->move(it->cardsRect.left(), it->cardsRect.top() - 100);

    //用户
    it = m_playerContextMap.find(m_gameControl->GetUserPlayer());
    it->cardsRect = QRect(250, rect().bottom() - 120, width() - 500, 100);
    it->playHandRect = QRect(150, rect().bottom() - 280, width() - 300, 100);
    it->rolePic->move(it->cardsRect.right() - 50, it->cardsRect.top() - 10);

    //设置用户按钮矩形框
    QRect rectUserTool(it->playHandRect.left(), it->playHandRect.bottom(),
        it->playHandRect.width(), it->cardsRect.top() - it->playHandRect.bottom());
    m_userTool->setGeometry(rectUserTool);

    //设置文字提示信息
    for (it = m_playerContextMap.begin(); it != m_playerContextMap.end(); it++) //对所有玩家
    {
        QRect playCardsRect = it->playHandRect;
        QLabel* infoLabel = it->info; //创建文字标签infoLabel

        QPoint pt( playCardsRect.left() + (playCardsRect.width() - infoLabel->width()) / 2,
            playCardsRect.top() + (playCardsRect.height() - infoLabel->height()) / 2 ); //根据手牌位置设置pt
        infoLabel->move(pt); //将infoLabel设置到pt位置
    }

    //显示剩下的三张底牌
    int base = (width() - 3 * m_cardSize.width() - 2 * 10) / 2;
    for (int i = 0; i < m_restThreeCards.size(); i++)
        m_restThreeCards[i]->move(base + (m_cardSize.width() + 10) * i, 20); //设置底牌位置

    //更新玩家卡牌
    UpdatePlayerCards(m_gameControl->GetUserPlayer());
    UpdatePlayerCards(m_gameControl->GetLeftRobot());
    UpdatePlayerCards(m_gameControl->GetRightRobot());
}

void GamePanel::UpdateMarkBoard() //更新分数面板
{
    m_markBoard->SetMarks(m_gameControl->GetUserPlayer()->GetMark(),
                          m_gameControl->GetLeftRobot()->GetMark(),
                          m_gameControl->GetRightRobot()->GetMark());
}

void GamePanel::OnCardPicking() //发牌时动画效果
{
    Player* curPlayer = m_gameControl->GetCurrentPlayer(); //设置当前给发牌的玩家

    if (m_curMoveStep >= 100) //若牌已移动到面板末端（上一张牌发牌结束）
    {
        Card card = m_gameControl->PickOneCard(); //随机选一张卡牌
        curPlayer->PickCard(card); //当前玩家选中此牌
        m_gameControl->SetCurrentPlayer(curPlayer->GetNextPlayer()); //将下一玩家置为当前玩家（给下一玩家发牌）
        m_curMoveStep = 0; //初始化移动距离

        if (m_gameControl->GetRestCards().Count() == 3) //还剩下3张牌，摸牌结束
        {
            m_pickCardTimer->stop(); //停止发牌计时器
            ProcessGameStatus(GameControl::CallingLordStatus); //切换到“叫地主”状态
        }
    }

    ShowPickingCardStep(curPlayer, m_curMoveStep); //牌移动一格
    m_curMoveStep += 10; //卡牌移动距离增加10
}

void GamePanel::ShowPickingCardStep(Player* player, int step) //发牌时移动一格的动画效果
{
    QRect cardsRect = m_playerContextMap[player].cardsRect; //创建玩家卡牌矩形框

    if (player == m_gameControl->GetLeftRobot()) //若发牌给左机器人
    {
        int unit = (m_baseCardPos.x() - cardsRect.right()) / 100; //单位移动距离
        m_movingCard->move(m_baseCardPos.x() - step * unit, m_baseCardPos.y()); //移动牌图片的移动
    }
    else if (player == m_gameControl->GetRightRobot()) //若发牌给右机器人
    {
        int unit = (cardsRect.left() - m_baseCardPos.x()) / 100;
        m_movingCard->move(m_baseCardPos.x() + step * unit, m_baseCardPos.y());
    }
    else if (player == m_gameControl->GetUserPlayer()) //若发牌给用户
    {
        int unit = (cardsRect.top() - m_baseCardPos.y()) / 100;
        m_movingCard->move(m_baseCardPos.x(), m_baseCardPos.y() + step * unit);
    }

    if (step == 0) //牌未开始移动时，显示移动牌
        m_movingCard->show();
    if (step >= 100) //牌已移动到面板末端时，隐藏移动牌
        m_movingCard->hide();

    update(); //更新画面
}

void GamePanel::OnCardPicSelected(Qt::MouseButton mouseButton) //选牌时的处理
{
    if (m_gameStatus == GameControl::PickingCardStatus) //如果在发牌阶段，则不执行
        return ;

    CardPic* cardPic = (CardPic*)sender(); //将信号发送给cardPic
    if (cardPic->GetOwner() != m_gameControl->GetUserPlayer()) //如果选牌玩家不是用户，则不执行
        return ;

    if (mouseButton == Qt::LeftButton) //点击鼠标左键
    {
        cardPic->SetSelected(!cardPic->IsSelected()); //将cardPic置为“未选中”
        UpdatePlayerCards(cardPic->GetOwner()); //更新玩家卡牌

        QSet<CardPic*>::Iterator it = m_selectedCards.find(cardPic); //迭代器it在m_selectedCards中依次寻找cardPic
        if (it == m_selectedCards.end()) //若未找到
            m_selectedCards.insert(cardPic); //将cardPic加入
        else //若找到
            m_selectedCards.erase(it); //取消选中牌
    }
    else if (mouseButton == Qt::RightButton) //右击出牌
        OnUserPlayHand();
}


void GamePanel::HidePlayerLastCards(Player* player) //隐藏玩家上一次出的牌
{
    QMap<Player*, PlayerContext>::Iterator it = m_playerContextMap.find(player);
    if (it != m_playerContextMap.end())
    {
        if (it->lastCards.IsEmpty())	//若上一次出的空牌（“不要”）
            it->info->hide(); //隐藏玩家文字提示信息

        else //若上一次出的牌非空
        {
            CardList lastCardList = it->lastCards.ToCardList(); //上一次出的牌加入lastCardList
            CardList::ConstIterator itLast = lastCardList.constBegin();
            for (; itLast != lastCardList.constEnd(); itLast++) //将lastCardList中的牌依次隐藏
                m_cardPicMap[*itLast]->hide();
        }
    }
}

void GamePanel::UpdatePlayerCards(Player* player) //更新玩家卡牌
{
    Cards restCards = player->GetCards(); //获取玩家剩余卡牌
    CardList restCardList = restCards.ToCardList(Cards::Desc); //将剩余卡牌降序（左大右小）置入restCardList
    const int cardSpacing = 25; //玩家卡牌图片之间的间隔

    // 显示剩下的牌
    QRect cardsRect = m_playerContextMap[player].cardsRect; //创建卡牌矩形框cardsRect
    CardList::ConstIterator itRest = restCardList.constBegin();
    for (int i = 0; itRest != restCardList.constEnd(); itRest++, i++)
    {
        CardPic* cardPic = m_cardPicMap[*itRest];
        cardPic->SetFrontSide(m_playerContextMap[player].isFrontSide);
        cardPic->show();
        cardPic->raise(); //新增的图片置于前方

        if (m_playerContextMap[player].cardsAlign == Horizontal) //若卡牌水平排列
        {
            int leftBase = cardsRect.left() + (cardsRect.width() - (restCardList.size() - 1) * cardSpacing - cardPic->width()) / 2;
            int top = cardsRect.top() + (cardsRect.height() - cardPic->height()) / 2;
            if (cardPic->IsSelected())
                top -= 10;
            cardPic->move(leftBase + i * cardSpacing, top); //移动卡牌图片
        }
        else //若卡牌垂直排列
        {
            int left = cardsRect.left() + (cardsRect.width() - cardPic->width()) / 2;
            if (cardPic->IsSelected())
                left += 10;
            int topBase = cardsRect.top() + (cardsRect.height() - (restCardList.size() - 1) * cardSpacing - cardPic->height()) / 2;
            cardPic->move(left, topBase + i * cardSpacing);
        }
    }

    //显示上一次打出去的牌
    QRect playCardsRect = m_playerContextMap[player].playHandRect;
    if (!m_playerContextMap[player].lastCards.IsEmpty()) //若不是空牌
    {
        int playSpacing = 25; //打出去的牌图片之间间隔
        CardList lastCardList = m_playerContextMap[player].lastCards.ToCardList();
        CardList::ConstIterator itPlayed = lastCardList.constBegin();
        for (int i = 0; itPlayed != lastCardList.constEnd(); itPlayed++, i++)
        {
            CardPic* cardPic = m_cardPicMap[*itPlayed];
            cardPic->SetFrontSide(true);
            cardPic->raise();

            if (m_playerContextMap[player].cardsAlign == Horizontal) //若卡牌水平排列
            {
                int leftBase = playCardsRect.left () +
                    (playCardsRect.width() - (lastCardList.size() - 1) * playSpacing - cardPic->width()) / 2;
                int top = playCardsRect.top() + (playCardsRect.height() - cardPic->height()) / 2;
                cardPic->move(leftBase + i * playSpacing, top);
            }
            else //若卡牌垂直排列
            {
                int left = playCardsRect.left() + (playCardsRect.width() - cardPic->width()) / 2;
                int topBase = playCardsRect.top();
                cardPic->move(left, topBase + i * playSpacing);
            }
        }
    }
}

//玩家操作的响应
void GamePanel::OnPlayerCalledLord(Player* player, int bet) //玩家叫分处理
{
    QString betInfo;
    //设置betInfo的值
    if (bet == 0)
        betInfo = tr("bet0");
    else if (bet == 1)
        betInfo = tr("bet1");
    else if (bet == 2)
        betInfo = tr("bet2");
    else if (bet == 3)
        betInfo = tr("bet3");

    //infoLabel显示玩家叫分文字
    QLabel* infoLabel = m_playerContextMap[player].info;
    infoLabel->setText(betInfo);
    infoLabel->show();

    // 用户叫完分，隐藏叫分按钮
    if (player == m_gameControl->GetUserPlayer())
    {
        m_bet0Button->hide();
        m_bet1Button->hide();
        m_bet2Button->hide();
        m_bet3Button->hide();
    }
}

void GamePanel::OnPlayerPlayHand(Player* player, const Cards& cards) //玩家出牌处理
{
    HidePlayerLastCards(player); //隐藏玩家上一次出的牌

    //记录上一次打的牌
    QMap<Player*, PlayerContext>::Iterator itContext = m_playerContextMap.find(player);
    itContext->lastCards = cards;

    //若打空牌，则显示“不要”
    if (cards.IsEmpty())
    {
        itContext->info->setText(tr("label_pass"));
        itContext->info->show();
    }

    UpdatePlayerCards(player); //更新玩家卡牌
}

void GamePanel::OnPlayerPickCards(Player* player, const Cards& cards) //玩家选牌处理
{
    CardList cardList = cards.ToCardList(); //将选中的牌加入cardList
    CardList::ConstIterator it = cardList.constBegin();
    for (; it != cardList.constEnd(); it++)
    {
        CardPic* pickedCard = m_cardPicMap[*it]; //将选中的卡牌对应到图片
        pickedCard->SetOwner(player); //卡牌图片关联到当前玩家
    }

    UpdatePlayerCards(player); //更新玩家卡牌
}


//用户按钮的响应的实现
void GamePanel::OnStartBtnClicked() //按下“开始游戏”按钮时
{
    m_startButton->hide(); //隐藏“开始游戏”按钮
    m_gameControl->ClearPlayerMarks(); //将分数面板初始化归零
    UpdateMarkBoard(); //更新分数面板
    ProcessGameStatus(GameControl::PickingCardStatus); //设置游戏状态为“发牌”
}

void GamePanel::OnContinueBtnClicked() //按下“再来一局”按钮时
{
    m_continueButton->hide(); //隐藏“再来一局”按钮
    ProcessGameStatus(GameControl::PickingCardStatus); //设置游戏状态为“发牌”
}

void GamePanel::OnUserNoBet() //按下“不叫”按钮时
{
    m_gameControl->GetUserPlayer()->CallLord(0);
}

void GamePanel::OnUserBet1() //按下“一分”按钮时
{
    m_gameControl->GetUserPlayer()->CallLord(1);
}

void GamePanel::OnUserBet2() //按下“两分”按钮时
{
    m_gameControl->GetUserPlayer()->CallLord(2);
}

void GamePanel::OnUserBet3() //按下“三分”按钮时
{
    m_gameControl->GetUserPlayer()->CallLord(3);
}

void GamePanel::OnUserPass() //按下“不要”按钮时
{
    //若当前玩家不是用户，则不执行
    if (m_gameControl->GetCurrentPlayer() != m_gameControl->GetUserPlayer())
        return ;
    Player* punchPlayer = m_gameControl->GetPunchPlayer(); //punchPlayer指针指向当前出有效牌的玩家
    //若前一次出有效牌的玩家是用户（即两个Robot都不要时），或者没有玩家出有效牌（首次出牌时），则不执行
    if (punchPlayer == m_gameControl->GetUserPlayer() || punchPlayer == NULL)
        return ;

    //否则
    m_gameControl->GetUserPlayer()->PlayHand(Cards()); //不移除用户手牌，控制权交给下一玩家

    //创建CardPic*类型的Set只读迭代器it，指向用户出牌前选择的牌图片的首位
    QSet<CardPic*>::ConstIterator it = m_selectedCards.constBegin();
    for (; it != m_selectedCards.constEnd(); it++) //遍历用户出牌前选择的牌图片
    {
        (*it)->SetSelected(false); //将选择的牌图片置为“未选择”状态（下降至原位）
    }
    m_selectedCards.clear(); //将选中的牌的Set清空

    UpdatePlayerCards(m_gameControl->GetUserPlayer()); //更新用户的卡牌
}

void GamePanel::OnUserPlayHand() //按下“出牌”按钮时
{
    if (m_gameStatus != GameControl::PlayingHandStatus) //若游戏状态不是“出牌”，则不执行
        return ;
    if (m_gameControl->GetCurrentPlayer() != m_gameControl->GetUserPlayer()) //若当前玩家不是用户，则不执行
        return ;
    if (m_selectedCards.isEmpty()) //若没有选中任何牌，则不执行
        return ;

    Cards playCards; //创建卡牌类对象playCards
    QSet<CardPic*>::const_iterator it = m_selectedCards.constBegin();
    for (; it != m_selectedCards.constEnd(); it++)
    {
        playCards.Add((*it)->GetCard()); //将选中的牌依次添加至playCards
    }

    Hand hand(playCards); //由playCards创建手牌类对象hand
    HandType ht = hand.GetType(); //获取牌型ht
    CardPoint bp = hand.GetBasePoint(); //获取基础点数bp
    int ex = hand.GetExtra(); //获取额外点数ex

    if (ht == Hand_Unknown)		//若是未知牌型，则不允许出
        return ;

    //若大不过其他人的牌，则不允许出
    if (m_gameControl->GetPunchPlayer() != m_gameControl->GetUserPlayer())
    {
        if (!hand.CanBeat(Hand(m_gameControl->GetPunchCards())))
            return ;
    }

    m_gameControl->GetUserPlayer()->PlayHand(playCards); //出playCards的牌
    m_selectedCards.clear(); //将选中的牌的set清空
}



void GamePanel::StartPickCard() //“发牌”操作
{
    //创建Map只读迭代器，元素为Card和CardPic*之间的映射
    QMap<Card, CardPic*>::const_iterator it = m_cardPicMap.constBegin();
    for (; it != m_cardPicMap.constEnd(); it++)
    {
        it.value()->SetSelected(false); //设置卡牌状态为“未选中”
        it.value()->SetFrontSide(true); //设置卡牌为“正面”
        it.value()->hide(); //隐藏已发出的卡牌
    }

    for (int i = 0; i < m_restThreeCards.size(); i++) //最后3张牌牌面隐藏
        m_restThreeCards[i]->hide();

    //初始化（重置）玩家背景
    m_selectedCards.clear(); //重置已选卡牌
    //重置最后卡牌
    m_playerContextMap[m_gameControl->GetLeftRobot()].lastCards.Clear();
    m_playerContextMap[m_gameControl->GetRightRobot()].lastCards.Clear();
    m_playerContextMap[m_gameControl->GetUserPlayer()].lastCards.Clear();
    //重置玩家角色（地主/农民）
    m_playerContextMap[m_gameControl->GetLeftRobot()].info->hide();
    m_playerContextMap[m_gameControl->GetRightRobot()].info->hide();
    m_playerContextMap[m_gameControl->GetUserPlayer()].info->hide();
    //重置卡牌正反面
    m_playerContextMap[m_gameControl->GetLeftRobot()].isFrontSide = false;
    m_playerContextMap[m_gameControl->GetRightRobot()].isFrontSide = false;
    m_playerContextMap[m_gameControl->GetUserPlayer()].isFrontSide = true;
    //重置角色图片
    m_playerContextMap[m_gameControl->GetLeftRobot()].rolePic->hide();
    m_playerContextMap[m_gameControl->GetRightRobot()].rolePic->hide();
    m_playerContextMap[m_gameControl->GetUserPlayer()].rolePic->hide();

    m_gameControl->Clear(); //重置控制权

    m_baseCard->show(); //显示基牌

    //隐藏按钮
    m_bet0Button->hide();
    m_bet1Button->hide();
    m_bet2Button->hide();
    m_bet3Button->hide();
    m_playHandButton->hide();
    m_passButton->hide();

    m_pickCardTimer->start(); //打开发牌计时器
}

void GamePanel::ProcessGameStatus(GameControl::GameStatus gameStatus) //游戏状态面板处理
{
    m_gameStatus = gameStatus; //gameStatus：枚举类型，有{发牌，叫地主，出牌}三个状态

    if (gameStatus == GameControl::PickingCardStatus) //“发牌”状态时
        StartPickCard(); //“发牌”操作

    else if (gameStatus == GameControl::CallingLordStatus) //“叫地主”状态时
    {
        CardList restThreeCards = m_gameControl->GetRestCards().ToCardList(); //最后3张底牌放入restThreeCards
        for (int i = 0; i < m_restThreeCards.size(); i++)
        {
            m_restThreeCards[i]->SetPic(m_cardPicMap[restThreeCards[i]]->GetPic(), m_cardBackPic); //置背面图片朝上
            m_restThreeCards[i]->hide(); //隐藏图片
        }
        m_gameControl->StartCallLord(); //发送“叫地主”信号给控制类
    }

    else if (gameStatus == GameControl::PlayingHandStatus) //“出牌”状态时
    {
        m_baseCard->hide(); //隐藏基牌
        m_movingCard->hide(); //隐藏移动牌

        for (int i = 0; i < m_restThreeCards.size(); i++) //显示最后3张底牌
            m_restThreeCards[i]->show();

        QMap<Player*, PlayerContext>::ConstIterator it = m_playerContextMap.constBegin();
        for (; it != m_playerContextMap.constEnd(); it++)
        {
            it.value().info->hide(); //隐藏叫分信息

            if (it.key()->GetRole() == Player::Lord) //如果玩家是“地主”
            {
                it.value().rolePic->setPixmap(QPixmap(":/res/lord.png")); //设置“地主”图片
                it.value().rolePic->show(); //显示图片
            }
            else //如果玩家是“农民”
            {
                it.value().rolePic->setPixmap(QPixmap(":/res/farmer.png")); //设置“农民”图片
                it.value().rolePic->show(); //显示图片
            }
        }
    }
}

void GamePanel::OnPlayerStatus(Player* player, GameControl::PlayerStatus playerStatus) //玩家状态的面板处理
{
    if (playerStatus == GameControl::ThinkingForCallLordStatus) //“叫地主”状态
    {
        if (player == m_gameControl->GetUserPlayer())	//若是用户叫分，则显示叫分按钮
        {
            m_bet0Button->show();
            m_bet1Button->show();
            m_bet2Button->show();
            m_bet3Button->show();
        }
    }

    else if (playerStatus == GameControl::ThinkingForPlayHandStatus) //“出牌”状态
    {
        HidePlayerLastCards(player); //隐藏玩家上一次出的牌

        if (player == m_gameControl->GetUserPlayer()) //若当前玩家为用户
        {
            m_playHandButton->show(); //显示“出牌”按钮

            Player* punchPlayer = m_gameControl->GetPunchPlayer(); //获取当前出有效牌的玩家punchPlayer
            //若前一次出有效牌的玩家是用户（即两个Robot都不要时），或者没有玩家出有效牌（首次出牌时）
            if (punchPlayer == m_gameControl->GetUserPlayer() || punchPlayer == NULL)
                m_passButton->hide(); //隐藏“不要”按钮
            else
                m_passButton->show(); //显示“不要”按钮
        }
        else //若当前玩家为机器人
        {
            m_playHandButton->hide(); //隐藏“出牌”按钮
            m_passButton->hide(); //隐藏“不要”按钮
        }
    }

    else if (m_gameStatus == GameControl::WinningStatus) //“赢”状态
    {
        //左右机器人摊牌（卡牌正面向上）
        m_playerContextMap[m_gameControl->GetLeftRobot()].isFrontSide = true;
        m_playerContextMap[m_gameControl->GetRightRobot()].isFrontSide = true;
        UpdatePlayerCards(m_gameControl->GetLeftRobot());
        UpdatePlayerCards(m_gameControl->GetRightRobot());

        UpdateMarkBoard(); //更新分数面板

        //对话框：显示“赢”的玩家
        QString str = tr("%1 wins the game.");
        str = str.arg(player->GetName());
        QMessageBox::information(this, tr("tip"), str);

        m_playHandButton->hide(); //隐藏“出牌”按钮
        m_passButton->hide(); //隐藏“不要”按钮
        m_continueButton->show(); //显示“再来一局”按钮

        // 赢者下一局先叫分
        m_gameControl->SetCurrentPlayer(player);
    }
}
