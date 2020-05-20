#ifndef MINIMODEWIGDET_H
#define MINIMODEWIGDET_H

#include <QWidget>
#include "Util/cusdr_buttons.h"
#include "cusdr_settings.h"
#include "cusdr_fonts.h"
#include "basewidget.h"


namespace Ui {
    class  MiniModeWidget;
}

class MiniModeWidget : public baseWidget
{
Q_OBJECT

public:
    MiniModeWidget(QWidget *parent = 0);
    ~MiniModeWidget();

private:
    Ui::MiniModeWidget *ui;
    QSDR::_ServerMode			m_serverMode;
    QSDR::_HWInterfaceMode		m_hwInterface;
    QSDR::_DataEngineState		m_dataEngineState;

    QList<TReceiver>	m_rxDataList;
    void    getSettings();
    CFonts	*fonts;
    TFonts	m_fonts;
    void	setupConnections();

public slots:
    void    modeChange();
    void    ModebtnCallback();
    void    BandbtnCallback();
    void    FilterbtnCallback();

static  void    test();

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



#endif // MINIMODEWIGDET_H
