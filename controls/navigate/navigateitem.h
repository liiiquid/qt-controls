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
    NavigateItem(const QString& title, const QString& iconPath, QObject *parent = nullptr);
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
public:
    static QColor _bgcolor;
    static QColor _hovercolor;
    static QColor _selcolor;
    static QColor _selcolor_font;
    static QColor _fontcolor;
    static QColor _tipcolor;
public:
    bool _isVisible;
public:
    QPixmap _icon;
public:
    QString _title;
    int     _tipCnt;
    int _toIndex;
public:
    bool _isExpand;
    bool _isDelete;
public:
    bool _hover;
    bool _selected;
public:
    static NavigateItem* _lastUpdatedItem;
    static NavigateItem* _lastLastUpdatedItem;

protected:
    float _rotateAngle;
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
    int _timerId[AnimationCnt];
    int _timerCnt[AnimationCnt];
private:
    int _timerId_rightArrow;
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
    virtual void draw(QPainter& painter) = 0;
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
    virtual void mouseClicked(QMouseEvent* ev) = 0;
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
signals:
    void clicked(int toIndex);
public:
    void nodeExpandHeightChanged(int dh);
    void lastExpandItemChanged(NavigateItem* lastExpandItem);
    void nodeContentHeightChanged(float dh);

public:
    void nodeExpandHeightChanged_expcol(int dh);
};

// class NavigateItem_C
class NavigateItem_C : public NavigateItem
{
    Q_OBJECT
public:
    explicit NavigateItem_C(QObject *parent = nullptr);
    NavigateItem_C(const QString& title, QObject *parent = nullptr);
    NavigateItem_C(const QString& title, const QString& iconPath, QObject *parent = nullptr);

public:
    void mouseClicked(QMouseEvent* ev) override;
    void draw(QPainter& painter) override;

private:
    void init();
private:
    QPixmap _indicatorArrow;
};

// class NavigateItem_I
class NavigateItem_I : public NavigateItem
{
    Q_OBJECT
public:
    explicit NavigateItem_I(QObject *parent = nullptr);
    NavigateItem_I(const QString& title, QObject *parent = nullptr);
    NavigateItem_I(const QString& title, const QString& iconPath, QObject *parent = nullptr);

public:
    void mouseClicked(QMouseEvent* ev) override;
    void draw(QPainter& painter) override;

};
#endif // NAVIGATEITEM_H
