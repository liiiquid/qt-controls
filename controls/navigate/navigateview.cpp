#include "navigateview.h"
#include <QResizeEvent>
#include <QDebug>
#define OPERATION_EXPAND 1
#define OPERATION_COLLAPSE 2
const int AnimationDuration = 250;
NavigateView::NavigateView(QWidget *parent)
    : QWidget{parent}
{
    //_allItem.clear();
    _itemSize = QSize(100, 30);
    _contentHeight = 0;
    _initOk = false;
    _operations.clear();
    _lastShow.clear();
    _hoverItem = nullptr;
    _root = new NavigateItem("root",this);
    _root->_contentOffset = -_itemSize.height();
    _root->_isExpand = true;
    setAttribute(Qt::WA_Hover);
}

void NavigateView::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    NavigateItem* lastItem = nullptr;
    int st_y;
    QColor color = palette().color(QPalette::Window);
    painter.fillRect(rect(), color);


    for(auto& x: _lastShow)
    {

        x->draw(painter);
    }
    lastItem = _lastShow.last();
    st_y = lastItem->_y + lastItem->_ch + 1;
    painter.fillRect(0, st_y, width(), height() - st_y, color);

    painter.drawRect(0, 0, width() - 1, height() - 1 );


}
void NavigateView::updateView(NavigateItem* st_item, int d1)
{
    for(auto& x:_lastShow)
    {
        if(x->_isDelete)
        {
            //qDebug() << x->_title << " deleted..";
        }
    }
    _lastShow.resize(0);
    if(st_item == nullptr)
    {
        update();
        return;
    }

    int h = _itemSize.height();
    int x = (width() - _itemSize.width()) >> 1;
    int y = -d1;
    int di = height() * 2;

    updateSubView(st_item, x, y, di);
    while (di > 0 && st_item->_next != nullptr ) {
        updateSubView(st_item->_next, x, y, di);
        st_item = st_item->_next;
    }
    NavigateItem* parent = st_item;
    while(parent->_parent != _root && di > 0)
    {
        parent = parent->_parent;
        while(parent->_next != nullptr && di > 0)
        {
            updateSubView(parent->_next, x, y, di);
            parent = parent->_next;
        }
    }
    update();
}

void NavigateView::updateSubView(NavigateItem *item, int x, int &y, int& rh)
{
    int i = 0;
    if(rh <= 0) return;
    item->setFixedSize(_itemSize);
    item->move(x, y);
    item->show();
    _lastShow.append(item);
    y += item->_ch;
    rh -= item->_ch;
    if( item->_isExpand == false) return;
    for(i = 0; i < item->/*_childs.size()*/_visibleLen && rh > 0; i++)
    {
        updateSubView(item->_childs[i], x, y, rh);
    }
}

// void NavigateView::updateSubView(NavigateItem *item, int x, int &y, int& cnt)
// {

//     if(cnt == 0) return;
//     item->setFixedSize(_itemSize);
//     item->move(x, y);
//     item->show();
//     _lastShow.append(item);
//     y += item->height();
//     cnt -= 1;
//     if( item->_isExpand == false) return;
//     for(int i = 0; i < item->_childs.size() && cnt > 0; i++)
//     {
//         updateSubView(item->_childs[i], x, y, cnt);
//     }
// }

// void NavigateView::updateSubView(NavigateItem *item, int x, int &y, int& cnt, NavigateItem *parent)
// {

//     if(cnt == 0) return;
//     item->setFixedSize(_itemSize);
//     item->move(x, y);
//     item->show();
//     if(parent == nullptr)
//         _lastShow.append(item);
//     else{
//         parent->_visibleChilds.push_back(item);
//     }
//     y += item->height();
//     cnt -= 1;
//     if( item->_isExpand == false) return;
//     for(int i = 0; i < item->_childs.size() && cnt > 0; i++)
//     {
//         updateSubView(item->_childs[i], x, y, cnt, item);
//     }

// }


// QVector<NavigateItem*> animation_group;
// void NavigateView::updateView(NavigateItem* st_item, int d1)
// {
//     int h = _itemSize.height();
//     int x = ( width() - _itemSize.width() ) >> 1;
//     int y = -d1;
//     int di = height() / h + 5;

//     for( int i = 0; i <_lastShow.size(); i++)
//     {
//         if( _lastShow[i]->_isDelete)
//         {
//             delete _lastShow[i];
//         }
//         else  _lastShow[i]->hide();
//     }
//     _lastShow.clear();
//     if(st_item == nullptr) return;
//     animation_group.clear();
//     updateSubView(st_item, x, y, di);

//     while (di > 0 && st_item->_next != nullptr ) {
//         updateSubView(st_item->_next, x, y, di);
//         st_item = st_item->_next;
//     }
//     NavigateItem* parent = st_item;

//     while(parent->_parent != nullptr && di > 0)
//     {
//         parent = parent->_parent;
//         while(parent->_next != nullptr && di > 0)
//         {
//             updateSubView(parent->_next, x, y, di);
//             parent = parent->_next;
//         }
//     }
//     int z = animation_group.size() - 1;
//     float p = 500;
//     for(int i = animation_group.size() - 1; i >= 0; i--)
//     {
//         animation_group[i]->_posAnimation->setDuration( -p * 1.0 / z * i + p );
//         animation_group[i]->_posAnimation->start();
//     }
// }


void NavigateView::resizeEvent(QResizeEvent *ev)
{
    onResize();
    if(_initOk == false)
    {
        _initOk = true;
        for(auto& x : _operations)
        {
            switch(x.first)
            {
            case OPERATION_EXPAND:
                onExpanded(x.second, -1);
                break;
            case OPERATION_COLLAPSE:
                break;
            default:
                break;
            }
        }
        _operations.clear();
    }
}


// void NavigateView::addItem(NavigateItem *item)
// {
//     addItem(item, _root->_childs.size());

// }

// void NavigateView::addItem(NavigateItem *item, int rank)
// {

//     int off = 0;

//     item->setParent(this);
//     item->setFixedSize(_itemSize);
//     item->_ch = _itemSize.height();
//     item->_bgcolor = palette().color(QPalette::Window);
//     if(rank > _root->_childs.size()) rank = _root->_childs.size();
//     if(_root->_childs.size() == 0)
//     {
//         item->_next = nullptr;
//         off = 0;
//     }else
//     {
//         if(rank == 0 )
//         {
//             item->_next = _root->_childs[0];
//             off = _root->_childs[0]->_contentOffset;
//         }else if(rank == _root->_childs.size())
//         {
//             item->_next = nullptr;
//             _root->_childs[rank -1]->_next = item;

//             off = _root->_childs[rank-1]->_contentOffset + _itemSize.height();
//         }else
//         {
//             _root->_childs[rank - 1]->_next = item;
//             item->_next = _allItem[rank];

//             off = _allItem[rank]->_contentOffset;
//         }
//     }
//     _allItem.insert(rank, item);
//     connect(item, &NavigateItem::updateTree, this, &NavigateView::onUpdateTree);
//     item->_contentOffset = off;
//     if(_initOk == false)
//         updateContentOffset(item, off);
//     else
//     {
//         if(_lastShow.size() > 0 )
//         {
//             if(item->_contentOffset > _lastShow.first()->_contentOffset + height() + _itemSize.height())
//             {
//                 updateContentOffset(item, off);
//                 return;
//             }
//         }
//         item->addAnimation(0, item->_h, AnimationDuration);

//     }
// }
void NavigateView::addItem(int parentContentOff, NavigateItem *item)
{
    NavigateItem* parent = search(parentContentOff);

    Q_ASSERT(parent != nullptr && parentContentOff == parent->_contentOffset);
    addItem(parent, item);
}

void NavigateView::addItem(NavigateItem *parent, NavigateItem *item)
{
    if(parent == nullptr) parent = _root;
    addItem(parent, item, parent->_childs.size());
}

void NavigateView::addItem(NavigateItem *parent, NavigateItem *item, int rank)
{
    int off = 0;
    if(parent == nullptr) parent = _root;
    item->_parent = parent;
    connect(item, &NavigateItem::updateTree, this, &NavigateView::onUpdateTree);

    item->setParent(this);
    item->setFixedSize(_itemSize);
    item->_ch = _itemSize.height();
    item->_bgcolor = palette().color(QPalette::Window);
    if(rank > parent->_childs.size()) rank = parent->_childs.size();
    if( parent->_childs.size() == 0 || rank == 0)
    {
        off = parent->_contentOffset + _itemSize.height();
    }else
    {
        off = parent->_childs[rank-1]->_contentOffset + parent->_childs[rank-1]->nodeHeight();
    }

    parent->addChild(item, rank);
    item->_contentOffset = off;
    if(_initOk == false)
        updateContentOffset(item, off);
    else
    {
        if(_lastShow.size() > 0 )
        {
            if(item->_contentOffset > _lastShow.first()->_contentOffset + height() + _itemSize.height())
            {
                updateContentOffset(item, off);
                return;
            }
        }
        item->addAnimation(0, item->_h, AnimationDuration);
    }
}

// void NavigateView::removeItem(int i)
// {
//     if(i >= _allItem.size()) return;

//     NavigateItem* item = _allItem[i];

//     item->removeAnimation(_allItem, i, AnimationDuration);
// }

void NavigateView::removeItem(NavigateItem *parent, int i)
{
    if(parent == nullptr) parent = _root;
    if(i >= parent->_childs.size()) return;
    // 1. collapse all the childs of the current node
    // 2. remove the current node from the array, mark it for deleteion.
    // 3. update content offset
    parent->_childs[i]->removeAnimation(AnimationDuration);
}

void NavigateView::onScrolled(int contentOffset)
{
    // contentOffset >= st_item->_contentOffset
    NavigateItem* st_item = search(contentOffset);
    int d1 = 0;
    if(st_item == nullptr)
    {
        qDebug() << "NavigateView::onScrolled(int contentOffset):  st_item == nullptr";
    }else
    {
        Q_ASSERT(contentOffset >= st_item->_contentOffset);
        d1 = contentOffset - st_item->_contentOffset;
        Q_ASSERT(d1 < st_item->height());
    }

    updateView(st_item, d1);
}

void NavigateView::onUpdateTree(NavigateItem *item)
{

    updateContentOffset(item, item->_contentOffset);
    // static int i = 0;
    // qDebug() << "NavigateView::onUpdateTree(NavigateItem *item)" << ": " << item->_title << ", " << i++;
}

void NavigateView::onExpanded(NavigateItem *item, int contentOffset)
{
    if(_initOk == false)
    {
        _operations.push_back(QPair(OPERATION_EXPAND, item));
        return;
    }

    if(item->_parent != nullptr && item->_parent->_isExpand == false) return;
    //updateContentOffset(item, item->_contentOffset);
    item->expandAnimation(AnimationDuration);
}

void NavigateView::onCollapsed(NavigateItem *item, int contentOffset)
{
    if(_initOk == false)
    {
        _operations.push_back(QPair(OPERATION_COLLAPSE, item));
        return;
    }

    //updateContentOffset(item, item->_contentOffset);
    item->collapseAnimation(AnimationDuration);

}

void NavigateView::setItemHeight(int h)
{
    _itemSize.setHeight(h);
    if(_root->_childs.size() > 0)
    {
        setAllItemSize(_root->_childs);
        updateContentOffset(_root->_childs[0], 0);
    }

}

int NavigateView::updateContentOffset(NavigateItem *item, int off)
{
    if(item != nullptr)
    {

        off += item->updateOffset(off);

        while(item->_next != nullptr)
        {
            item = item->_next;
            off += item->updateOffset(off);
        }

        NavigateItem* parent = item;

        while( parent->_parent != _root )
        {
            while(parent->_next != nullptr )
            {
                off += parent->_next->updateOffset(off);
                parent = parent->_next;
            }
            parent = parent->_parent;
        }

        while(parent->_next != nullptr )
        {
            off += parent->_next->updateOffset(off);
            parent = parent->_next;
        }
    }
    _contentHeight = off;
    emit heightChanged(_contentHeight, height());

    return off;
}

void NavigateView::setAllItemSize(const QVector<NavigateItem *> &roots)
{
    for(auto& x: roots)
    {
        x->setFixedSize(_itemSize);
        if(x->_isExpand)
            setAllItemSize(x->_childs);
    }
}

NavigateItem *NavigateView::search(int contentOffset)
{
    Q_ASSERT(contentOffset >= 0);
    NavigateItem* ret;
    ret = internalSearch(contentOffset, _root->_childs);
    if(ret == nullptr )
    {
        qDebug() << "NavigateView::search: " << "first-level internal search meets nullptr";
        return nullptr;
    }
    if(ret->_isExpand == true && ret->_childs.size() > 0)
    {
        //(ret->_contentOffset + ret->_ch) >= contentOffset
        while(  (contentOffset - ret->_contentOffset) >= ret->_ch && ret->_visibleLen > 0 )
        {
            ret = internalSearch(contentOffset, ret->_childs, ret->_visibleLen);
            if( ret->_childs.size() == 0 || ret->_isExpand == false) break;
        }
    }
    return ret;
}

/**
 * @brief search for the item that with the greatest content offset that is not greater than contentOffset param.
 * @param contentOffset the range of contentOffset should be [0, contentHeight].
 * @param roots
 * @return If contentOffset is less than 0, the first element in the root is returned; if contentOffset is greater than contentHeight, the last element in the root is returned.
 * Otherwise, the item with the largest content offset that is not greater than the contentOffset parameter is returned.
 */
NavigateItem *NavigateView::internalSearch(int contentOffset, const QVector<NavigateItem*>& roots, int len)
{
    if(len == 0) return nullptr;

    int l = 0, r = len - 1;
    while( l <= r)
    {
        int m = (l + r) >> 1;
        if(contentOffset >= roots[m]->_contentOffset)
        {
            l = m + 1;
        }else r = m -1;
    }

    Q_ASSERT(r >= 0);
    return roots[r];
}
NavigateItem *NavigateView::internalSearch(int contentOffset, const QVector<NavigateItem *> &roots)
{
    if(roots.size() == 0) return nullptr;

    int l = 0, r = roots.size() - 1;
    while( l <= r)
    {
        int m = (l + r) >> 1;
        if(contentOffset >= roots[m]->_contentOffset)
        {
            l = m + 1;
        }else r = m -1;
    }

    Q_ASSERT(r >= 0);
    return roots[r];
}

void NavigateView::onResize()
{
    _itemSize.setWidth( width() - 6 );
    emit heightChanged(_contentHeight, height());

}

bool NavigateView::event(QEvent *ev)
{
    QHoverEvent* ev1;
    if(ev->type() == QEvent::HoverMove)
    {
        ev1 = (QHoverEvent*)(ev);
        if(_hoverItem)
        {
            if( _hoverItem->inRange(ev1->pos()) == false )
            {
                _hoverItem->setHover(false);
                _hoverItem = nullptr;
                update();
            }
        }
        for(auto&x : _lastShow)
        {
            if( x->inRange(ev1->pos()) )
            {
                if(_hoverItem != x)
                {
                    if(_hoverItem!=nullptr)
                        _hoverItem->setHover(false);
                    _hoverItem = x;
                    _hoverItem->setHover(true);
                    update();
                }
                break;
            }
        }

        ev->accept();
        return true;
    }else if(ev->type() == QEvent::HoverLeave)
    {
        if(_hoverItem)
        {
            _hoverItem->setHover(false);
            update();
            _hoverItem = nullptr;
        }
    }
    return __super::event(ev);
}

void NavigateView::mousePressEvent(QMouseEvent *ev)
{
    for(auto&x : _lastShow)
    {
        if( x->inRange(ev->pos()) )
        {
            x->mouseClicked();
            break;
        }
    }

}


