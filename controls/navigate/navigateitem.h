#ifndef NAVIGATEITEM_H
#define NAVIGATEITEM_H

#include <QWidget>
#include <QList>
#include <QPropertyAnimation>
#include <QRect>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>

class NavigateItem : public QObject
{
    Q_OBJECT
public:
    explicit NavigateItem(QObject *parent = nullptr);
    NavigateItem(const QString& title, QObject *parent = nullptr);

    ~NavigateItem();
public:
    typedef enum{
        Adding,
        Expanding,
        Collapsing,
        Removing,
        Normal,
        AnimationCnt
    }AnimatedState;
    typedef enum{
        Add,
        Expand,
        Collapse,
        Remove,
        Non,
    }ActionState;
public:
    NavigateItem* _parent;
    QVector<NavigateItem*> _childs;
    NavigateItem* _next;
public:
    int _contentOffset;
public:
    float _nodeContentHeight;
    int _nodeExpandHeight;
public:
    int _w;
    int _h;
    float _ch;
    int _x;
    int _y;
public:
    int _visibleLen;
public:
    // represents real time status of the item
    AnimatedState _state;
    QColor _bgcolor;
public:
    bool _isVisible;
public:
    QPixmap _icon;
    QString _title;
public:
    int     _tipCnt;
public:
    bool _isExpand;
    bool _isDelete;
public:
    bool _hover;
public:
    static NavigateItem* _lastUpdatedItem;
    static NavigateItem* _lastLastUpdatedItem;
private:
    QMap<NavigateItem*, int> _childsIndex;
private:
    NavigateItem* _lastExpandItem;
    bool _isAllExpanded;
private:
    int _cth;
    float _cch;
    float _cch0;
    float _dh;
    float _ch0;
private:
    float _t1;
    float _r1;
    float _dr;
private:
    // represents current action, which is the combination of several AnimatedState, and controls the running and switching of AnimatedState.
    ActionState _action;
    int _timerId[5];
    int _timerCnt[5];

private:
    qint64 _tt;
public:
    void setExpand(bool isExpand);
    // update the contentOffset of every child nodes
    // and return the height of this node
    int updateOffset(int off, int& rh);
    int updateOffset_delta(int &rh);
    void addChild(NavigateItem *item);
    void addChild(NavigateItem* item, int rank);
    void removeChild(int);
    void removeChild(NavigateItem* item);
public:
    void draw(QPainter& painter);
public:
    int width() {return _w;}
    int height() {return _h;}
    void setFixedSize(QSize size){_w = size.width(); nodeExpandHeightChanged(size.height() - _h);_h = size.height();  }
    void hide() {_isVisible = false; }
    void show() {_isVisible = true; }
    QPoint pos(){return QPoint(_x, _y);}
    void setHover(bool hover){_hover = hover;}
    void move(int x, int y){_x = x; _y = y; }
public:
    bool inRange(QPoint pos);
public:
    void mouseClicked(QMouseEvent* ev);
public:
    void addAnimation(int t);
    void expandAnimation(int t);
    void collapseAnimation(int t);
    void remove(int t);

    NavigateItem* findAnimationAncestor_nearest(AnimatedState state);
    NavigateItem* findAnimationAncestor_nearest_neg(AnimatedState state);
public:
    void setVisibleLen(NavigateItem *item);
protected:
    void timerEvent(QTimerEvent* ev) override;
private:
    void init();
    int internalUpdateOffset(int off, float& rh, NavigateItem* root);
    float internalUpdateOffset_delta_exp(float& rh);
    float internalUpdateOffset_delta_col(float& rh);
    float internalUpdateOffset_delta_add(float& rh);
    void internalExpandParent(NavigateItem* item);
    void internalCollapseChild(NavigateItem* item);
    void internalDeleteChild();
private:
    void saveContext();
    void recoverContext();
private:
    void removeAnimation(int t);
signals:
    void collapsed(NavigateItem* );
    void expanded(NavigateItem* );
    void updateTree(NavigateItem*);
public:
    void nodeExpandHeightChanged(int dh);
    void lastExpandItemChanged(NavigateItem* lastExpandItem);
    void nodeContentHeightChanged(float dh);

public:
    void nodeExpandHeightChanged_expcol(int dh);
};


#endif // NAVIGATEITEM_H
