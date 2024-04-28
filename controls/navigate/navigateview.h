#ifndef NAVIGATEVIEW_H
#define NAVIGATEVIEW_H

#include <QWidget>
#include <QList>
#include <QStack>
#include "navigateitem.h"

class NavigateView : public QWidget
{
    Q_OBJECT

public:
    explicit NavigateView(QWidget *parent = nullptr);
public:
    QSize _itemSize;
public:
    QVector<NavigateItem*> _lastShow;
    NavigateItem* _root;
private:
    int _contentHeight;
    int _lastContentOffset;
private:
    NavigateItem* _hoverItem;
private:
    bool _initOk;
    QVector<QPair<int, NavigateItem*>> _operations;
public:
    //int contentHeight();
public:
    // tree control
    // void  addItem(NavigateItem* item);
    // void addItem(NavigateItem* item, int rank);
    void addItem(int parentContentOff, NavigateItem* item);
    void addItem(NavigateItem* parentItem, NavigateItem* item);
    void addItem(NavigateItem* parentItem, NavigateItem* item, int rank);
    //void  removeItem(int i);
    void removeItem(NavigateItem* parent, int i);
public:
    void onExpanded(NavigateItem* item, int contentOffset);
    void onCollapsed(NavigateItem* item, int contentOffset);

public:
    void setItemHeight(int height);
public:
    NavigateItem* search(int contentOffset);

protected:
    void paintEvent(QPaintEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    bool event(QEvent* ev) override;
    void mousePressEvent(QMouseEvent*) override;
private:
    void setAllItemSize(const QVector<NavigateItem*>& roots);
    NavigateItem *internalSearch(int contentOffset, const QVector<NavigateItem*>& roots);
    NavigateItem *internalSearch(int contentOffset, const QVector<NavigateItem*>& roots, int len);
private:
    void onResize();
private:

    //void updateSubView(NavigateItem* item, int x, int &y, int& cnt, NavigateItem* parent);
    //void updateSubView(NavigateItem* item, int x, int &y, int& cnt);
    void updateSubView(NavigateItem* item, int x, int &y, int& rh);
    void updateView(NavigateItem* st_item, int d1);
    /**
     * @brief updateContentOffset
     * @param item
     * @param off
     * @return return the total height from item->_contentOffset to _allItem[n-1]
     * you can get the delta height of from item to _allItem[n-1] with return_value - item->_contentOffset
     */
    int updateContentOffset(NavigateItem* item, int off);
    /**
     * too complicated, have discarded it until i can solve...
     * i just want to make an transitional animation with time complexity of O(C)
     * now it seems that the management of tree control with transitional animation is similar to Qt's multi-level window management.
     *
     * it seems to be working fine, more testing to be done.
     * it can handle different states accrordingly and return the final nodeContentHeight
     * the update of content height now only depends on the delta value of height of the node and all of its descendants.
     * 2024.4.29
     */
    int updateContentOffset(NavigateItem* item, int off, int rh);
private:
    void printInfor(NavigateItem* root);
signals:
    void heightChanged(int contentHeight, int viewHeight);

public slots:
    void onScrolled(int contentOffset);
    void onReachBottom();
private slots:
    void onUpdateTree(NavigateItem * item);

};

#endif // NAVIGATEVIEW_H
