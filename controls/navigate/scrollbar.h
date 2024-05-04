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

    enum TimerTag{
        TimerAnimation,
        TimerCnt,
    };

public:
    explicit ScrollBar(QWidget *parent = nullptr);

public:
    // the bar width as a percentage of overall control width
    float _barWPct;
    // scaling factor to control the scroll speed
    float _scrollFactor;
public:
    // the offset of the first control displayed in the view from the overall content height.
    float _contentOffset;

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
private:
    // save the value in pixels to be scrolled
    float _r;
    float _r1;
    float _dr;
    int _timerCnt;
private:
    bool _scrollUp;
private:
    int _timerId[TimerCnt];
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
    void timerEvent(QTimerEvent* ev) override;

private:
    void drawVerticalScroll(QPainter& painter);

    void updateBar();
    void requestScroll();
signals:
    void scrolled(int contentOffset);
    void reachBottom();
};

#endif // SCROLLBAR_H
