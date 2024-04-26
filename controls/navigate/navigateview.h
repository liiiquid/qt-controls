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
    QVector<NavigateItem*> _allItem;
    QVector<NavigateItem*> _lastShow;
    NavigateItem* _root;
private:
    int _contentHeight;
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
    /**
     * too complicated, have discarded it until i can solve...
     * i just want to make an transitional animation with O(c) time complexity
     * now it seems that the management of tree control with transitional animation is similar to Qt's multi-level window management.
     */
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

signals:
    void heightChanged(int contentHeight, int viewHeight);

public slots:
    void onScrolled(int contentOffset);

private slots:
    void onUpdateTree(NavigateItem * item);

};

#endif // NAVIGATEVIEW_H
