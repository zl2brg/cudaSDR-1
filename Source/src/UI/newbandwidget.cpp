#include "newbandwidget.h"
#include "ui_newbandwidget.h"

NewBandWidget::NewBandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewBandWidget)
{
    ui->setupUi(this);
}

NewBandWidget::~NewBandWidget()
{
    delete ui;
}
