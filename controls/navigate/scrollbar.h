#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <QWidget>
#include <QtMath>
#include <QWheelEvent>
#include <QPainter>
#include <QDebug>



class ScrollBar : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollBar(QWidget *parent = nullptr);

public:
    // the bar width as a percentage of overall control width
    float _barWPct;
    // scaling factor to control the scroll speed
    int _scrollFactor;
public:
    // the offset of the first control displayed in the view from the overall content height.
    int _contentOffset;

private:
    int _barHeight;
    int _barWidth;
    int _barx;
    int _bary;
private:
    // total height of all contents
    int _contentHeight;
    // view height that show content
    int _viewHeight;
    // _contentHeight / this->height()
    float _scrollRatio;

public:
    void scroll(int step);

public:
    void setHeight(int contentHeight, int viewHeight);
protected:
    void paintEvent(QPaintEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override {};
    void mousePressEvent(QMouseEvent* ev) override {};
    void mouseReleaseEvent(QMouseEvent* ev) override {};
    void wheelEvent(QWheelEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

private:
    void drawVerticalScroll(QPainter& painter);

    void updateBar();
    void scroll();
signals:
    void scrolled(int contentOffset);
    void reachBottom();
};

#endif // SCROLLBAR_H
