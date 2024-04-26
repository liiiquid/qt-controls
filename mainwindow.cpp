#include "mainwindow.h"
#include "global.h"
#include "QPushButton"
MainWindow::MainWindow(QWidget *parent)
    : QWidget{parent}
{
    resize(MainWindowWidth, MainWindowHeight);
    QBoxLayout* hLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    initLayout(hLayout);
    _navigator = new NavigateEntry(this);
    _navigator->setMaximumWidth(this->width() * NavigatorMaximumWidthFactor);
    hLayout->addWidget(_navigator);

    QPushButton* btn = new QPushButton("clicked");
    hLayout->addWidget(btn);
    NavigateItem* p0;
    NavigateItem* p1 = new NavigateItem("Title0");
    p0 = p1;
    _navigator->addItem(p1);
    // for(int i = 0; i < 1; i++)
    // {
    //     _navigator->addItem(new NavigateItem(QString("Title%1").arg(i)));
    // }
    for(int i = 0; i < 50; i++)
    {
        _navigator->addItem(p1, new NavigateItem(QString("Title0-%1").arg(i)));
    }
    p1 = new NavigateItem("Title1");
    _navigator->addItem(p1);
    // for(int i = 0; i < 5; i++)
    // {
    //     _navigator->addItem(p1, new NavigateItem(QString("Title1-%1").arg(i)));
    // }
    // for(int i = 0; i < 50; i++ )
    // {
    //     _navigator->addItem(_navigator->_view->_allItem[1]->_childs[0], new NavigateItem(QString("Title1-0-%1").arg(i)));
    // }

    //p0->setExpand(true);
    //_navigator->_view->_allItem[1]->_childs[0]->setExpand(true);
    connect(btn, &QPushButton::clicked, this, [&]()->void{
        qDebug() << "clicked";
        static int i = 1;
        //_navigator->removeItem(_navigator->_view->_allItem[1], 0);
        _navigator->removeItem(0);
        //_navigator->addItem(new NavigateItem(QString("Title%1").arg(i++)), 0);
    });

    //_navigator->setItemSize(QSize(100, 30));
}

void MainWindow::resizeEvent(QResizeEvent *ev)
{
    //qDebug() << "MainWindow::resizeEvent(QResizeEvent* ev)" << ev->oldSize() << size();
}

void MainWindow::initLayout(QBoxLayout *layout)
{

}
