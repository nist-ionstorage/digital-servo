#ifndef SIMPLEPIOUTPUT_H
#define SIMPLEPIOUTPUT_H

#include "SimplePIgroupBox.h"
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include "VariableDoubleSpinBox.h"
#include <QDoubleSpinBox>
#include "IIRfilter1stOrder.h"
#include "IIRfilter2ndOrder.h"

class LockBox;

class SimplePIoutput : public SimplePIgroupBox
{
    Q_OBJECT

public:
    SimplePIoutput(LockBox *parent, unsigned int configOffset, QString outputName, QStringList *outputRanges, QStringList *analogSignalNames, QStringList *digitalSignalNames);

	void set_widgets_from_config();

	double getOutputRangeMin();
	double getOutputRangeMax();
	void analogSignalRangeModified();
	void setAnalogEditable(bool editable);
	
protected:
	QComboBox *OutputRangeComboBox;
	VariableDoubleSpinBox *OutputMinSpinBox, *OutputMaxSpinBox;

	QPushButton *LoopFilterOnOffPushButton, *LoopFilterEditPushButton;
	QComboBox *LoopFilterSignComboBox, *LoopFilterInputComboBox, *HoldComboBox, *RelockInputComboBox;
	VariableDoubleSpinBox *LoopFilterOffsetSpinBox, *HoldFallingDelaySpinBox, *HoldRisingDelaySpinBox, *RelockMinSpinBox, *RelockMaxSpinBox, *RelockSweepRateSpinBox;
	QCheckBox *HoldCheckBox, *RelockCheckBox, *ClearIntCheckBox;
	IIRfilter2ndOrder *IIR0;
	IIRfilter1stOrder *IIR1, *IIR2, *IIR3;

	QPushButton *SweepOnOffPushButton, *SweepEditPushButton;
	VariableDoubleSpinBox *SweepCenterSpinBox, *SweepAmplitudeSpinBox, *SweepFrequencySpinBox;

	QPushButton *LockinOnOffPushButton, *LockinEditPushButton;
	QSpinBox *LockinAmplitudeSpinBox;

private slots:
	void on_OutputParametersChanged();
	void on_LoopFilterInputParametersChanged();
	void on_LoopFilterHoldParametersChanged();
	void on_LoopFilterRelockParametersChanged();
	void on_SweepParametersChanged();
	void on_LockinParametersChanged();
};

#endif // SIMPLEPIOUTPUT_H