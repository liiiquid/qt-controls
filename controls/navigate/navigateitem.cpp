#include "navigateitem.h"
#include <QDebug>
#include <QDateTime>
#include <QEvent>
const int TimerInverval = 5;
NavigateItem::NavigateItem(QObject *parent)
    : QObject{parent}
{
    _tipCnt = 0;
    _title = "Title";
    init();
}
NavigateItem::NavigateItem(const QString &title, QObject *parent)
    : QObject{parent}
{
    _tipCnt = 0;
    _title = title;
    init();

}

NavigateItem::~NavigateItem()
{
    internalDeleteChild();
}

void NavigateItem::draw(QPainter &painter)
{
    painter.save();
    painter.translate(_x, _y);

    painter.fillRect(0, 0, _w - 1, _h - 1, _bgcolor);
    auto r = QRect(0, 0, _w, _h);
    r.adjust(15,11,-1,-1);
    if( _hover )
        painter.fillRect(0, 0, _w - 1, _h - 1, QColor(255,255,255));
    painter.drawText(r, _title);
    painter.drawRect(0, 0, _w -1, _h -1);
    painter.restore();
}

bool NavigateItem::inRange(QPoint pos)
{
    if( _x < pos.x() && pos.x() < _x + _w && _y < pos.y() && pos.y() < _y + _ch ) return true;
    return false;
}

void NavigateItem::mouseClicked()
{
    qDebug() << _title << " clicked";
    setExpand(!_isExpand);
}

void NavigateItem::addAnimation(int t)
{
    _state = Adding;
    _ch = _ch0 = 0;
    _dh = _h - _ch;
    _dh = _dh / (t * 1.0 / TimerInverval);
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::addAnimation: startTimer error";
        _state = Normal;
    }else
    {
        _timerId[_state] = ret;
    }
}

void NavigateItem::expandAnimation(int t)
{
    if(_state == Collapsing)
    {
        killTimer(_timerId[_state]);
    }else
    {
        _cch = _ch;
        _nodeContentHeight = _cch;
    }

    _state = Expanding;
    _timerCnt = 0;
    _lastExpandItem = _childs[0];

    _cth = _nodeExpandHeight;
    _dh = (_cth - _cch);
    _dh = _dh / (t * 1.0 / TimerInverval);
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::expandAnimation: startTimer error";
        _state = Normal;
        return;
    }else
    {
        _timerId[_state] = ret;
    }
    qDebug() << _title << " expandAnimation start" << _dh  << _cch << _cth;
}

void NavigateItem::collapseAnimation(int t)
{
    if(_state == Expanding)
    {
        killTimer(_timerId[_state]);
    }
    _state = Collapsing;
    _timerCnt = 0;
    _dh = _cch / (t * 1.0 / TimerInverval);
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::collapseAnimation: startTimer error";
        _state = Normal;
        return;
    }else
    {
        _timerId[_state] = ret;
    }
    qDebug() << _title << " collapseAnimation start" << _dh << _cch;
}

void NavigateItem::removeAnimation(int t)
{
    internalCollapseChild(this);
    _state = Removing;
    _dh = _ch / (t * 1.0 / TimerInverval);
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::removeAnimation: startTimer error";
        _state = Normal;
    }else
    {
        _timerId[_state] = ret;
    }
    qDebug() << _title << " removeAniamtion start" << _dh << _ch;
}

void NavigateItem::setVisibleLen(NavigateItem *item)
{
    const auto& x = _childsIndex.find(item);
    if(x != _childsIndex.end())
    {
        _visibleLen = x.value() + 1;
    }
}

NavigateItem *NavigateItem::findAnimationAncestor()
{
    NavigateItem* ancestor = _parent;
    NavigateItem* aancestor = ancestor;
    while( ancestor->_parent != nullptr)
    {
        aancestor = ancestor;
        ancestor = ancestor->_parent;
        if(ancestor->_state == Expanding || ancestor->_state == Collapsing)
        {

        }
    }
    return nullptr;
}

void NavigateItem::nodeExpandHeightChanged(int dh)
{
    if(dh == 0) return;
    _nodeExpandHeight += dh;
    //qDebug() << "NavigateItem::nodeExpandHeightChanged: " <<_title << _nodeExpandHeight;
    if(_isExpand == false) return;
    if(_parent)
    {
        _parent->nodeExpandHeightChanged(dh);
    }
}

void NavigateItem::nodeExpandHeightChanged_expcol(int dh)
{
    if(dh == 0) return;
    if(_parent)
        _parent->nodeExpandHeightChanged(dh);
}

void NavigateItem::timerEvent(QTimerEvent *ev)
{
    if(_state == Adding)
    {
        if(_ch >= _h)
        {
            killTimer(_timerId[_state]);
            _state = Normal;
            _ch = _h;
        }else
        {
            _ch0 = _ch;
            _ch += _dh;
        }
    }else if(_state == Expanding)
    {
        if(_cch >= _cth)
        {
            killTimer(_timerId[_state]);
            _state = Normal;
            _cch = _cth;
            nodeExpandHeightChanged_expcol((int)_cch - _h);

            qDebug() << _title << " expandAnimation end";
        }else
        {
            _cch0 = _cch;
            _cch += _dh;
        }

    }else if(_state == Collapsing)
    {
        if(_cch <= _ch)
        {
            killTimer(_timerId[_state]);
            _state = Normal;
            _isExpand = false;
            _cch = _ch;
            nodeExpandHeightChanged_expcol((int)_cch - _nodeExpandHeight);
            qDebug() << _title << " collapseAnimation end";
        }else
        {
            _cch0 = _cch;
            _cch -= _dh;
        }

    }else if(_state == Removing)
    {
        if( _ch <= 0)
        {
            killTimer(_timerId[_state]);
            Q_ASSERT(_parent != nullptr);
            _ch0 = _ch = 0;
            _parent->removeChild(this);

            qDebug() << "removeAnimation end";

        }else
        {
            _ch0 = _ch;
            _ch -= _dh;
        }

    }
    emit updateTree(this);

}

void NavigateItem::setExpand(bool isExpand)
{
    if(_childs.size() == 0) return;
    if(_state == Collapsing)
    {
        float t =  (_cth - _cch) / _dh + 1;
        expandAnimation(t * 1.0 * TimerInverval);
        return;
    }
    if(isExpand)
    {
        _isExpand = isExpand;
        //internalExpandParent(this);
        emit expanded(this);
    }else {
        //internalCollapseChild(this);
        emit collapsed(this);
    }
}

int NavigateItem::updateOffset(int off)
{
    _nodeContentHeight = internalUpdateOffset(off, this);
    return _nodeContentHeight;
}

int NavigateItem::updateOffset(int off, int &rh)
{
    float x;
    int ret;
    Q_ASSERT(_state == Normal);
    x = rh;
    ret = internalUpdateOffset(off, x, this);

    _nodeContentHeight = ret;
    return ret;

}

int NavigateItem::updateOffset_delta(int &rh)
{
    float x;
    float ret = 0;
    float dh = 0;
    if(_state == Expanding )
    {
        dh = _cch - _cch0;
        x = dh < rh ? dh : rh;
        //Q_ASSERT(dh < rh);
        ret = internalUpdateOffset_delta_exp(x);

        //qDebug() << "expanding ret: " << dh << ret;
        rh -= ret;
    }else if(_state == Collapsing)
    {
        dh = _cch0 - _cch;
        x = dh < rh ? dh : rh;
        //Q_ASSERT(dh < rh);
        ret = internalUpdateOffset_delta_col(x);
        rh -= ret;
        ret *= -1;
    }
    else if(_state == Adding)
    {
        x = _ch < rh ? _ch : rh;
        internalUpdateOffset(this->_contentOffset, x, this);
        ret = _ch - _ch0;
        rh -= _ch;
    }
    else if(_state == Removing)
    {
        if(_rflag == false)
        {
            ret = _ch - _nodeContentHeight;
            _rflag = true;
        }
        x = _ch < rh ? _ch : rh;
        internalUpdateOffset(this->_contentOffset, x, this);
        ret += (_ch - _ch0);
        rh -= _ch;
    }
    else
    {
        x = rh;
        ret = internalUpdateOffset(_contentOffset, x, this);
        ret = ret - _nodeContentHeight;
        rh -= ret;
        //x = ret = 0;
    }
    _nodeContentHeight += ret;
    //qDebug() << _title << _nodeContentHeight << ret << x << rh << dh;
    return _nodeContentHeight;
}

// int NavigateItem::nodeHeight()
// {
//     return internalNodeHeight(this);
// }

void NavigateItem::addChild(NavigateItem *item)
{
    addChild(item, _childs.size());
}

void NavigateItem::addChild(NavigateItem *item, int rank)
{
    if(rank > _childs.size()) rank = _childs.size();

    item->_parent = this;

    if(_childs.size() == 0)
    {
        item->_next = nullptr;
    }else
    {
        if(rank == 0 )
        {
            item->_next = _childs[0];
        }else if(rank == _childs.size())
        {
            item->_next = nullptr;
            _childs[rank -1]->_next = item;
        }else
        {
            _childs[rank - 1]->_next = item;
            item->_next = _childs[rank];
        }
    }
    _childs.insert(rank, item);
    _childsIndex.insert(item, rank);
    for(int i = rank + 1; i < _childs.size(); i++)
    {
        const auto& x = _childsIndex.find(_childs[i]);
        if(x != _childsIndex.end())
        {
            _childsIndex[_childs[i]] += 1;
        }
    }
    // if(_isExpand)
    //     _visibleLen = _childs.size();
    nodeExpandHeightChanged(_childs[rank]->_h);
}

void NavigateItem::removeChild(int i)
{
    if( i >= _childs.size() || i < 0 ) return;
    _childs[i]->_isDelete = true;
    if(i > 0)
    {
        _childs[i-1]->_next = _childs[i]->_next;
    }

    nodeExpandHeightChanged(0 - _childs[i]->_h);
    if(i < _visibleLen)
    {
        _visibleLen -= 1;
    }

    _childsIndex.remove(_childs[i]);
    _childs.remove(i);

    for(int r = i; r < _childs.size(); r++)
    {
        const auto& x = _childsIndex.find(_childs[r]);
        if(x != _childsIndex.end())
            _childsIndex[_childs[r]] -= 1;
    }

    // _visibleLen = _childs.size();

}

void NavigateItem::removeChild(NavigateItem *item)
{
    const auto& x = _childsIndex.find(item);
    Q_ASSERT(x != _childsIndex.end());
    return removeChild(x.value());
}

void NavigateItem::init()
{
    _hover = false;
    _isDelete = false;
    _contentOffset = 0;
    _childs.clear();
    _isExpand = false;
    _lastExpandItem = _next = _parent = nullptr;
    _w = _h = _ch = 0;
    _x = _y = -1;
    _ch0 = 0;
    _cth = _cch = 0;
    _visibleLen = 0;
    _dh = 0;
    _state = Normal;
    _nodeContentHeight = _nodeExpandHeight = 0;
    _timerCnt = 0;
    _rflag = false;
}

int NavigateItem::internalUpdateOffset(int off, NavigateItem *root)
{
    int h = 0;
    root->_contentOffset = off;
    root->_ch = root->_h;
    h += root->_h;
    if(root->_isExpand == false) return h;
    for( auto& x: root->_childs)
    {
        h += internalUpdateOffset(off + h, x);
    }
    root->_visibleLen = root->_childs.size();
    return h;
}

int NavigateItem::internalUpdateOffset(int off, float &rh, NavigateItem *root)
{
    int h = 0, i = 0;
    if(rh <= 0 ) return h;
    root->_contentOffset = off;
    if(rh >= root->_h)
    {
        root->_ch = root->_h;
    }else
    {
        root->_ch = rh;
    }
    h += root->_ch;
    rh -= root->_ch;
    if(root->_isExpand == false) return h;
    for( i = 0; i < root->_childs.size() && rh > 0; i++)
    {
        h += internalUpdateOffset(off + h, root->_childs[i]);
    }
    root->_visibleLen = i;
    return h;
}

float NavigateItem::internalUpdateOffset_delta_exp(float &rh)
{
    NavigateItem* curExpandItem = _lastExpandItem;
    int i = 0;
    float h = 0, dh;
    if(rh <= 0) return h;
    //_lastUpdatedItem->_contentOffset = off;
    if( curExpandItem->_ch + rh >= curExpandItem->_h )
    {
        dh = curExpandItem->_h - curExpandItem->_ch;
        curExpandItem->_ch = curExpandItem->_h;
        rh -= dh;
        h += dh;
        curExpandItem->_parent->_visibleLen = curExpandItem->_parent->_childsIndex[curExpandItem] + 1;

        if(curExpandItem->_isExpand == false)
        {
            if(curExpandItem->_next != nullptr)
            {
                _lastExpandItem = curExpandItem->_next;
            }else
            {
                if(curExpandItem->_parent != this)
                {
                    _lastExpandItem = curExpandItem->_parent->_next;
                }else
                {
                    return h;
                }
            }
            _lastExpandItem->_contentOffset = curExpandItem->_contentOffset + curExpandItem->_ch;
            h += internalUpdateOffset_delta_exp(rh);

        }else
        {
            for( i = 0; i < curExpandItem->_childs.size() && rh > 0; i++)
            {
                _lastExpandItem = curExpandItem->_childs[i];
                _lastExpandItem->_contentOffset = curExpandItem->_contentOffset + h;
                h += internalUpdateOffset_delta_exp(rh);
            }
            curExpandItem->_visibleLen = i;
        }

    }else
    {
        curExpandItem->_ch += rh;
        h += rh;
        rh -= rh;
        curExpandItem->_parent->_visibleLen = curExpandItem->_parent->_childsIndex[curExpandItem] + 1;
    }
    return h;
}

float NavigateItem::internalUpdateOffset_delta_col(float &rh)
{
    NavigateItem* curExpandItem = _lastExpandItem;
    NavigateItem* parent = curExpandItem->_parent;
    int i = 0;
    float h = 0, dh;
    if(rh <= 0) return h;
    //_lastUpdatedItem->_contentOffset = off;
    if( curExpandItem->_ch - rh <= 0 )
    {
        dh = curExpandItem->_ch;
        curExpandItem->_ch = 0;
        rh -= dh;
        h += dh;
        parent->_visibleLen = parent->_childsIndex[curExpandItem];

        if(parent->_childsIndex[curExpandItem] == 0)
        {
            if(parent != this)
            {
                _lastExpandItem = parent;
            }else
            {
                return h;
            }
        }
        else
        {
            i = parent->_childsIndex[curExpandItem];
            _lastExpandItem = parent->_childs[i - 1];
        }
        h += internalUpdateOffset_delta_col(rh);

    }else
    {
        curExpandItem->_ch -= rh;
        h += rh;
        rh -= rh;
        curExpandItem->_parent->_visibleLen = curExpandItem->_parent->_childsIndex[curExpandItem] + 1;
    }
    return h;
}

int NavigateItem::internalNodeHeight(NavigateItem *root)
{
    int h = 0;
    h += root->height();
    if(root->_isExpand == false ) return h;
    for( auto& x: root->_childs)
    {
        h += internalNodeHeight(x);
    }
    return h;
}

void NavigateItem::internalExpandParent(NavigateItem *item)
{
    while(item->_parent != nullptr)
    {
        item->nodeExpandHeightChanged_expcol(item->_nodeExpandHeight - item->_h);
        item->_parent->_isExpand = true;
        item = item->_parent;
    }
}

void NavigateItem::internalCollapseChild(NavigateItem *item)
{
    if( item->_isExpand == false ) return;
    item->nodeExpandHeightChanged_expcol(item->_h - item->_nodeExpandHeight);
    item->_isExpand = false;
    for(auto& x: item->_childs)
    {
        internalCollapseChild(x);
    }
}

void NavigateItem::internalDeleteChild()
{
//    qDebug() << this ->_title << " deleted..";
    for(auto& x: _childs)
    {
        delete x;
    }
}
