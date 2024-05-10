#include "navigateitem.h"
#include <QDebug>
#include <QMouseEvent>

NavigateItem_C::NavigateItem_C(QObject *parent)
    : NavigateItem{parent}
{
    init();
}

NavigateItem_C::NavigateItem_C(const QString &title, QObject *parent)
    : NavigateItem(title, parent)
{
    init();
}

NavigateItem_C::NavigateItem_C(const QString &title, const QString &iconPath, QObject *parent)
    : NavigateItem(title, iconPath, parent)
{
    init();
}

void NavigateItem_C::mouseClicked(QMouseEvent *ev)
{
    qDebug() << _title << " clicked";
    auto x = findAnimationAncestor_nearest_neg(Normal);
    if( x != this && x != nullptr) return;
    if( ev->buttons() &Qt::LeftButton )
    {
        if(_state == Collapsing)
        {
            setExpand(true);
        }else if(_state == Expanding)
        {
            setExpand(false);
        }else
        {
            setExpand(!_isExpand);
        }
    }

}

void NavigateItem_C::draw(QPainter &painter)
{
    painter.save();
    painter.translate(_x, _y);
    auto r = QRect(0, 0, _w, _h);
    painter.fillRect(r, _bgcolor);
    if( _hover )
        painter.fillRect(r, _hovercolor);
    if(_selected)
        painter.fillRect(r, _selcolor);

    int pw;
    int ph;
    int cx;
    int cy;
    int leftMargin = 5;
    // left icon region
    pw = _h * 0.6;
    ph = pw;
    cx = leftMargin;
    cy = (_h - ph) >> 1;
    painter.drawPixmap( cx, cy, pw, ph , _icon);

    // middle text region
    if(_selected)
        painter.setPen(_selcolor_font);
    else painter.setPen(_fontcolor);
    painter.drawText(r.adjusted(_h, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, _title);

    // right indicating region
    pw = _h * 0.3;
    ph = pw;
    cx = _w - (_h >> 1);
    cy = _h >> 1;
    painter.translate(cx, cy);
    painter.rotate(_rotateAngle);
    painter.drawPixmap(QRect(-pw>>1, -ph >> 1, pw, ph), _indicatorArrow);
    //painter.drawRect(0, 0, _w -1, _h -1);
    painter.restore();
}

void NavigateItem_C::init()
{
    _indicatorArrow.load(":/icons/indicator_right.png");
}
