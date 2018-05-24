#include "noisefilterwidget.h"
#include "ui_noisefilterwidget.h"

#define	btn_height		15
#define	btn_width		70
#define	btn_widths		32
#define	btn_width2		52
#define	btn_width3		60



NoiseFilterWidget::NoiseFilterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoiseFilterWidget)
{
    setContentsMargins(4, 0, 4, 0);

    ui->setupUi(this);
    ui->snbCheckBox->setStyleSheet(set->getLabelStyle());
    ui->anfCheckBox->setStyleSheet(set->getLabelStyle());
    ui->anfCheckBox->setFont(QFont("Arial", 8));

    ui->label->setFrameStyle(QFrame::Box | QFrame::Raised);
    ui->label->setStyleSheet(set->getLabelStyle());
    ui->label->setFont(QFont("Arial", 8));

    ui->label_2->setFrameStyle(QFrame::Box | QFrame::Raised);
    ui->label_2->setStyleSheet(set->getLabelStyle());
    ui->label_2->setFont(QFont("Arial", 8));

    ui->label_3->setFrameStyle(QFrame::Box | QFrame::Raised);
    ui->label_3->setStyleSheet(set->getLabelStyle());
    ui->label_3->setFont(QFont("Arial", 8));

    ui->label_4->setFrameStyle(QFrame::Box | QFrame::Raised);
    ui->label_4->setStyleSheet(set->getLabelStyle());
    ui->label_4->setFont(QFont("Arial", 8));

    ui->label_5->setFrameStyle(QFrame::Box | QFrame::Raised);
    ui->label_5->setStyleSheet(set->getLabelStyle());
    ui->label_5->setFont(QFont("Arial", 8));

    ui->nbModeComboBox->setFont(QFont("Arial", 8));
    ui->nbModeComboBox->setStyleSheet(set->getComboBoxStyle());
    ui->nrModeComboBox->setFont(QFont("Arial", 8));
    ui->nrModeComboBox->setStyleSheet(set->getComboBoxStyle());

    ui->nr2GainComboBox->setFont(QFont("Arial", 8));
    ui->nr2GainComboBox->setStyleSheet(set->getComboBoxStyle());

    ui->mmseCheckBox->setFont(QFont("Arial", 8));
    ui->mmseCheckBox->setStyleSheet(set->getLabelStyle());

    ui->omsCheckBox->setFont(QFont("Arial", 8));
    ui->omsCheckBox->setStyleSheet(set->getLabelStyle());

    ui->postAGCCheckBox->setFont(QFont("Arial", 8));
    ui->postAGCCheckBox->setStyleSheet(set->getLabelStyle());

    ui->preAGCCheckBox->setFont(QFont("Arial", 8));
    ui->preAGCCheckBox->setStyleSheet(set->getLabelStyle());

    ui->nr2aeCheckBox->setFont(QFont("Arial", 8));
    ui->nr2aeCheckBox->setStyleSheet(set->getLabelStyle());

//    ui->frame->setFrameStyle(1);

}

NoiseFilterWidget::~NoiseFilterWidget()
{
    delete ui;
}

QSize NoiseFilterWidget::sizeHint() const {

    return QSize(m_minimumWidgetWidth, height());
}

QSize NoiseFilterWidget::minimumSizeHint() const {

    return QSize(m_minimumWidgetWidth, height());
}



void NoiseFilterWidget::setCurrentReceiver(QObject *sender, int rx) {

}

void NoiseFilterWidget::systemStateChanged(
        QObject *sender,
        QSDR::_Error err,
        QSDR::_HWInterfaceMode hwmode,
        QSDR::_ServerMode mode,
        QSDR::_DataEngineState state)
{
    Q_UNUSED (sender)
    Q_UNUSED (err)

    if (m_hwInterface != hwmode)
        m_hwInterface = hwmode;


    if (m_serverMode != mode)
        m_serverMode = mode;

    if (m_dataEngineState != state)
        m_dataEngineState = state;

    update();
}
