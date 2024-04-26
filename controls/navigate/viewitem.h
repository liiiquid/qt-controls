#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QObject>
#include "navigateitem.h"

class ViewItem : public QObject
{
    Q_OBJECT
public:
    enum AnimateState{
        Normal,
        Expanding,
        Collapsing,
    };
public:
    explicit ViewItem(QObject *parent = nullptr);

public:
    NavigateItem* _item;
    QVector<ViewItem> _childs;
    int _curHeight;
    AnimateState _state;
public:
    int _w;
    int _h;
    int _x;
    int _y;
    int _curh;
public:
    bool _hover;
    bool _isVisible;
public:
    void draw(QPainter& painter);

public:
    int width() {return _w;}
    int height() {return _h;}
    void setFixedSize(QSize size){_w = size.width(); _h = size.height();}
    void hide() {_isVisible = false; }
    void show() {_isVisible = true; }
    QPoint pos(){return QPoint(_x, _y);}
    void setHover(bool hover){_hover = hover;}
    void move(int x, int y){_x = x; _y = y; }
public:
    bool inRange(QPoint pos);
public:
    void mouseClicked();
signals:
};

#endif // VIEWITEM_H
