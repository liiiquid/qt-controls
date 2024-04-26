#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QBoxLayout>
#include "controls/navigate/navigateentry.h"
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
protected:
    void resizeEvent(QResizeEvent* ev) override;

private:
    NavigateEntry* _navigator;
private:
    void initLayout(QBoxLayout *layout);
signals:
};

#endif // MAINWINDOW_H
