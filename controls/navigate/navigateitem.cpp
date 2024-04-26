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

void NavigateItem::addAnimation(int sh, int eh, int t)
{



    _state = Adding;
    _ch = sh;
    _h = eh;
    _dh = eh - sh;
    _dh = _dh / (t * 1.0 / TimerInverval);
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::addAnimation: startTimer error";
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
    }
    _state = Expanding;

    _cth = nodeHeight();
    _dh = (_cth - _cch);
    _dh = _dh / (t * 1.0 / TimerInverval);
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::expandAnimation: startTimer error";
        return;
    }else
    {
        _timerId[_state] = ret;
    }
    qDebug() << _title << " expandAnimation start" << _dh  << _cch;
}

void NavigateItem::collapseAnimation(int t)
{
    if(_state == Expanding)
    {
        killTimer(_timerId[_state]);
    }
    _state = Collapsing;
    _dh = _cch / (t * 1.0 / TimerInverval);
    int ret = startTimer(TimerInverval);
    if(ret == 0)
    {
        qDebug() << "NavigateItem::collapseAnimation: startTimer error";
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
    }else
    {
        _timerId[_state] = ret;
    }
    qDebug() << _title << " removeAniamtion start" << _dh << _ch;
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
            _ch += _dh;
        }
    }else if(_state == Expanding)
    {
        if((int)_cch >= _cth)
        {
            killTimer(_timerId[_state]);
            _state = Normal;
            _cch = _cth;
            qDebug() << _title << " expandAnimation end";
        }else
        {
            _cch += _dh;
        }
    }else if(_state == Collapsing)
    {
        if((int)_cch <= _ch)
        {
            killTimer(_timerId[_state]);
            _state = Normal;
            _isExpand = false;
            _cch = _ch;
            qDebug() << _title << " collapseAnimation end";
        }else
        {
            _cch -= _dh;
        }
    }else if(_state == Removing)
    {
        if((int)_ch <= 0)
        {
            killTimer(_timerId[_state]);
            Q_ASSERT(_parent != nullptr);
            _parent->removeChild(this);
            qDebug() << "removeAnimation end";
            return;

        }else
        {
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
    if(_state == Expanding || _state == Collapsing)
    {
        int cch = _cch;
        return internalUpdateOffset(off, cch, this);
    }else if(_state == Adding || _state == Removing)
    {
        float x = _ch;
        return internalUpdateOffset(off,x, this);
    }
    else return internalUpdateOffset(off, this);

}

int NavigateItem::nodeHeight()
{
    return internalNodeHeight(this);
}

void NavigateItem::addChild(NavigateItem *item)
{
    addChild(item, _childs.size());
}

void NavigateItem::addChild(NavigateItem *item, int rank)
{
    if(rank > _childs.size()) rank = _childs.size();
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
    _visibleLen = _childs.size();
}

void NavigateItem::removeChild(int i)
{
    if( i >= _childs.size() || i < 0 ) return;
    _childs[i]->_isDelete = true;
    if(i > 0)
    {
        _childs[i-1]->_next = _childs[i]->_next;
    }
    NavigateItem* ret = _childs[i];
    _childs.remove(i);

    _visibleLen = _childs.size();

}

void NavigateItem::removeChild(NavigateItem *item)
{
    int i = 0;
    for( i = 0; i < _childs.size(); i++)
    {
        if(_childs[i] == item)
        {
            break;
        }
    }
    return removeChild(i);
}

void NavigateItem::init()
{
    _hover = false;
    _isDelete = false;
    _contentOffset = 0;
    _childs.clear();
    _isExpand = false;
    _parent = nullptr;
    _w = _h = _ch = 0;
    _x = _y = -1;
    _cth = _cch = 0;
    _visibleLen = 0;
    _dh = 0;
    _state = Normal;
}

int NavigateItem::internalUpdateOffset(int off, NavigateItem *root)
{
    int h = 0;
    root->_contentOffset = off;
    h += root->height();
    if(root->_isExpand == false) return h;
    for( auto& x: root->_childs)
    {
        h += internalUpdateOffset(off + h, x);
    }
    root->_visibleLen = root->_childs.size();
    return h;
}

int NavigateItem::internalUpdateOffset(int off, int &rh, NavigateItem* root)
{
    int h = 0, i = 0;
    if(rh <= 0) return h;
    root->_contentOffset = off;
    if( rh >= root->_h)
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
        h += internalUpdateOffset(off + h, rh, root->_childs[i]);
    }
    if(i == 0)
    {
        root->_visibleLen = 0;
    }else root->_visibleLen = i;
    return h;
}

int NavigateItem::internalUpdateOffset(int off, float &rh, NavigateItem *root)
{
    int h = 0, i = 0;
    if(rh <= 0) return h;
    root->_contentOffset = off;
    if( rh >= root->_h)
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
        h += internalUpdateOffset(off + h, rh, root->_childs[i]);
    }
    if(i == 0)
    {
        root->_visibleLen = 0;
    }else root->_visibleLen = i;
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
        item->_parent->_isExpand = true;
        item = item->_parent;
    }
}

void NavigateItem::internalCollapseChild(NavigateItem *item)
{
    if( item->_isExpand == false ) return;
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
