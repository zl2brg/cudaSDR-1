
/**
* @file  qtdsp_dspEngine.h
* @brief header file for QtDSP
* @author Hermann von Hasseln, DL3HVH
* @version 0.1
* @date 2012-04-07
*/

/*   
 *   Copyright (C) 2007, 2008, 2009, 2010 Philip A Covington, N8VB
 *
 *	 adapted for QtDSP by (C) 2012 Hermann von Hasseln, DL3HVH
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _QTWDSP_DSP_ENGINE_H
#define _QTWDSP_DSP_ENGINE_H

#define AGCOFFSET -18.0//-63.0

//#include <QObject>
//#include <QThread>
//#include <QMetaType>
//#include <QMutexLocker>
//#include <QMutex>
//#include <QWaitCondition>
//#include <QVariant>
//#include <QElapsedTimer>
extern "C" {
#include    <wdsp.h>
}

#include "../cusdr_settings.h"
#include "QtDSP/qtdsp_qComplex.h"
#include "QtDSP/qtdsp_powerSpectrum.h"
#include "QtDSP/qtdsp_signalMeter.h"


#ifdef LOG_WDSP_ENGINE
#define  WDSP_ENGINE_DEBUG qDebug().nospace() << "WDSPEngine::\t"
#else
#   define WDSP_ENGINE_DEBUG nullDebug()
#endif

#define min(x,y) (x<y?x:y)
#define max(x,y) (x<y?y:x)


class QWDSPEngine : public QObject {

	Q_OBJECT

public:
	QWDSPEngine(QObject* parent = 0, int rx = 0, int size = 0);
	~QWDSPEngine();


	void	processDSP(CPX &in, CPX &out,  int size);

	double	getSMeterInstValue();
    void    init_analyzer(int refreshrate);
    void    calcDisplayAveraging();
    int     spectrumDataReady;
    qVectorFloat spectrumBuffer;

public slots:
	bool getQtDSPStatus() { return m_qtdspOn; }
	
	void setNCOFrequency(int rx, long value);
	void setSampleRate(QObject *sender, int value);
	void setSampleSize(int rx, int size);
	void setQtDSPStatus(bool value);
	void setVolume(float value);
	void setDSPMode(DSPMode mode);
	void setAGCMode(AGCMode mode);
    void setFilter(double  low,double high);
	void	setAGCMaximumGain(qreal);
	void	setAGCHangThreshold(qreal);
	//void	setAGCHangLeveldBLine(qreal value);
	//void	setAGCThresholdLine(QObject *sender, int rx, qreal value);
	void	setAGCLineValues(QObject *sender, int rx);
	void 	setAGCThreshold( double threshold);
	void	setAGCHangTime(int hang);
	void 	setAGCHangLevel(double level);
	void    setAGCAttackTime(qreal value);
    void    setAGCDecayTime(qreal value);
    void    setAGCSlope(qreal value);
    void 	setFramesPerSecond(QObject* sender, int rx, int value);
	void    setPanAdaptorAveragingMode( int rx, int value);
    void    setPanAdaptorDetectorMode( int rx, int value);
    void    setPanAdaptorAveragingCnt(QObject*, int rx , int value);

private:
	Settings*	set;
	TReceiver	m_rxData;
	AGCMode		m_agcMode;
	PowerSpectrum*		spectrum;
	QList<PowerSpectrum* >	powerSpectraList;
	CPX		tmp1CPX;
	CPX		tmp2CPX;
	cpx		osc1cpx;


	QMutex	m_mutex;

	bool	m_qtdspOn;

	int		m_rx;
	int		m_size;
	int		m_spectrumSize;
	int		m_samplerate;
	int		m_fftMultiplier;
	int     m_refreshrate;
	int     m_averageCount;
    double  m_display_avb;
    int     m_display_average;
    int     m_PanAvMode;
    int     m_PanDetMode;
   	float	m_volume;
   	int     m_agcSlope;
   	qreal   m_agcThreshold;
   	qreal   m_agcHangThreshold;
   	qreal   m_agcMaximumGain;
   	qreal   m_agcAttackTime;
   	qreal   m_agcDecayTime;
   	DSPMode	m_dspmode;
	qreal	m_NcoFreq;
	qreal	m_NcoInc;
	qreal	m_NcoTime;
	qreal	m_CWoffset;
	qreal	m_OscCos;
	qreal	m_OscSin;
	//qreal	m_calOffset;

	void	ProcessFrequencyShift(CPX &in, CPX &out, int size);
	void	setupConnections();


private slots:

};

#endif // _QTDSP_DSP_ENGINE_H
