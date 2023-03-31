/*
 *<代码量统计>
 *Headers: 728 lines
 *Sources: 2997 lines
 *Forms: 166 lines
 */

#include "MainFrame.h"
#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QMediaPlayer>

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling); //解决文字遮挡显示不全的问题
    QApplication a(argc, argv); //定义并创建应用程序

    //加载语言包，“zh.qm”做出程序中英文->窗口中中文的转换
    QTranslator translator;
    translator.load(":/zh.qm", QApplication::applicationDirPath()); //在可执行文件所在目录下加载
    a.installTranslator(&translator);

    //加载qss文件并配置
    QFile qssFile(":/stylesheet.qss");
    qssFile.open(QIODevice::ReadOnly);
    QString stylesheet = QString::fromUtf8(qssFile.readAll());
    a.setStyleSheet(stylesheet);
    qssFile.close();

    MainFrame w; //定义并创建窗口
    w.setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint); //窗口显示最小化、最大化、关闭按钮
    w.show(); //显示窗口

    return a.exec(); //运行应用程序
}
