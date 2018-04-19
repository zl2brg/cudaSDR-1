
/**
* @file  qtwdsp_dspEngine.cpp
* @brief QtWDSP DSP engine class
* @author Hermann von Hasseln, DL3HVH
* @version 0.1
* @date 2012-04-07
*/

/*   
 *   Copyright (C) 2007, 2008, 2009, 2010 Philip A Covington, N8VB
 *
 *	 adapted for QtDSP by (C) 2012 Hermann von Hasseln, DL3HVH
 *
 *   The ProcessFrequencyShift method is adpated from cuteSDR by (C) Moe Wheatley, AE4JY.
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
#define LOG_WDSP_ENGINE

// use: WDSP_ENGINE_DEBUG << "debug message";

#include "qtwdsp_dspEngine.h"


double wmyLog(double x, double base) {

	return log(x) / log(base);
}

QWDSPEngine::QWDSPEngine(QObject *parent, int rx, int size)
	: QObject(parent)
	, set(Settings::instance())
	, m_qtdspOn(false)
	, m_rx(rx)
	, m_size(size)
	, m_samplerate(set->getSampleRate())
	, m_fftMultiplier(1)
	, m_volume(0.0f)

{
    int result;
    qRegisterMetaType<QVector<cpx> >();
	qRegisterMetaType<CPX>();
    m_refreshrate = set->getFramesPerSecond(m_rx);
    m_averageCount= set->getSpectrumAveragingCnt(m_rx);
    m_PanAvMode = set->getPanAveragingMode(m_rx);
    m_PanDetMode = set->getPanDetectorMode(m_rx);
    m_agcSlope = set->getAGCSlope(m_rx);
    m_agcMaximumGain = set->getAGCMaximumGain_dB(m_rx);
	m_agcHangThreshold = set->getAGCHangThreshold(m_rx);
	spectrumBuffer.resize(BUFFER_SIZE*4);


	setNCOFrequency(m_rx, m_rxData.vfoFrequency - m_rxData.ctrFrequency);
	WDSP_ENGINE_DEBUG << "init DSPEngine with size: " << m_size;
	SleeperThread::msleep(100);

	setupConnections();

    WDSP_ENGINE_DEBUG << "Opening WDSP channel" << m_rx << "m_size=" << m_size << "Sample rate=" << m_samplerate;

    OpenChannel(m_rx,
                m_size,
                4096, // ,
                m_samplerate,
                48000, // dsp rate
                48000, // output rate
                0, // receive
                0, // run
                0.010, 0.025, 0.0, 0.010, 0);

	SetRXAMode(m_rx, 1);
	RXASetNC(m_rx,4096);
    SetRXAPanelRun(m_rx, 1);
    SetRXAPanelSelect(m_rx,3);
    XCreateAnalyzer(m_rx, &result,4096, 1, 1, "");
    if(result != 0) {
        WDSP_ENGINE_DEBUG <<  "XCreateAnalyzer id=%d failed: %d\n" <<  result;
    }
	init_analyzer(m_refreshrate);
    calcDisplayAveraging();
    SetDisplayAvBackmult(rx, 0, m_display_avb);
    SetDisplayNumAverage(rx, 0, m_display_average);
    SetDisplayDetectorMode(rx,0,m_PanDetMode);
    SetDisplayAverageMode(rx,0,m_PanAvMode);

    SetChannelState(m_rx,1,0);




}



QWDSPEngine::~QWDSPEngine() {
    CloseChannel(m_rx);
	tmp1CPX.clear();
	tmp2CPX.clear();


	if (!powerSpectraList.empty()) {

		powerSpectraList.clear();
	}



	//if (agc)
	//	delete agc;

}

void QWDSPEngine::setupConnections() {

	CHECKED_CONNECT(
			set,
			SIGNAL(ncoFrequencyChanged(int, long)),
			this,
			SLOT(setNCOFrequency(int, long)));
	CHECKED_CONNECT(
			set,
			SIGNAL(sampleSizeChanged(int, int)),
			this,
			SLOT(setSampleSize(int, int)));
	CHECKED_CONNECT(
			set,
			SIGNAL(framesPerSecondChanged(QObject*, int, int)),
			this,
			SLOT(setFramesPerSecond(QObject*, int, int)));


    CHECKED_CONNECT(
            set,
            SIGNAL(framesPerSecondChanged(QObject*, int, int)),
            this,
            SLOT(setFramesPerSecond(QObject*, int, int)));


    CHECKED_CONNECT(
            set,
            SIGNAL(panAveragingModeChanged( int, int)),
            this,
            SLOT(setPanAdaptorAveragingMode( int, int)));


    CHECKED_CONNECT(
            set,
            SIGNAL(panDetectorModeChanged( int, int)),
            this,
            SLOT(setPanAdaptorDetectorMode( int, int)));

    CHECKED_CONNECT(
            set,
            SIGNAL(spectrumAveragingCntChanged(QObject*, int , int)),
            this,
            SLOT(setPanAdaptorAveragingCnt(QObject*, int, int)));



//	CHECKED_CONNECT(
//		wpagc,
//		SIGNAL(hangLeveldBLineChanged(qreal)),
//		this,
//		SLOT(setAGCHangLeveldBLine(qreal)));
//
//	CHECKED_CONNECT(
//		wpagc,
//		SIGNAL(minimumVoltageChanged(QObject *, int, qreal)),
//		this,
//		SLOT(setAGCThresholdLine(QObject *, int, qreal)));


}



void QWDSPEngine::processDSP(CPX &in, CPX &out, int size) {

int error;
	m_mutex.lock();
    fexchange0(m_rx, (double *) in.data(),  (double *) out.data(), &error);

    if(error!=0) {
        WDSP_ENGINE_DEBUG << "WDSP channel read error " << error;
    }

//	int idx = (int)(wmyLog(m_fftMultiplier, 2));
//	powerSpectraList.at(idx)->ProcessSpectrum(in, size * m_fftMultiplier, m_fftMultiplier*2-1);
	Spectrum0(1, m_rx, 0, 0,(double *) in.data());
    GetPixels(0,0,(float *) spectrumBuffer.data(), &spectrumDataReady);
	m_mutex.unlock();
}

double QWDSPEngine::getSMeterInstValue() {

    return  GetRXAMeter(m_rx,RXA_ADC_PK);

}

void QWDSPEngine::setVolume(float value) {
    SetRXAPanelGain1(m_rx,  (double) value);
    WDSP_ENGINE_DEBUG << "WDSP volume set to " <<value;


}

void QWDSPEngine::setQtDSPStatus(bool value) { 
	
	m_qtdspOn = value; 
}

void QWDSPEngine::setDSPMode(DSPMode mode) {

	WDSP_ENGINE_DEBUG << "WDSP mode set to " << mode;
	SetRXAMode(m_rx, mode);

}

void QWDSPEngine::setAGCMode(AGCMode agc) {



		SetRXAAGCMode(m_rx, agc);
		//SetRXAAGCThresh(rx->id, agc_thresh_point, 4096.0, rx->sample_rate);
		SetRXAAGCSlope(m_rx,m_agcSlope);
		SetRXAAGCTop(m_rx,m_agcMaximumGain);
		switch(agc) {
			case agcOFF:
				break;
			case agcLONG:
				SetRXAAGCAttack(m_rx,2);
				SetRXAAGCHang(m_rx,2000);
				SetRXAAGCDecay(m_rx,2000);
				SetRXAAGCHangThreshold(m_rx,(int) m_agcHangThreshold);
				break;
			case agcSLOW:
				SetRXAAGCAttack(m_rx,2);
				SetRXAAGCHang(m_rx,1000);
				SetRXAAGCDecay(m_rx,500);
				SetRXAAGCHangThreshold(m_rx,(int)m_agcHangThreshold);
				break;
			case agcMED:
				SetRXAAGCAttack(m_rx,2);
				SetRXAAGCHang(m_rx,0);
				SetRXAAGCDecay(m_rx,250);
				SetRXAAGCHangThreshold(m_rx,100);
				break;
			case agcFAST:
				SetRXAAGCAttack(m_rx,2);
				SetRXAAGCHang(m_rx,0);
				SetRXAAGCDecay(m_rx,50);
				SetRXAAGCHangThreshold(m_rx,100);
				break;
		}

 WDSP_ENGINE_DEBUG << "Set AGC Mode " << agc;

}

void QWDSPEngine::setAGCMaximumGain(qreal value) {
	SetRXAAGCTop(m_rx, value);
	m_agcMaximumGain = value;
    WDSP_ENGINE_DEBUG << "Set AGCMaximum gain " << value;
}

void QWDSPEngine::setAGCHangThreshold(qreal value) {

	m_agcHangThreshold = value;
	SetRXAAGCHangThreshold(m_rx, (int) value);SetRXAAGCHangThreshold(m_rx, (int) value);
   	WDSP_ENGINE_DEBUG << "Set AGC Hang Threshold " << value;

}

void QWDSPEngine::setAGCLineValues(QObject *sender, int rx, qreal thresh, qreal hang) {
    if (m_rx != rx) return;

//    qreal noiseOffset = 10.0 * log10(qAbs(filter->filterHi() - filter->filterLo()) * 2 * m_size / m_samplerate);
//    qreal threshold = 20.0 * log10(thresh) - noiseOffset + AGCOFFSET;

    SetRXAAGCHangLevel(m_rx,hang);
   WDSP_ENGINE_DEBUG << "Set AGC line value" << hang;

}


void QWDSPEngine::setAGCHangLevel(double level) {

	SetRXAAGCHangLevel(m_rx,level);
	WDSP_ENGINE_DEBUG << "Set AGC line value" << level;
}


void QWDSPEngine::setAGCThreshold(double threshold) {

	SetRXAAGCThresh(m_rx,threshold,4096,this->m_samplerate);
	WDSP_ENGINE_DEBUG << "Set AGC threshold " << threshold;
}

void QWDSPEngine::setAGCHangTime(int value) {

	SetRXAAGCHang(m_rx,value);
	WDSP_ENGINE_DEBUG << "Set AGC Hang time" << value;

}


void QWDSPEngine::setSampleRate(QObject *sender, int value) {

	Q_UNUSED(sender)

	if (m_samplerate == value) return;

	m_mutex.lock();
	switch (value) {

		case 48000:
			m_samplerate = value;
			break;

		case 96000:
			m_samplerate = value;
			break;

		case 192000:
			m_samplerate = value;
			break;

		case 384000:
			m_samplerate = value;
			break;

		default:
			WDSP_ENGINE_DEBUG << "invalid sample rate (possible values are: 48, 96, 192, or 384 kHz)!\n";
			break;
	}
 	SetChannelState(m_rx,0,1);
	SetInputSamplerate(m_rx,m_samplerate);
	SetChannelState(m_rx,1,0);
	WDSP_ENGINE_DEBUG << "sample rate set to " << m_samplerate ;
	m_mutex.unlock();

}


void QWDSPEngine:: setFilter(double low,double high) {

	RXASetPassband(m_rx,low,high);
    WDSP_ENGINE_DEBUG << "Set Filter:Low  " <<  low << "High " << high;
}


void QWDSPEngine::setNCOFrequency(int rx, long ncoFreq) {

	if (getQtDSPStatus() == 0 ) return;

	if (m_rx != rx) return;

	if(ncoFreq==0) {
		SetRXAShiftFreq(m_rx, (double)ncoFreq);
//		RXANBPSetShiftFrequency(m_rx, (double)ncoFreq);
		SetRXAShiftRun(m_rx, 0);
	} else {
		SetRXAShiftFreq(m_rx, (double)ncoFreq);
//		RXANBPSetShiftFrequency(m_rx, (double)ncoFreq);
		SetRXAShiftRun(m_rx, 1);
	}
}

void QWDSPEngine::setSampleSize(int rx, int size) {

	if (m_rx == rx) {

		m_mutex.lock();
		m_spectrumSize = size;
		WDSP_ENGINE_DEBUG <<  "Set sample size" <<  size;
		m_mutex.unlock();
	}
}

void QWDSPEngine::ProcessFrequencyShift(CPX &in, CPX &out, int size) {


}

void QWDSPEngine::init_analyzer(int refreshrate) {

	int result;

	int flp[] = {0};
	double keep_time = 0.1;
	int n_pixout = 1;
	int spur_elimination_ffts = 1;
	int data_type = 1;
	int fft_size = 2048;
	int window_type = 5;
	double kaiser_pi = 14.0;
	int overlap = 2048;
	int clip = 0;
	int span_clip_l = 0;
	int span_clip_h = 0;
	int pixels = 4096;
	int stitches = 1;
	int avm = 0;
	double tau = 0.001 * 120.0;
	int calibration_data_set = 0;
	double span_min_freq = 0.0;
	double span_max_freq = 0.0;

	int max_w = fft_size + (int) min(keep_time * (double) refreshrate, keep_time * (double) fft_size * (double) refreshrate);

	overlap = (int) max(0.0, ceil(fft_size - (double) m_samplerate / (double) refreshrate));

	fprintf(stderr, "SetAnalyzer id=%d buffer_size=%d overlap=%d\n", m_rx, m_size, overlap);


	SetAnalyzer(m_rx,
				n_pixout,
				spur_elimination_ffts, //number of LO frequencies = number of ffts used in elimination
				data_type, //0 for real input data (I only); 1 for complex input data (I & Q)
				flp, //vector with one elt for each LO frequency, 1 if high-side LO, 0 otherwise
				fft_size, //size of the fft, i.e., number of input samples
				1024, //number of samples transferred for each OpenBuffer()/CloseBuffer()
				window_type, //integer specifying which window function to use
				kaiser_pi, //PiAlpha parameter for Kaiser window
				overlap, //number of samples each fft (other than the first) is to re-use from the previous
				clip, //number of fft output bins to be clipped from EACH side of each sub-span
				span_clip_l, //number of bins to clip from low end of entire span
				span_clip_h, //number of bins to clip from high end of entire span
				pixels, //number of pixel values to return.  may be either <= or > number of bins
				stitches, //number of sub-spans to concatenate to form a complete span
				calibration_data_set, //identifier of which set of calibration data to use
				span_min_freq, //frequency at first pixel value8192
				span_max_freq, //frequency at last pixel value
				max_w //max samples to hold in input ring buffers
	);


	SetDisplayDetectorMode(m_rx, 0, 0);
//	SetDisplayAverageMode(m_rx, 0,  2);
}



void QWDSPEngine::setFramesPerSecond(QObject* sender, int rx, int value){

	Q_UNUSED(sender)
	if (rx != m_rx) return;
	m_mutex.lock();
	m_refreshrate = value;
    init_analyzer(value);
    calcDisplayAveraging();
    SetDisplayAvBackmult(rx, 0, m_display_avb);
    SetDisplayNumAverage(rx, 0, m_display_average);
    m_mutex.unlock();
    WDSP_ENGINE_DEBUG <<  "SetFramesPerSecond" <<  value;
}


void QWDSPEngine::setPanAdaptorAveragingMode(int rx, int mode) {
    if (rx != m_rx) return;
    WDSP_ENGINE_DEBUG <<  "Setpan av mode" <<  mode;
    SetDisplayAverageMode(m_rx,0,mode);
}


void QWDSPEngine::setPanAdaptorDetectorMode(int rx, int mode) {
    if (rx != m_rx) return;
    WDSP_ENGINE_DEBUG <<  "Setpan av det  mode" <<  mode;
    SetDisplayDetectorMode(rx,0,mode);

}

void QWDSPEngine::setPanAdaptorAveragingCnt(QObject* sender, int rx, int count){
    Q_UNUSED(sender);
    if (rx != m_rx) return;
    m_averageCount = count;
    calcDisplayAveraging();
    SetDisplayAvBackmult(rx, 0, m_display_avb);
    SetDisplayNumAverage(rx, 0, m_display_average);
    WDSP_ENGINE_DEBUG <<  "Setpan av count mode" <<  m_display_avb << " " << m_display_average;
}

void QWDSPEngine::calcDisplayAveraging() {
    double t=0.001*m_averageCount;
    m_display_avb = exp(-1.0 / ((double)m_refreshrate * t));
    m_display_average = max(2, (int)min(60, (double)m_refreshrate * t));
}

