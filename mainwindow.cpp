#include "mainwindow.h"
#include "global.h"
#include "QPushButton"

#include <QLineEdit>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
QLineEdit* edit = nullptr;
QGridLayout* gLayout;
MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    resize(MainWindowWidth, MainWindowHeight);
    QBoxLayout* hLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    gLayout = new QGridLayout(this);

    initLayout(hLayout);

    _navigator = new NavigateEntry(this);
    _navigator->setFixedWidth(this->width() * NavigatorMaximumWidthFactor);

    hLayout->addWidget(_navigator);

    QPushButton* btn = new QPushButton("add top");
    QPushButton* btn1 = new QPushButton("remove top");
    QPushButton* btn2 = new QPushButton("add 1-level");
    QPushButton* btn3 = new QPushButton("remove 1-level");
    QPushButton* btn4 = new QPushButton("add 2-level");
    QPushButton* btn5 = new QPushButton("remove 2-level");
    QVector<QPushButton*> x;
    x.push_back(btn);
    x.push_back(btn1);
    x.push_back(btn2);
    x.push_back(btn3);
    x.push_back(btn4);
    x.push_back(btn5);
    for(auto y: x)
    {
        y->setFixedSize(120, 50);
    }
    edit = new QLineEdit();

    gLayout->addWidget(btn, 0, 0);
    gLayout->addWidget(btn1, 0, 1);

    gLayout->addWidget(btn2, 1, 0);
    gLayout->addWidget(btn3, 1, 1);
    gLayout->addWidget(btn4, 2, 0);
    gLayout->addWidget(btn5, 2, 1);
    gLayout->addWidget(edit, 3, 0);

    hLayout->addLayout(gLayout);
    hLayout->setSpacing(60);

    connect(btn, &QPushButton::clicked, this, [&]()->void{
        qDebug() << dynamic_cast<QPushButton*>(sender())->text() << "clicked";
        int rank = edit->text().toInt();
        static int i = 0;
        _navigator->addItem(new NavigateItem_C(QString("Title%1").arg(i++)), rank);
    });

    connect(btn1, &QPushButton::clicked, this, [&]()->void{
        qDebug() << dynamic_cast<QPushButton*>(sender())->text() << "clicked";
        int rank = edit->text().toInt();

        _navigator->removeItem(rank);
    });

    connect(btn2, &QPushButton::clicked, this, [&]()->void{
        qDebug() << dynamic_cast<QPushButton*>(sender())->text() << "clicked";
        NavigateItem* p0 = _navigator->_view->_root->_childs[0];
        int rank = edit->text().toInt();
        static int i = 0;
        _navigator->addItem(p0, new NavigateItem_C(QString("Title0-%1").arg(i++)), rank);
    });

    connect(btn3, &QPushButton::clicked, this, [&]()->void{
        qDebug() << dynamic_cast<QPushButton*>(sender())->text() << "clicked";
        static int i = 1;
        NavigateItem* p0 = _navigator->_view->_root->_childs[0];
        int rank = edit->text().toInt();
        _navigator->removeItem(p0, rank);
        //_navigator->addItem(new NavigateItem(QString("Title%1").arg(i++)), 0);
    });

    connect(btn4, &QPushButton::clicked, this, [&]()->void{
        qDebug() << dynamic_cast<QPushButton*>(sender())->text() << "clicked";
        static int i = 1;
        NavigateItem* p0 = _navigator->_view->_root->_childs[0]->_childs[0];
        int rank = edit->text().toInt();
        _navigator->addItem(p0, new NavigateItem_C(QString("Title0-0-%1").arg(i++)), rank);

    });

    connect(btn5, &QPushButton::clicked, this, [&]()->void{
        qDebug() << dynamic_cast<QPushButton*>(sender())->text() << "clicked";
        //_navigator->removeItem(_navigator->_view->_allItem[1], 0);
        NavigateItem* p0 = _navigator->_view->_root->_childs[0]->_childs[0];
        int rank = edit->text().toInt();
        _navigator->removeItem(p0, rank);
    });

    QFile file("://config/catagory.json");
    Q_ASSERT(file.open(QFile::ReadOnly));
    QDataStream qds(&file);
    QByteArray arr;
    int len;
    while( (len = qds.readRawData(buffer, 1024)) != 0 )
    {
        arr.append(buffer, len);
    }
    _navigator->initWithJson(arr);
    // NavigateItem* p0 = new NavigateItem("Title0");
    // _navigator->addItem(p0);

    // for(int i = 0; i < 5; i++)
    // {
    //     _navigator->addItem(p0, new NavigateItem(QString("Title0-%1").arg(i)));
    // }
    // NavigateItem* p1 = new NavigateItem("Title1");
    // _navigator->addItem(p1);
    // for(int i = 0; i < 5; i++)
    // {
    //     _navigator->addItem(p1, new NavigateItem(QString("Title1-%1").arg(i)));
    // }
    // NavigateItem* p2 = new NavigateItem("Title2");
    // _navigator->addItem(p2);
    // for(int i = 0; i < 5; i++)
    // {
    //     _navigator->addItem(p2, new NavigateItem(QString("Title1-%1").arg(i)));
    // }

    // _navigator->setExpand(p0, true);
    // _navigator->setExpand(p1, true);
    _navigator->setItemSize(QSize(100, 50));

}

void MainWindow::resizeEvent(QResizeEvent *ev)
{
    //qDebug() << "MainWindow::resizeEvent(QResizeEvent* ev)" << ev->oldSize() << size();
    gLayout->setSpacing(0);
}

void MainWindow::initLayout(QBoxLayout *layout)
{

}
