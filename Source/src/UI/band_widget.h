#ifndef BANDWIDGET_H
#define BANDWIDGET_H

#include <QWidget>
#include "Util/cusdr_buttons.h"
#include "basewidget.h"
#include "cusdr_settings.h"
#include "cusdr_fonts.h"


namespace Ui {
    class BandWidget;
}

class BandWidget : public baseWidget {
Q_OBJECT

public:
    BandWidget(QWidget *parent = 0);
    ~BandWidget();

private:
    Ui::BandWidget *ui;

    QSDR::_ServerMode			m_serverMode;
    QSDR::_HWInterfaceMode		m_hwInterface;
    QSDR::_DataEngineState		m_dataEngineState;

    QList<TReceiver>	m_rxDataList;
    void        getSettings();
    CFonts		*fonts;
    TFonts		m_fonts;


    void	setupConnections();
    HamBand m_hamBand;

public slots:
    void btnCallback();

private slots:
    void	systemStateChanged(
            QObject *sender,
            QSDR::_Error err,
            QSDR::_HWInterfaceMode hwmode,
            QSDR::_ServerMode mode,
            QSDR::_DataEngineState state);
/*
    void	setCurrentReceiver(QObject *sender, int rx);
    void    nfModeChanged(int value);
    void    nbModeChanged(int value);
    void    nr2GainChanged(int value);
    void    agcProcChanged(int value);
    void    snbChanged(bool value);
    void    anfChanged(bool value);
    void    nr2aeChanged(bool value);
    void    omsChanged(bool value);
    void    mmseChanged(bool value);
    void    preAgcChanged(bool value);
    void    postAgcChanged(bool nalue);
    void    npeModeChanged(int value);
*/


};



#endif // BANDWIDGET_H
