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
        Normal
    }AnimatedState;

    int _timerId[5];
public:
    NavigateItem* _parent;
    QVector<NavigateItem*> _childs;
    NavigateItem* _next;
public:
    int _contentOffset;
public:
    int _w;
    int _h;
    float _ch;
    int _x;
    int _y;
public:
    float _dh;
public:
    int _cth;
    float _cch;
    int _visibleLen;
public:
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
    void setExpand(bool isExpand);
    // update the contentOffset of every child nodes
    // and return the height of this node
    int updateOffset(int off);
    int nodeHeight();
    void addChild(NavigateItem *item);
    void addChild(NavigateItem* item, int rank);
    void removeChild(int);
    void removeChild(NavigateItem* item);
public:
    void draw(QPainter& painter);
public:
    int width() {return _w;}
    int height() {return _h;}
    void setFixedSize(QSize size){_w = size.width();_h = size.height(); }
    void hide() {_isVisible = false; }
    void show() {_isVisible = true; }
    QPoint pos(){return QPoint(_x, _y);}
    void setHover(bool hover){_hover = hover;}
    void move(int x, int y){_x = x; _y = y; }
public:
    bool inRange(QPoint pos);
public:
    void mouseClicked();
public:
    void addAnimation(int sh, int eh, int t);
    void expandAnimation(int t);
    void collapseAnimation(int t);
    void removeAnimation(int t);
    NavigateItem* findAnimationAncestor();
protected:
    void timerEvent(QTimerEvent* ev) override;
private:
    void init();
    int internalUpdateOffset(int off, NavigateItem* root);
    int internalUpdateOffset(int off, int& rh, NavigateItem* root);
    int internalUpdateOffset(int off, float& rh, NavigateItem* root);
    int internalNodeHeight(NavigateItem* root);
    void internalExpandParent(NavigateItem* item);
    void internalCollapseChild(NavigateItem* item);
    void internalDeleteChild();
signals:
    void collapsed(NavigateItem* );
    void expanded(NavigateItem* );
    void updateTree(NavigateItem*);
};


#endif // NAVIGATEITEM_H
