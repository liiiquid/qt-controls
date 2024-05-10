#include "navigateitem.h"
#include <QDebug>
#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
const int TimerInverval = 5;
NavigateItem* NavigateItem::_lastUpdatedItem = nullptr;
NavigateItem* NavigateItem::_lastLastUpdatedItem = nullptr;

QColor NavigateItem::_bgcolor = QColor("#fff");
QColor NavigateItem::_hovercolor = QColor("#E9F5FF");
QColor NavigateItem::_selcolor= QColor("#C4E4FF");
QColor NavigateItem::_selcolor_font = QColor("#054A99");
QColor NavigateItem::_fontcolor = QColor("#333");
QColor NavigateItem::_tipcolor = QColor("#FF5E54");

NavigateItem::NavigateItem(QObject *parent)
    : QObject{parent}
{
    _tipCnt = 0;
    _title = "Title";
    init();
}

NavigateItem::NavigateItem(const QString &title, QObject *parent)
{
    _tipCnt = 0;
    _title = title;
    init();
}
NavigateItem::NavigateItem(const QString &title, const QString &iconPath, QObject *parent)
    : QObject{parent}
{
    _tipCnt = 0;
    _title = title;
    _icon.load(iconPath);
    //Q_ASSERT( == true);
    init();

}

NavigateItem::~NavigateItem()
{
    // qt will delete them when program exits automatically, need to call internalDeleteChild to delete the node and its children explicitly.
    //internalDeleteChild();
}

bool NavigateItem::inRange(QPoint pos)
{
    if( _x < pos.x() && pos.x() < _x + _w && _y < pos.y() && pos.y() < _y + _ch ) return true;
    return false;
}

void NavigateItem::remove(int t)
{
    if(_state != Normal)
    {
        qDebug() << "NavigateItem::removeChild: " << "the item can not be deleted at this time";
        return;
    }
    _t1 = t;
    _action = Remove;
    if(_isExpand)
        collapseAnimation(_t1);
    else
        removeAnimation(_t1);
}


void NavigateItem::setVisibleLen(NavigateItem *item)
{
    const auto& x = _childsIndex.find(item);
    if(x != _childsIndex.end())
    {
        _visibleLen = x.value() + 1;
    }
}

void NavigateItem::nodeExpandHeightChanged(int dh)
{
    if(dh == 0) return;
    _nodeExpandHeight += dh;
    NavigateItem* p = _parent;
    while( p )
    {
        p->_nodeExpandHeight += dh;
        if( p->_isExpand == false ) return;
        p = p->_parent;
    }
}

void NavigateItem::lastExpandItemChanged(NavigateItem *lastExpandItem)
{
    NavigateItem* p = _parent;
    while(p != nullptr)
    {
        Q_ASSERT(p->_isExpand == true);
        if(p->_lastExpandItem)
        {
            if(p->_lastExpandItem->_contentOffset < lastExpandItem->_contentOffset)
            {
                p->_lastExpandItem = lastExpandItem;
            }
        }else
            p->_lastExpandItem = lastExpandItem;
        p = p->_parent;
    }
}

void NavigateItem::nodeContentHeightChanged(float dh)
{

    if(dh == 0)
    {
        return;
    }
    NavigateItem* p = this;

    while( p != nullptr)
    {
        p->_nodeContentHeight += dh;
        p = p->_parent;
    }
}

void NavigateItem::nodeExpandHeightChanged_expcol(int dh)
{
    NavigateItem* p = _parent;
    if(p)
    {
        Q_ASSERT(p->_isExpand == true);
        p->nodeExpandHeightChanged(dh);
    }
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

            qDebug() << "addAnimation end" << _ch << _nodeExpandHeight << _nodeContentHeight;
        }
        else {
            double d1 = 2 * _dh / _t1;
            _dr =  -d1 / _t1 * _timerCnt[Adding] + d1;
            _ch0 = _ch;
            _ch += _dr;
            _timerCnt[Adding] += 1;
            if(_ch >= _h)
            {
                _ch = _h;
            }
            //qDebug() << _dh << _ch << _dr << _timerCnt[Adding];
        }
    }else if(_state == Expanding)
    {
        if(_cch >= _cth && _rotateAngle >= 90)
        {
            killTimer(_timerId[_state]);
            _state = Normal;
            _cch = _cth;
            if(_isAllExpanded == false)
            {
                nodeExpandHeightChanged_expcol(_cch - _h);
                _isAllExpanded = true;
            }

            _rotateAngle = 90;

            //qDebug() << _lastExpandItem->_title;
            qDebug() << _title << " expandAnimation end" << _cch - _h << QDateTime::currentMSecsSinceEpoch() - _tt;
        }
        else {
            double d1 = 2 * _dh / _t1;
            _dr =  -d1 / _t1 * _timerCnt[Expanding] + d1;
            _cch0 = _cch;
            _cch += _dr;
            _timerCnt[Expanding] += 1;

            _rotateAngle += _r1 / _t1;

            if(_cch >= _cth)
            {
                _cch = _cth;
            }
            if(_rotateAngle >= 90)
            {
                _rotateAngle = 90;
            }
        }

    }else if(_state == Collapsing)
    {
        if(_cch <= _ch && _rotateAngle <= 0)
        {
            killTimer(_timerId[_state]);
            _timerId[_state] = 0;
            _state = Normal;
            _isExpand = false;
            _cch = _ch;
            if(_isAllExpanded == true)
            {
                nodeExpandHeightChanged_expcol(_cch - _nodeExpandHeight);
                _isAllExpanded = false;
            }

            _rotateAngle = 0;
            qDebug() << _title << " collapseAnimation end";


            if(_action == Remove)
            {
                removeAnimation(_t1 * TimerInverval);
            }
        }
        else {
            double d1 = 2 * _dh / _t1;
            _dr =  -d1 / _t1 * _timerCnt[Expanding] + d1;
            _cch0 = _cch;
            _cch -= _dr;
            _rotateAngle -= _r1 / _t1;
            _timerCnt[Expanding] -= 1;
            if(_cch <= _ch)
            {
                _cch = _ch;
            }

            if(_rotateAngle <= 0)
            {
                _rotateAngle = 0;
            }
        }

    }else if(_state == Removing)
    {
        if(_ch <= 0)
        {
            killTimer(_timerId[_state]);
            Q_ASSERT(_parent != nullptr);
            _ch = 0;
            _parent->removeChild(this);
            qDebug() << "removeAnimation end";
            return;
        }
        else {
            double d1 = 2 * _dh / _t1;
            _dr =  -d1 / _t1 * _timerCnt[Adding] + d1;
            _ch0 = _ch;
            _ch -= _dr;
            _timerCnt[Adding] -= 1;
            if( _ch <= 0)
            {
                _ch = 0;
            }
            //qDebug() << _dh << _ch << _dr << _t1 << _timerCnt[Adding];
        }
    }
    emit updateTree(this);
}

void NavigateItem::addAnimation(int t)
{
    if(_state != Normal) return;
    _parent->setVisibleLen(this);
    _state = Adding;
    _ch = 0;
    _dh = _h - _ch;
    _t1 = t / TimerInverval;
    _timerCnt[Adding] = 0;
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::addAnimation: startTimer error";
        _state = Normal;
    }else
    {
        _timerId[_state] = ret;
    }
    qDebug() << "addAnimation start.. " << _dh << _t1;
}

void NavigateItem::expandAnimation(int t)
{
    if(_state == Collapsing)
    {
        killTimer(_timerId[_state]);
    }else if(_state == Normal)
    {
        _cch0 = _cch = _ch;
        _cth = _nodeExpandHeight;
        _dh = _cth - _cch;
        _t1 = t / TimerInverval;
        _timerCnt[Expanding] = 0;
        if(_childs.size() <= 0)
        {
            _lastExpandItem = nullptr;
        }else
        {
            _lastExpandItem = _childs[0];
            _lastExpandItem->_contentOffset = _contentOffset + _ch;
            lastExpandItemChanged(_lastExpandItem);
        }

        _r1 = 90;
        // _dh = (_cth - _cch);
        // _dh = _dh / ( t * 1.0 / TimerInverval);
    }else
    {
        qDebug() <<  "NavigateItem::expandAnimation: State other than Collapsing, Normal met";
        return;
    }

    _state = Expanding;

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
    _tt = QDateTime::currentMSecsSinceEpoch();
    qDebug() << _title << " expandAnimation start" << _dh << _cch << _cth;
}

void NavigateItem::collapseAnimation(int t)
{
    if(_state == Expanding)
    {
        killTimer(_timerId[_state]);
    }else if(_state == Normal)
    {
        _cch0 = _cch = _nodeExpandHeight;
        _dh = _cch - _ch;
        _t1 = t / TimerInverval;
        _r1 = 90;
    }else
    {
        qDebug() << "NavigateItem::collapseAnimation: State other than Expanding, Normal met";
        return;
    }

    _state = Collapsing;

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
    _state = Removing;
    _ch0 = _ch;
    _dh = _ch;
    _t1 = t / TimerInverval;
    int ret = startTimer(TimerInverval);
    _timerCnt[Adding] = _t1;
    if(ret == 0)
    {
        qDebug() << "NavigateItem::removeAnimation: startTimer error";
        _state = Normal;
    }else
    {
        _timerId[_state] = ret;
    }
    qDebug() << _title << " removeAniamtion start" << _dh << _ch << _ch0;
}

void NavigateItem::setExpand(bool isExpand)
{
    //if(_childs.size() <= 0) return;
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

int NavigateItem::updateOffset(int off, int &rh)
{
    float x;
    int ret;
    Q_ASSERT(_state == Normal);
    x = rh;
    ret = internalUpdateOffset(off, x, this);

    //_nodeContentHeight = ret;
    return ret;

}

int NavigateItem::updateOffset_delta(int &rh)
{
    float x;
    float ret = 0;
    float dh = 0;
    if(_state == Expanding )
    {
        if(_cch != _cch0)
        {
            dh = _cch - _cch0;

            x = dh < rh ? dh : rh;
            Q_ASSERT(dh < rh);
            ret = internalUpdateOffset_delta_exp(x);
            rh -= ret;
            _cch0 = _cch;
        }

    }else if(_state == Collapsing)
    {
        if(_cch != _cch0)
        {
            dh = _cch - _cch0;
            x = qAbs(dh) < qAbs(rh) ? qAbs(dh) : qAbs(rh);
            Q_ASSERT(dh < rh);
            ret = internalUpdateOffset_delta_col(x);
            rh -= ret;
            ret *= -1;
            _cch0 = _cch;
        }

    }
    else if(_state == Adding)
    {
        // x = _ch < rh ? _ch : rh;
        // internalUpdateOffset(_contentOffset, x, this);
        // ret = _dh;
        // rh -= _dh;
        if(_ch0 != _ch)
        {
            nodeContentHeightChanged(_ch - _ch0);
            _ch0 = _ch;
        }

    }
    else if(_state == Removing)
    {
        //x = _ch < rh ? _ch : rh;
        //ret = internalUpdateOffset(this->_contentOffset, x, this);
        if(_ch0 != _ch)
        {
            nodeContentHeightChanged(_ch - _ch0);
            _ch0 = _ch;
        }

    }
    else
    {
        //qDebug() << "Normal";
        x = rh;
        ret = internalUpdateOffset(_contentOffset, x, this);
        rh = x;
    }
    Q_UNUSED(ret);
    return qRound(_nodeContentHeight);
}

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
}

void NavigateItem::removeChild(NavigateItem *item)
{
    const auto& x = _childsIndex.find(item);

    Q_ASSERT(x != _childsIndex.end());

    return removeChild(x.value());
}

void NavigateItem::init()
{
    _hover = _selected = false;
    _isDelete = false;
    _contentOffset = 0;
    _childs.clear();
    _isExpand = false;
    _lastExpandItem = _next = _parent = nullptr;
    _w = _h = _ch = 0;
    _x = _y = -1;
    _cth = _cch = 0;
    _rotateAngle = 0;
    _visibleLen = 0;
    _dh = 0;
    _state = Normal;
    _action = Non;
    _nodeContentHeight = _nodeExpandHeight = 0;
    _isAllExpanded = false;
    _tipCnt = _toIndex = INT_MIN;

    for(int i = 0; i < AnimationCnt; i++)
    {
        _timerCnt[i] = 0;
        _timerId[i] = 0;
    }
}

int NavigateItem::internalUpdateOffset(int off, float &rh, NavigateItem *root)
{
    float h = 0, i = 0;
    float oc = 0;
    if(rh <= 0 ) return h;
    root->_contentOffset = off;
    if(root->_ch == root->_h)
    {
        oc = root->_ch;
    }else
    {
        if(rh >= root->_h)
        {
            oc = root->_ch;
            root->_ch = root->_h;
        }else
        {
            oc = root->_ch;
            root->_ch = rh;
        }
        rh -= root->_ch;
    }
    h += root->_ch;
    if(root->_parent)
        root->_parent->setVisibleLen(root);
    root->nodeContentHeightChanged(root->_ch - oc);
    if(root->_isExpand == false) return h;
    for( i = 0; i < root->_childs.size() && rh > 0; i++)
    {
        h += internalUpdateOffset(off + h, rh, root->_childs[i]);
    }
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
        curExpandItem->_ch += dh;
        curExpandItem->nodeContentHeightChanged(dh);
        rh -= dh;
        h += dh;
        //curExpandItem->_parent->_visibleLen = curExpandItem->_parent->_childsIndex[curExpandItem] + 1;
        curExpandItem->_parent->setVisibleLen(curExpandItem);
        if(curExpandItem->_isExpand == false)
        {
            if(curExpandItem->_next != nullptr)
            {
                _lastExpandItem = curExpandItem->_next;
            }else
            {
                // No consideration for descendant nodes
                if(curExpandItem->_parent != this)
                {
                    NavigateItem* p = curExpandItem->_parent->_next;
                    if(p)
                    {
                        _lastExpandItem = p;
                    }else return h;
                }else
                {
                    return h;
                }
            }
            _lastExpandItem->_contentOffset = curExpandItem->_contentOffset + curExpandItem->_ch;
            lastExpandItemChanged(_lastExpandItem);
            h += internalUpdateOffset_delta_exp(rh);

        }else
        {
            for( i = 0; i < curExpandItem->_childs.size() && rh > 0; i++)
            {
                _lastExpandItem = curExpandItem->_childs[i];
                _lastExpandItem->_contentOffset = curExpandItem->_contentOffset + curExpandItem->_ch;
                lastExpandItemChanged(_lastExpandItem);                
                h += internalUpdateOffset_delta_exp(rh);
            }
            curExpandItem->_visibleLen = i;
        }

    }else
    {
        curExpandItem->_ch += rh;
        curExpandItem->nodeContentHeightChanged(rh);
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
    //qDebug() << "current collapse item" << curExpandItem->_title;
    int i = 0;
    float h = 0, dh;
    if(rh <= 0) return h;
    //_lastUpdatedItem->_contentOffset = off;
    if( curExpandItem->_ch - rh <= 0 )
    {
        dh = curExpandItem->_ch;
        curExpandItem->_ch = 0;
        curExpandItem->nodeContentHeightChanged(-dh);
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
            if(_lastExpandItem->_isExpand)
            {
                Q_ASSERT(_lastExpandItem->_visibleLen == _lastExpandItem->_childs.size());
                while(_lastExpandItem->_visibleLen > 0)
                {
                    _lastExpandItem = _lastExpandItem->_childs[_lastExpandItem->_visibleLen - 1];
                }
            }
        }
        //lastExpandItemChanged(_lastExpandItem);
        h += internalUpdateOffset_delta_col(rh);

    }else
    {
        curExpandItem->_ch -= rh;
        curExpandItem->nodeContentHeightChanged(-rh);
        h += rh;
        rh -= rh;
        curExpandItem->_parent->_visibleLen = curExpandItem->_parent->_childsIndex[curExpandItem] + 1;
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
    if(item->_isExpand == false) return;
    item->_isExpand = false;
    item->nodeContentHeightChanged(item->_h - item->_nodeContentHeight);
    item->nodeExpandHeightChanged(item->_h - item->_nodeExpandHeight);
    item->_visibleLen = 0;
}

void NavigateItem::internalDeleteChild()
{
//    qDebug() << this ->_title << " deleted..";
    for(auto& x: _childs)
    {
        delete x;
    }
}

void NavigateItem::saveContext()
{

}

NavigateItem *NavigateItem::findAnimationAncestor_nearest(AnimatedState state)
{
    NavigateItem* ret = this;
    while(ret != nullptr)
    {
        if(ret->_state == state)
        {
            return ret;
        }
        ret = ret->_parent;
    }
    return ret;
}

NavigateItem *NavigateItem::findAnimationAncestor_nearest_neg(AnimatedState state)
{
    NavigateItem* ret = this;
    while(ret != nullptr)
    {
        if(ret->_state != state)
        {
            return ret;
        }
        ret = ret->_parent;
    }
    return ret;
}
