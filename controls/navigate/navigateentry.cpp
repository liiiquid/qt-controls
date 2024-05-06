#include "navigateentry.h"
const int angleDelta = 60;
NavigateEntry::NavigateEntry(QWidget *parent)
    : QWidget(parent)
{
    initView();
    initScrollBar();
    initLayout();
}

void NavigateEntry::addItem(const QString &title)
{
    NavigateItem* item = new NavigateItem(title, _view);
    addItem(item);
}

void NavigateEntry::addItem(NavigateItem *item)
{
    connect(item, &NavigateItem::collapsed, this, &NavigateEntry::onCollapsed);
    connect(item, &NavigateItem::expanded, this, &NavigateEntry::onExpanded);
    _view->addItem(nullptr, item);
}

void NavigateEntry::addItem(NavigateItem *item, int rank)
{
    connect(item, &NavigateItem::collapsed, this, &NavigateEntry::onCollapsed);
    connect(item, &NavigateItem::expanded, this, &NavigateEntry::onExpanded);
    _view->addItem(nullptr, item, rank);
}
void NavigateEntry::addItem(NavigateItem *parent, NavigateItem *item)
{
    if(parent == nullptr ) {addItem(item); return; }
    connect(item, &NavigateItem::collapsed, this, &NavigateEntry::onCollapsed);
    connect(item, &NavigateItem::expanded, this, &NavigateEntry::onExpanded);
    _view->addItem(parent, item);
}

void NavigateEntry::addItem(NavigateItem *parent, NavigateItem *item, int rank)
{
    if(parent == nullptr ) {addItem(item, rank); return; }

    connect(item, &NavigateItem::collapsed, this, &NavigateEntry::onCollapsed);
    connect(item, &NavigateItem::expanded, this, &NavigateEntry::onExpanded);
    _view->addItem(parent, item, rank);
}

void NavigateEntry::removeItem(int i)
{
    _view->removeItem(nullptr, i);
}

void NavigateEntry::removeItem(NavigateItem *parent, int i)
{
    _view->removeItem(parent, i);
}

void NavigateEntry::scroll(int step)
{
    _scrollBar->scroll(-step);
}

void NavigateEntry::setItemSize(QSize size)
{
    _view->setItemHeight(size.height());
}

void NavigateEntry::setExpand(NavigateItem *item, bool isExpand)
{
    _view->setExpand(item, isExpand);
}

void NavigateEntry::initView()
{
    _view = new NavigateView(this);

}

void NavigateEntry::initScrollBar()
{
    _scrollBar = new ScrollBar(this);
    _scrollBar->_scrollDelta = angleDelta;
    _scrollBar->_barWPct = 0.5;
}

void NavigateEntry::initLayout()
{
    _hLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    _scrollBar->setFixedWidth(15);
    _hLayout->addWidget(_view);
    _hLayout->addWidget(_scrollBar);
    _hLayout->setSpacing(0);
    connect(_scrollBar, &ScrollBar::scrolled, _view, &NavigateView::onScrolled);
    connect(_scrollBar, &ScrollBar::reachBottom, _view, &NavigateView::onReachBottom);

    connect(this, &NavigateEntry::collapsed, _view, &NavigateView::onCollapsed);
    connect(this, &NavigateEntry::expanded, _view, &NavigateView::onExpanded);

    connect(_view, &NavigateView::heightChanged, this, &NavigateEntry::onHeightChanged);


}

void NavigateEntry::onHeightChanged(int contentHeight, int viewHeight)
{
    _scrollBar->setHeight(contentHeight, viewHeight);
}

void NavigateEntry::wheelEvent(QWheelEvent *ev)
{
    if(_view->_initOk == false) return;
    if(_scrollBar->_isReachBound) return;
    // Upwards
    if( ev->angleDelta().y() > 0)
        _scrollBar->scroll( _scrollBar->_scrollDelta );
    // Downwards
    else
        _scrollBar->scroll(-_scrollBar->_scrollDelta);
}

void NavigateEntry::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    //painter.drawRect(0, 0, width() -1, height() - 1);
}

void NavigateEntry::timerEvent(QTimerEvent *ev)
{

}
