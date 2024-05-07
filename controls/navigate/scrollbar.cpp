#include "scrollbar.h"

const float ExcessScrollRatio = 0.5f;
const int AnimationDuration = 500;
const int AnimationBounceDuration = 500;
const int TimerInterval = 5;
const int AnimationDurationMax = 250;
const int AngleDeltaMax = 120;
const int HideInterval = 1000;
ScrollBar::ScrollBar(QWidget *parent)
    : QWidget{parent}
    , _contentOffset(0)
    , _bary(0)
{
    _r = 0;
    for(int i = 0; i < TimerCnt; i++)
    {
        _timerCnt[i] = 0;
        _timerId[i] = 0;
    }
    _scrollTag = Static;
    _isReachBound = false;

}


void ScrollBar::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    drawVerticalScroll(painter);
    //painter.drawRect(0, 0, width() - 1, height() - 1);
}

void ScrollBar::drawVerticalScroll(QPainter &painter)
{
    painter.save();
    painter.fillRect( _barx, _bary, _barWidth, _barHeight, QColor(0, 0, 0));
    painter.restore();
}

void ScrollBar::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == _timerId[TimerAnimation])
    {
        double d1 = 2 * _r1 / _t1;
        _dr =  -d1 / _t1 * _timerCnt[TimerAnimation] + d1;
        _r -= _dr;
        _timerCnt[TimerAnimation] += 1;
        //qDebug() << _dr << _r;

        if(_scrollTag == Upwards)
            _contentOffset -= _dr;
        else if(_scrollTag == Downwards)
            _contentOffset += _dr;
        _contentOffset = qMax(_lowerBound, (double)_contentOffset);
        _contentOffset = qMin(_upperBound, (double)_contentOffset);
        if(_contentOffset == _upperBound || _contentOffset == _lowerBound)
        {
            _r = 0;
        }

        if( (_r  <= 0 /*&& _scrollTag == Downwards) || (_r <= 0 && _scrollTag == Upwards*/))
        {
            killTimer(_timerId[TimerAnimation]);
            _timerId[TimerAnimation] = 0;
            _r = 0;
            if(_isReachBound == true)
            {
                if(_scrollTag == Upwards)
                {
                    _contentOffset = _overPoint;

                }else if(_scrollTag == Downwards)
                {
                    _contentOffset = 0;
                }
                _isReachBound = false;
            }
            _scrollTag = Static;

            if( _contentOffset > _overPoint)
            {
                qDebug() << "the value above the _overPoint:" << _contentOffset - _overPoint;
                _isReachBound = true;
                scroll( _contentOffset - _overPoint );

            }else if(_contentOffset < 0)
            {
                qDebug() << "the value below the 0  " << 0 - _contentOffset;
                _isReachBound = true;
                scroll( _contentOffset );
            }

        }

        requestScroll();
    }
    else if(ev->timerId() == _timerId[TimerHide])
    {
        static int i =0;
        qDebug() << "hide scroll" << i++;
        killTimer(_timerId[TimerHide]);
        _timerId[TimerHide] = 0;
        hide();
    }
    else
    {
        qDebug() << "ScrollBar::timerEvent: unknown timer found";
    }
}

void ScrollBar::scroll(int step)
{
    //if( (step < 0 && _contentOffset == _overPoint) || (step > 0 && _contentOffset == 0) ) return;
    if(step == 0)
    {
        _scrollTag = Static;
        _r = 0;
        return;
    }
    if(!canScroll())
    {
        qDebug() << "can not scroll";
        return;
    }
    if(isVisible() == false)
    {
        show();
    }

    //qDebug() << "ScrollBar::scroll called, viewHeight: " << _viewHeight << ", contentHeight: " << _contentHeight << ", height(): " << height();

    if(_timerId[TimerAnimation] == 0)
    {
        _timerId[TimerAnimation] = startTimer(TimerInterval);
        if(_timerId[TimerAnimation] == 0)
        {
            qDebug() << "ScrollBar::scroll start timer error";
        }
        _timerCnt[TimerAnimation] = 0;
    }

    // before upwards, now downwards, direction change.
    if(step < 0 &&  (_scrollTag == Upwards || _scrollTag == Static ))
    {
        _r = 0;
        _scrollTag = Downwards;
        _scrollCnt = 0;
    }
    // before downwards, now upwards, direction change.
    else if(step > 0 && (_scrollTag == Downwards || _scrollTag == Static))
    {
        _r = 0;
        _scrollTag = Upwards;
        _scrollCnt = 0;
    }
    step = qAbs(step);

    if(_isReachBound)
    {
        _t1 = AnimationBounceDuration / TimerInterval;
        _r = step;
    }
    else
    {
        float excess = qMin(ExcessScrollRatio * _viewHeight*1.0, _scrollCnt * 10.0);
        _upperBound = _overPoint + excess;
        _lowerBound = -excess;
        _r += qMin(AngleDeltaMax, step + _scrollCnt * 50);
        _t1 = qMin( AnimationDurationMax * 1.0, AnimationDuration * 1.0 / TimerInterval + _scrollCnt * 20);
    }
    _r1 = _r;
    _timerCnt[TimerAnimation] = 0;
    _scrollCnt += 1;

}

void ScrollBar::requestScroll()
{

    updateBar();
    if( !canScroll() )
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
    setFixedHeight(_viewHeight);
    _scrollRatio = height() * 1.0 / _contentHeight;
    _barHeight = _viewHeight * _scrollRatio;
    _overPoint = _contentHeight* 1.0 - _viewHeight;

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
bool ScrollBar::canScroll()
{
    return _viewHeight < _contentHeight;
}

void ScrollBar::show()
{
    // if(_timerId[TimerHide] != 0)
    // {
    //     killTimer(_timerId[TimerHide]);
    // }
    // _timerId[TimerHide] = startTimer(HideInterval);
    __super::show();
}
void ScrollBar::wheelEvent(QWheelEvent *ev)
{
}

void ScrollBar::resizeEvent(QResizeEvent *ev)
{
}

