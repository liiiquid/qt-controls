#include "navigateitem.h"
#include <QDebug>
#include <qapplication.h>
NavigateItem_I::NavigateItem_I(QObject *parent)
    : NavigateItem{parent}
{}

NavigateItem_I::NavigateItem_I(const QString &title, QObject *parent)
    : NavigateItem(title, parent)
{

}

NavigateItem_I::NavigateItem_I(const QString &title, const QString &iconPath, QObject *parent)
    : NavigateItem(title, iconPath, parent)
{

}

void NavigateItem_I::mouseClicked(QMouseEvent *ev)
{
    qDebug() << _title << " clicked";
    auto x = findAnimationAncestor_nearest_neg(Normal);
    emit clicked(_toIndex);
    if( x != this && x != nullptr) return;
}

void NavigateItem_I::draw(QPainter &painter)
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
    if(_tipCnt != INT_MIN)
    {
        const QString& tipCnt = _tipCnt > 999 ? QString("999+") : QString("%1").arg(_tipCnt);
        QFont font = QApplication::font();
        QFontMetrics fm(font);

        ph = 22 >> 1;
        pw = fm.horizontalAdvance(tipCnt) >> 1;
        pw = pw > ph ? pw : ph;
        cx = _w - (_h >> 1);
        cy = _h >> 1;
        r = QRect(-pw, -ph, pw << 1, ph << 1);

        painter.translate(cx, cy);

        font.setPixelSize(12);
        painter.setFont(font);

        painter.setPen(_tipcolor);
        painter.setBrush(_tipcolor);

        painter.drawRoundedRect(r, ph, ph);

        painter.setPen( "#fff" );
        painter.drawText(r, Qt::AlignCenter, tipCnt);
    }


    //painter.drawRect(0, 0, _w -1, _h -1);
    painter.restore();
}
