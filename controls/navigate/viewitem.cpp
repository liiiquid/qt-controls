#include "viewitem.h"

ViewItem::ViewItem(QObject *parent)
    : QObject{parent}
{}

void ViewItem::draw(QPainter &painter)
{
    painter.save();
    painter.translate(_x, _y);
    auto r = QRect(0, 0, _w, _h);
    r.adjust(15,11,-1,-1);
    if(  _hover )
        painter.fillRect(0, 0, _w - 1, _h - 1, QColor(255,255,255));
    painter.drawText(r, _item->_title);
    painter.drawRect(0, 0, _w -1, _h -1);
    painter.restore();
}

bool ViewItem::inRange(QPoint pos)
{
    if( _x < pos.x() && pos.x() < _x + _w && _y < pos.y() && pos.y() < _y + _curh ) return true;
    return false;
}

void ViewItem::mouseClicked()
{
    _item->setExpand(!_item->_isExpand);
}
