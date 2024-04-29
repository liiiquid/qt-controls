#include "scrollbar.h"

ScrollBar::ScrollBar(QWidget *parent)
    : QWidget{parent}
    , _contentOffset(0)
    , _bary(0)
{
}


void ScrollBar::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    drawVerticalScroll(painter);
    painter.drawRect(0, 0, width() - 1, height() - 1);
}

void ScrollBar::wheelEvent(QWheelEvent *ev)
{
    QPoint numSteps = ev->angleDelta() / _scrollFactor;
    scroll(numSteps.y());

}

void ScrollBar::resizeEvent(QResizeEvent *ev)
{
    _scrollRatio = height() * 1.0 / _contentHeight;
    _barHeight = _viewHeight * _scrollRatio;
    updateBar();
}


void ScrollBar::drawVerticalScroll(QPainter &painter)
{
    painter.save();
    painter.fillRect( _barx, _bary, _barWidth, _barHeight, QColor(0, 0, 0));
    painter.restore();
}


void ScrollBar::scroll(int step)
{
    //qDebug() << "viewHeight: " << _viewHeight << ", contentHeight: " << _contentHeight << ", height(): " << height();
    if(_viewHeight >= _contentHeight )
    {
        hide();
        emit scrolled(0);
        return;
    }
    _contentOffset -= step;
    _contentOffset = qMax(0, _contentOffset);
    _contentOffset = qMin(_contentHeight - _viewHeight, _contentOffset);
    if(_contentOffset == _contentHeight - _viewHeight)
    {
        _contentOffset -= 1;
        emit reachBottom();
        return;
    }
    updateBar();
    show();
    emit scrolled(_contentOffset);

}

void ScrollBar::setHeight(int contentHeight, int viewHeight)
{
    _contentHeight = contentHeight;
    _viewHeight = viewHeight;
    _scrollRatio = height() * 1.0 / _contentHeight;
    _barHeight = _viewHeight * _scrollRatio;

    scroll(0);
}

void ScrollBar::updateBar()
{
    int w = width();
    _barWidth = w * 1.0 * _barWPct;
    _barx = (w - _barWidth) >> 1;
    _bary = _scrollRatio * _contentOffset;

    //qDebug() << "_bary" << _bary << ", _barheight: " << _barHeight;
    //qDebug() << "_contentOffset: " << _contentOffset << ", max: " << _contentHeight - _viewHeight;
    update();
}
