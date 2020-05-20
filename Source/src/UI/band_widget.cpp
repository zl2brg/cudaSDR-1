#include "band_widget.h"
#include "Util/cusdr_buttons.h"
#include "ui_band_widget.h"


BandWidget::BandWidget (QWidget *parent)
    : baseWidget(parent)
        , m_serverMode(set->getCurrentServerMode())
        , m_hwInterface(set->getHWInterface())
        , m_dataEngineState(set->getDataEngineState())
        //, m_panadapterMode(set->getPanadapterMode())
        //, m_waterColorScheme(set->getWaterfallColorScheme())

        , ui(new Ui::BandWidget)

{

    setContentsMargins(4, 0, 4, 0);
    ui->setupUi(this);
    setStyleSheet(set->getDialogStyle());
   // this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setupModeBtn(ui->bnd_2200m);
    setupModeBtn(ui->bnd_630m);
    setupModeBtn(ui->bnd_160m);
    setupModeBtn(ui->bnd_80m);
    setupModeBtn(ui->bnd_60m);
    setupModeBtn(ui->bnd_40m);
    setupModeBtn(ui->bnd_30m);
    setupModeBtn(ui->bnd_20m);
    setupModeBtn(ui->bnd_17m);
    setupModeBtn(ui->bnd_15m);
    setupModeBtn(ui->bnd_12m);
    setupModeBtn(ui->bnd_10m);
    setupModeBtn(ui->bnd_6m);
    setupModeBtn(ui->bnd_2m);
    setupModeBtn(ui->bnd_125cm);
    setupModeBtn(ui->bnd_70cm);
    setupModeBtn(ui->bnd_33cm);
    setupModeBtn(ui->bnd_23cm);
    setupModeBtn(ui->bnd_13cm);
    setupModeBtn(ui->bnd_10cm);
    setupModeBtn(ui->bnd_5cm);
    setupModeBtn(ui->bnd_gen);




    adjustSize();


}

BandWidget::~BandWidget()
{
    delete ui;
}


void BandWidget::setupConnections() {

}




void BandWidget::systemStateChanged(
        QObject *sender,
        QSDR::_Error err,
        QSDR::_HWInterfaceMode hwmode,
        QSDR::_ServerMode mode,
        QSDR::_DataEngineState state)
{
    Q_UNUSED (sender)
    Q_UNUSED (err)

    m_receiverDataList = set->getReceiverDataList();

    m_hamBand = m_receiverDataList.at(m_receiver).hamBand;
    //m_dspMode = m_receiverDataList.at(m_receiver).dspMode;
    m_dspModeList = m_receiverDataList.at(m_receiver).dspModeList;
    m_lastCtrFrequencyList = m_receiverDataList.at(m_receiver).lastCenterFrequencyList;
    m_lastVfoFrequencyList = m_receiverDataList.at(m_receiver).lastVfoFrequencyList;

    if (m_hwInterface != hwmode)
        m_hwInterface = hwmode;


    if (m_serverMode != mode)
        m_serverMode = mode;

    if (m_dataEngineState != state)
        m_dataEngineState = state;

    update();
}

void BandWidget::getSettings() {

}



void BandWidget::btnCallback() {
    AeroButton *button = qobject_cast<AeroButton *>(sender());
    int btn = btnList.indexOf(button);
    qDebug() << "Button" << btn;
    foreach(AeroButton *btn, btnList) {

        btn->setBtnState(AeroButton::OFF);
        btn->update();
    }

    button->setBtnState(AeroButton::ON);
    button->update();
    qDebug() << "size" << btnList.size();
    set->setHamBand(this, m_receiver, true, (HamBand) btn);

    QString str = button->text();
    if (str == "2200 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m2200));
    else
    if (str == "630 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m630));
    else
    if (str == "160 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m160));
    else
    if (str == "80 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m80));
    else
    if (str == "60 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m60));
    else
    if (str == "40 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m40));
    else
    if (str == "30 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m30));
    else
    if (str == "20 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m20));
    else
    if (str == "17 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m17));
    else
    if (str == "15 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m15));
    else
    if (str == "12 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m12));
    else
    if (str == "10 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m10));
    else
    if (str == "6 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m6));
    else
    if (str == "2 m")
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(m2));

    else
        set->setVFOFrequency(this, 2, m_receiver, m_lastVfoFrequencyList.at(gen));
}


