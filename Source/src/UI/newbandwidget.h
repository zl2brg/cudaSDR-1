#ifndef NEWBANDWIDGET_H
#define NEWBANDWIDGET_H

#include <QWidget>

namespace Ui {
class NewBandWidget;
}

class NewBandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewBandWidget(QWidget *parent = nullptr);
    ~NewBandWidget();

private:
    Ui::NewBandWidget *ui;
};

#endif // NEWBANDWIDGET_H
