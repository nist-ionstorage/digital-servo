#ifndef MEASURETRANSFERFUNCTIONDIALOG_H
#define MEASURETRANSFERFUNCTIONDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>

class MeasureTransferFunctionDialog : public QDialog
{
    Q_OBJECT

public:
    MeasureTransferFunctionDialog(QWidget *parent = 0);

	double getModFreqStart();
	double getModFreqStop();
	int getModFreqSteps();
	int getModAmp();
	int getModOutChan();
	int getSampleRate();
	int getNsamples();

	void setModFreqStart(double modFreqStart);
	void setModFreqStop(double modFreqStop);
	void setModFreqSteps(int modFreqSteps);
	void setModAmp(int modAmp);
	void setModOutChan(int modOutChan);
	void setSampleRate(int sampleRate);
	void setNsamples(int Nsamples);

private slots:
	void NsamplesChanged(int newValue);

private:
	QDoubleSpinBox *modFreqStartSpinBox, *modFreqStopSpinBox;
	QSpinBox *modFreqStepsSpinBox, *modAmpSpinBox;
	QComboBox *modOutChanComboBox;
	QSpinBox *sampleRateSpinBox, *NsamplesSpinBox;
	QPushButton *okButton, *cancelButton;
};

#endif // MEASURETRANSFERFUNCTIONDIALOG_H
