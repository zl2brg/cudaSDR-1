#ifndef NOISEFILTERWIDGET_H
#define NOISEFILTERWIDGET_H

#include <QWidget>
#include "Util/cusdr_buttons.h"
#include "cusdr_settings.h"
#include "cusdr_fonts.h"


namespace Ui {
class NoiseFilterWidget;
}

class NoiseFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoiseFilterWidget(QWidget *parent = 0);
    ~NoiseFilterWidget();

private:
    Ui::NoiseFilterWidget *ui;
    Settings	*set;
    QSDR::_ServerMode			m_serverMode;
    QSDR::_HWInterfaceMode		m_hwInterface;
    QSDR::_DataEngineState		m_dataEngineState;

    QList<TReceiver>	m_rxDataList;

    CFonts		*fonts;
    TFonts		m_fonts;


    int		m_minimumWidgetWidth;
    int		m_minimumGroupBoxWidth;
    int		m_btnSpacing;
    int		m_fontHeight;
    int		m_maxFontWidth;
    int		m_currentReceiver;
    bool	m_mouseOver;

    void	setupConnections();


private slots:
    void	systemStateChanged(
            QObject *sender,
            QSDR::_Error err,
            QSDR::_HWInterfaceMode hwmode,
            QSDR::_ServerMode mode,
            QSDR::_DataEngineState state);

    void	setCurrentReceiver(QObject *sender, int rx);


public slots:
    QSize	sizeHint() const;
    QSize	minimumSizeHint() const;



};

#endif // NOISEFILTERWIDGET_H
