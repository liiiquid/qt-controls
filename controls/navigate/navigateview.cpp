#include "navigateview.h"
#include <QResizeEvent>
#include <QDebug>
#include <qdatetime.h>
#define OPERATION_EXPAND 1
#define OPERATION_COLLAPSE 2
#define OPERATION_SETSIZE  3

const int AnimationDuration = 250;
const int PageLoadRatio = 0;

NavigateView::NavigateView(QWidget *parent)
    : QWidget{parent}
{
    _itemSize = QSize(100, 30);
    _contentHeight = _lastContentOffset = 0;
    _initOk = false;
    _operations.clear();
    _lastShow.clear();
    _hoverItem = nullptr;
    _root = new NavigateItem("root",this);
    _root->_contentOffset = -_itemSize.height();
    _root->setFixedSize(QSize(0, 0));
    _root->_isExpand = true;
    _preloadPageHeight = _yof = 0;
    setAttribute(Qt::WA_Hover);
}

void NavigateView::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    NavigateItem* lastItem = nullptr;
    int st_y = 0;
    QColor color = palette().color(QPalette::Window);
    painter.fillRect(rect(), color);

    for(auto& x: _lastShow)
    {
        x->draw(painter);
    }
    if(_lastShow.size() > 0)
    {
        lastItem = _lastShow.last();
        st_y = lastItem->_y + lastItem->_ch;
    }

    painter.fillRect(0, st_y, width(), height() - st_y, color);

    //painter.drawRect(0, 0, width() - 1, height() - 1 );


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

    int x = (width() - _itemSize.width()) >> 1;
    int y = -d1 + _yof;
    _yof = 0;
    int di = height() + d1;

    updateSubView(st_item, x, y, di);
    while (di > 0 && st_item->_next != nullptr ) {
        updateSubView(st_item->_next, x, y, di);
        st_item = st_item->_next;
    }
    NavigateItem* parent = st_item;

    while(parent->_parent != nullptr && di > 0)
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



void NavigateView::onScrolled(int contentOffset)
{
    if(contentOffset < 0)
    {
        _yof = -contentOffset;
        contentOffset = 0;
    }
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
        qDebug() << st_item->_title << st_item->_ch << st_item->_contentOffset << contentOffset << d1;
        Q_ASSERT(d1 <= (int)st_item->_ch);
        if(NavigateItem::_lastUpdatedItem != NavigateItem::_lastLastUpdatedItem)
        {
            if( (NavigateItem::_lastUpdatedItem->_contentOffset - st_item->_contentOffset) <= _preloadPageHeight >> PageLoadRatio)
            {
                auto x = NavigateItem::_lastUpdatedItem->findAnimationAncestor_nearest_neg(NavigateItem::Normal);
                if( x == nullptr )
                {
                    //qDebug() << NavigateItem::_lastUpdatedItem->_title << "updating...";
                    updateContentOffset(NavigateItem::_lastUpdatedItem, NavigateItem::_lastUpdatedItem->_contentOffset, _preloadPageHeight);
                    //qDebug() << "updated...";
                }
            }
            NavigateItem::_lastLastUpdatedItem = NavigateItem::_lastUpdatedItem;
        }

    }
    updateView(st_item, d1);
}

int NavigateView::updateContentOffset(NavigateItem *item, int off, int rh)
{
#ifdef DEBUG
    NavigateItem* t = item;
#endif
    float d = 0;
    d = _root->_nodeContentHeight;
    Q_ASSERT(item != nullptr);
    // the delta value of height of the item is the delta value of _contentHeight
    // update the item.
    item->_contentOffset = off;
    off += item->updateOffset_delta(rh);
    // check the sis of item.
    while(item->_next != nullptr && rh > 0)
    {
        item = item->_next;
        item->_contentOffset = off;
        off += item->updateOffset_delta(rh);
    }

    // upwards until reach the top level, which is the child of _root.
    while( item->_parent != _root && rh > 0)
    {
        item = item->_parent;
        while(item->_next != nullptr && rh > 0)
        {
            item = item->_next;
            item->_contentOffset = off;
            off += item->updateOffset_delta(rh);
        }
    }

    // check the top level item in the range of rh.
    while(item->_next != nullptr && rh > 0 )
    {
        item = item->_next;
        item->_contentOffset = off;
        off += item->updateOffset_delta(rh);
    }
    if(item->_state == NavigateItem::Normal)
    {
        while(item->_visibleLen > 0)
        {
            item = item->_childs[item->_visibleLen - 1];
        }
    }

    NavigateItem::_lastUpdatedItem = item;

    d = _root->_nodeContentHeight - d;
    _contentHeight = qRound(_root->_nodeContentHeight);
    if( d != 0)
    {
        emit heightChanged(_contentHeight, height());
    }
#ifdef DEBUG
    qDebug() << "NavigateView::updateContentOffset:"<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")<<":";
    if(NavigateItem::_lastUpdatedItem)
        qDebug() << "NavigateView::updateContentOffset: lastItem:" << NavigateItem::_lastUpdatedItem->_title <<NavigateItem::_lastUpdatedItem->_contentOffset << NavigateItem::_lastUpdatedItem->_nodeContentHeight;

    qDebug() << "NavigateView::updateContentOffset:" << "sender:" << t->_title << "current contentHeight:" << _root->_nodeContentHeight << "," << d << " height changed..";
    printInfor(_root);
#endif
    return _contentHeight;
}

void NavigateView::printInfor(NavigateItem* root)
{

    qDebug() << "NavigateView::printInfor:" << root->_title << root->_contentOffset << root->_visibleLen << root->_ch << root->_h << root->_nodeContentHeight << root->_nodeExpandHeight;
    if(root->_isExpand == false) return;
    for(auto&x : root->_childs)
    {
        printInfor(x);
    }

}

void NavigateView::setAllItemSize(NavigateItem *root)
{
    root->setFixedSize(_itemSize);
    for(auto& x: root->_childs)
    {
        setAllItemSize(x);
    }
}


NavigateItem *NavigateView::search(int contentOffset)
{
    Q_ASSERT(contentOffset >= 0);
    NavigateItem* ret;
    ret = internalSearch(contentOffset, _root->_childs, _root->_visibleLen);
    if(ret == nullptr )
    {
        qDebug() << "NavigateView::search: " << "first-level internal search meets nullptr";
        return nullptr;
    }
    if(ret->_isExpand == true && ret->_visibleLen > 0)
    {
        //(ret->_contentOffset + ret->_ch) >= contentOffset
        while(  (contentOffset - ret->_contentOffset) >= ret->_ch && ret->_visibleLen > 0)
        {
            ret = internalSearch(contentOffset, ret->_childs, ret->_visibleLen);
            if( ret->_isExpand == false) break;
        }
    }
    return ret;
}

void NavigateView::setExpand(NavigateItem *item, bool isExpand)
{
    if(item == nullptr) return;
    if(_initOk == false)
    {
        if(isExpand == true)
        {
            auto x = _operations.find(OPERATION_EXPAND);
            if(x != _operations.end())
            {
                x.value().push_back(item);
            }else
            {
                _operations.insert(OPERATION_EXPAND, QVector<NavigateItem*>(1, item));
            }
        }
        else
        {
            auto x = _operations.find(OPERATION_COLLAPSE);
            if(x != _operations.end())
            {
                x.value().push_back(item);
            }else
            {
                _operations.insert(OPERATION_COLLAPSE, QVector<NavigateItem*>(1, item));
            }
        }
        return;
    }
    item->setExpand(isExpand);
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

            x->mouseClicked(ev);
            break;
        }
    }
}

void NavigateView::mouseReleaseEvent(QMouseEvent *)
{

}


void NavigateView::onReachBottom()
{

}

void NavigateView::onUpdateTree(NavigateItem *item)
{
    updateContentOffset(item, item->_contentOffset, _preloadPageHeight);
    // static int i = 0;
    // qDebug() << "NavigateView::onUpdateTree(NavigateItem *item)" << ": " << item->_title << item->_contentOffset << ", " << i++;
}

void NavigateView::onExpanded(NavigateItem *item, int contentOffset)
{

    qDebug() << "NavigateView::onExpanded funciton called";

    Q_ASSERT(item->_parent != nullptr && item->_parent->_isExpand == true);
    item->expandAnimation(AnimationDuration);
}

void NavigateView::onCollapsed(NavigateItem *item, int contentOffset)
{

    item->collapseAnimation(AnimationDuration);

}

void NavigateView::setItemHeight(int h)
{
    _itemSize.setHeight(h);
    if(_initOk == false)
    {
        _operations.insert(OPERATION_SETSIZE, QVector<NavigateItem*>(1, _root));
        return;
    }
    qDebug() << "NavigateView::setItemHeight funciton called";
    _root->_contentOffset = -_itemSize.height();
    if(_root->_childs.size() > 0)
    {
        for(auto& x: _root->_childs)
        {
            setAllItemSize(x);
        }
        qDebug() << "NavigateView::setItemHeight funciton called ... 2";
        updateContentOffset(_root->_childs[0], 0, _preloadPageHeight);
    }

}

void NavigateView::resizeEvent(QResizeEvent *ev)
{
    _preloadPageHeight = height() << PageLoadRatio;
    _itemSize.setWidth( width() - 6 );
    if(ev->oldSize().height() != ev->size().height())
    {
        emit heightChanged(_contentHeight, height());
    }
    if(ev->oldSize().width() != ev->size().width())
    {
        emit widthChanged(0, width());
    }
    if(_initOk == false)
    {
        _initOk = true;
        _root->updateOffset(-_root->_h, _preloadPageHeight);

        auto x = _operations.find(OPERATION_SETSIZE);
        if( x != _operations.end())
        {
            setItemHeight(_itemSize.height());
        }

        x = _operations.find(OPERATION_EXPAND);
        if(x != _operations.end())
        {
            for(auto& z : x.value())
            {
                setExpand(z, true);
            }
        }

        x = _operations.find(OPERATION_COLLAPSE);
        if(x != _operations.end())
        {
            for(auto& z : x.value())
            {
                if(z->_state == NavigateItem::Expanding)
                    setExpand(z, false);
            }
        }
        _operations.clear();
    }
}

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
    if(rank > parent->_childs.size()) rank = parent->_childs.size();
    //item->_parent = parent;
    connect(item, &NavigateItem::updateTree, this, &NavigateView::onUpdateTree);

    parent->addChild(item, rank);

    item->setParent(this);
    item->setFixedSize(_itemSize);
    item->_ch = 0;
    item->_bgcolor = palette().color(QPalette::Window);

    if( parent->_childs.size() == 0 || rank == 0)
    {
        off = parent->_contentOffset + _itemSize.height();
    }else
    {
        off = parent->_childs[rank-1]->_contentOffset + parent->_childs[rank-1]->_nodeContentHeight;
    }

    item->_contentOffset = off;

    if(parent->_isExpand == false) return;
    if(_initOk == false)
        updateContentOffset(item, off, _preloadPageHeight);
    else
    {
        if(_lastShow.size() > 0 )
        {
            if(item->_contentOffset > _lastShow.first()->_contentOffset + height() + _itemSize.height())
            {
                updateContentOffset(item, off, _preloadPageHeight);
                return;
            }
        }
        item->addAnimation(AnimationDuration);
    }
}

void NavigateView::removeItem(NavigateItem *parent, int i)
{
    if(parent == nullptr) parent = _root;
    if(i >= parent->_childs.size()) return;
    if(i == -1) i = parent->_childs.size() - 1;
    // 1. collapse all the childs of the current node
    // 2. remove the current node from the array, mark it for deleteion.
    // 3. update content offset
    parent->_childs[i]->remove(AnimationDuration);
}
