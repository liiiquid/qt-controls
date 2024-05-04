#include "scrollbar.h"
#include <QtMath>

const float OverScrollRatio = 1.0;
const int AnimationDuration = 275;
const int TimerInterval = 5;

ScrollBar::ScrollBar(QWidget *parent)
    : QWidget{parent}
    , _contentOffset(0)
    , _bary(0)
{
    _r = 0;
    _timerCnt = 0;
    for(int i = 0; i < TimerCnt; i++)
    {
        _timerId[i] = 0;
    }
}


void ScrollBar::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    drawVerticalScroll(painter);
    //painter.drawRect(0, 0, width() - 1, height() - 1);
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

void ScrollBar::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == _timerId[TimerAnimation])
    {
        _r -= _dr;
        if( _r <= _r1)
        {
            _dr /= 1.5;
            _r1 = _r + 1;
        }
        if( qRound(_r) == 0)
        {
            killTimer(_timerId[TimerAnimation]);
            _timerId[TimerAnimation] = 0;
        }
        _contentOffset -= _dr;
        _contentOffset = qMax(0.0, (double)_contentOffset);
        _contentOffset = qMax(0.0, qMin(_contentHeight * 1.0 - (_viewHeight * 1.0 / OverScrollRatio), (double)_contentOffset));
        requestScroll();
    }
}


void ScrollBar::drawVerticalScroll(QPainter &painter)
{
    painter.save();
    painter.fillRect( _barx, _bary, _barWidth, _barHeight, QColor(0, 0, 0));
    painter.restore();
}


void ScrollBar::scroll(int step)
{
    //qDebug() << "ScrollBar::scroll called, viewHeight: " << _viewHeight << ", contentHeight: " << _contentHeight << ", height(): " << height();

    if( qRound(_r) == 0)
    {
        if(_timerId[TimerAnimation] == 0)
        {
            _timerId[TimerAnimation] = startTimer(TimerInterval);
            if(_timerId[TimerAnimation] == 0)
            {
                qDebug() << "ScrollBar::scroll start timer error";
            }
            _timerCnt = 0;
        }
    }

    // before upwards, now downwards, direction change.
    if(step < 0 && _scrollUp == true)
    {
        _r = 0;
    }
    // before downwards, now upwards, direction change.
    else if(step > 0 && _scrollUp == false)
    {
        _r = 0;
    }

    _r += step;
    _r1 = _r / 3;
    _dr = _r / ( (AnimationDuration) * 1.0 / TimerInterval);
}

void ScrollBar::requestScroll()
{
    updateBar();
    if(_viewHeight >= _contentHeight )
    {
        hide();
    }else
    {
        show();
    }
    emit scrolled(qRound(_contentOffset));
    if(_contentOffset >= _contentHeight - _viewHeight)
    {
        emit reachBottom();
    }
}

void ScrollBar::setHeight(int contentHeight, int viewHeight)
{
    _contentHeight = contentHeight;
    _viewHeight = viewHeight;
    _scrollRatio = height() * 1.0 / _contentHeight;
    _barHeight = _viewHeight * _scrollRatio;

    requestScroll();

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


