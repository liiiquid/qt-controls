#ifndef NAVIGATEENTRY_H
#define NAVIGATEENTRY_H

#include <QBoxLayout>
#include "navigateview.h"
#include "scrollbar.h"

class NavigateEntry : public QWidget
{
    Q_OBJECT
public:
    explicit NavigateEntry(QWidget *parent = nullptr);

public:
    NavigateView* _view;

public:
    ScrollBar* _scrollBar;

public:
    QBoxLayout* _hLayout;

public:
    void addItem(const QString& title);
    void addItem(NavigateItem* item);
    void addItem(NavigateItem* item, int rank);
    void addItem(NavigateItem* parent, NavigateItem* item);
    void removeItem(int i);
    void removeItem(NavigateItem* parent, int i);
    void scroll(int step);
public:
    void setItemSize(QSize size);
protected:
    void wheelEvent(QWheelEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;
    void timerEvent(QTimerEvent* ev) override;
private:
    void initView();
    void initScrollBar();
    void initLayout();

private slots:
    void onExpanded(NavigateItem* item){emit expanded(item, _scrollBar->_contentOffset);}
    void onCollapsed(NavigateItem* item){emit collapsed(item, _scrollBar->_contentOffset);}
    void onHeightChanged(int contentHeight, int viewHeight);
signals:
    void expanded(NavigateItem*, int);
    void collapsed(NavigateItem*, int);

};

#endif // NAVIGATEENTRY_H
