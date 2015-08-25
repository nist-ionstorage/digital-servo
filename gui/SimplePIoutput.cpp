#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

#include "LockBox.h"
#include "SimplePIoutput.h"
#include "common.h"

SimplePIoutput::SimplePIoutput(LockBox *parent, unsigned int configOffset, QString outputName, QStringList *outputRanges, QStringList *analogSignalNames, QStringList *digitalSignalNames) :
	SimplePIgroupBox(parent, configOffset, outputName, 70, 230),
	IIR2(0),
	IIR3(0)
{
	int labelSize = 70;
	int widgetSize = 230;
	
	// Output range
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Output range:"));
		OutputRangeComboBox = new QComboBox();

		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		for (int i = 0; i < outputRanges->length(); i ++)
		{
			OutputRangeComboBox->addItem(outputRanges->value(i));
		}
		OutputRangeComboBox->setMinimumSize(widgetSize, 0);
		OutputRangeComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(OutputRangeComboBox);
		mainLayout->addLayout(hLayout);
	}

	// Output limits
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label0 = new QLabel(tr("Output limits:"));
		OutputMinSpinBox = new VariableDoubleSpinBox();
		QLabel *label1 = new QLabel("< " + outputName + " <");
		OutputMaxSpinBox = new VariableDoubleSpinBox();

		label0->setMinimumSize(labelSize, 0);
		label0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		OutputMinSpinBox->setSuffix(tr(" V"));
		OutputMinSpinBox->setRange(-10, 10);
		OutputMinSpinBox->setDecimals(3);
		OutputMinSpinBox->setSingleStep(0.1);
		OutputMinSpinBox->setValue(0);

		label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		OutputMaxSpinBox->setSuffix(tr(" V"));
		OutputMaxSpinBox->setRange(-10, 10);
		OutputMaxSpinBox->setDecimals(3);
		OutputMaxSpinBox->setSingleStep(0.1);
		OutputMaxSpinBox->setValue(0);
		
		hLayout->addWidget(label0);
		hLayout->addWidget(OutputMinSpinBox);
		hLayout->addWidget(label1);
		hLayout->addWidget(OutputMaxSpinBox);
		mainLayout->addLayout(hLayout);
	}
	
	// Loop filter on/off
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		LoopFilterOnOffPushButton = new QPushButton(tr("Loop filter"));
		LoopFilterEditPushButton = new QPushButton(tr("Edit parameters"));
		
		LoopFilterOnOffPushButton->setCheckable(true);

		LoopFilterEditPushButton->setCheckable(true);
		LoopFilterEditPushButton->setChecked(true);

		hLayout->addWidget(LoopFilterOnOffPushButton);
		hLayout->addWidget(LoopFilterEditPushButton);
		mainLayout->addLayout(hLayout);
	}

	// Loop filter input
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label0 = new QLabel(tr("Input:"));
		LoopFilterSignComboBox = new QComboBox();
		QLabel *label1 = new QLabel(tr("("));
		LoopFilterInputComboBox = new QComboBox();
		QLabel *label2 = new QLabel(tr("-"));
		LoopFilterOffsetSpinBox = new VariableDoubleSpinBox();
		QLabel *label3 = new QLabel(tr(")"));
		
		label0->setMinimumSize(labelSize, 0);
		label0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		label2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		label3->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		LoopFilterSignComboBox->addItem(tr("+"));
		LoopFilterSignComboBox->addItem(tr("-"));

		for (int i = 0; i < analogSignalNames->length(); i ++)
		{
			LoopFilterInputComboBox->addItem(analogSignalNames->value(i));
		}

		LoopFilterOffsetSpinBox->setSuffix(tr(" V"));
		LoopFilterOffsetSpinBox->setRange(-10, 10);
		LoopFilterOffsetSpinBox->setDecimals(4);
		LoopFilterOffsetSpinBox->setSingleStep(0.001);
		LoopFilterOffsetSpinBox->setValue(0);
		
		hLayout->addWidget(label0);
		hLayout->addWidget(LoopFilterSignComboBox);
		hLayout->addWidget(label1);
		hLayout->addWidget(LoopFilterInputComboBox);
		hLayout->addWidget(label2);
		hLayout->addWidget(LoopFilterOffsetSpinBox);
		hLayout->addWidget(label3);
		mainLayout->addLayout(hLayout);
		
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label0, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), LoopFilterSignComboBox, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label1, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), LoopFilterInputComboBox, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label2, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), LoopFilterOffsetSpinBox, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label3, SLOT(setVisible(bool)));
	}

	// 2nd order IIR filter
	{
		QStringList filterTypes;
		filterTypes << QString("P");
		filterTypes << QString("NOTCH");
		filterTypes << QString("I/HO");
		filterTypes << QString("CUSTOM");
		
		IIR0 = new IIRfilter2ndOrder(parent, configOffset + 19, 26, 27, false, &filterTypes, labelSize, widgetSize - 18);
		mainLayout->addWidget(IIR0);

		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), IIR0, SLOT(setVisible(bool)));
	}

	// 1st order IIR filter
	{
		QStringList filterTypes;
		filterTypes << QString("LOW PASS");
		filterTypes << QString("HIGH PASS");
		filterTypes << QString("ALL PASS");
		filterTypes << QString("I");
		filterTypes << QString("PI");
		filterTypes << QString("P");
		filterTypes << QString("PD");
		filterTypes << QString("CUSTOM");
		
		IIR1 = new IIRfilter1stOrder(parent, configOffset + 35, 26, true, &filterTypes, labelSize, widgetSize - 18);
		mainLayout->addWidget(IIR1);

		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), IIR1, SLOT(setVisible(bool)));
	}

	if ((outputName == "AOUT0") || (outputName == "AOUT1"))
	{
		// 1st order IIR filter
		{
			QStringList filterTypes;
			filterTypes << QString("LOW PASS");
			filterTypes << QString("HIGH PASS");
			filterTypes << QString("ALL PASS");
			filterTypes << QString("I");
			filterTypes << QString("PI");
			filterTypes << QString("P");
			filterTypes << QString("PD");
			filterTypes << QString("CUSTOM");
			
			IIR2 = new IIRfilter1stOrder(parent, configOffset + 45, 26, false, &filterTypes, labelSize, widgetSize - 18);
			mainLayout->addWidget(IIR2);

			QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), IIR2, SLOT(setVisible(bool)));
		}

		// 1st order IIR filter
		{
			QStringList filterTypes;
			filterTypes << QString("LOW PASS");
			filterTypes << QString("HIGH PASS");
			filterTypes << QString("ALL PASS");
			filterTypes << QString("I");
			filterTypes << QString("PI");
			filterTypes << QString("P");
			filterTypes << QString("PD");
			filterTypes << QString("CUSTOM");
			
			IIR3 = new IIRfilter1stOrder(parent, configOffset + 55, 26, false, &filterTypes, labelSize, widgetSize - 18);
			mainLayout->addWidget(IIR3);

			QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), IIR3, SLOT(setVisible(bool)));
		}
	}
	
	// Loop filter hold
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		HoldCheckBox = new QCheckBox(tr("Hold:"));
		HoldComboBox = new QComboBox();

		HoldCheckBox->setMinimumSize(labelSize, 0);
		HoldCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		HoldCheckBox->setChecked(false);

		for (int i = 0; i < digitalSignalNames->length(); i ++)
		{
			HoldComboBox->addItem(digitalSignalNames->value(i));
		}
		HoldComboBox->setEnabled(false);

		hLayout->addWidget(HoldCheckBox);
		hLayout->addWidget(HoldComboBox);
		mainLayout->addLayout(hLayout);
		
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), HoldCheckBox, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), HoldComboBox, SLOT(setVisible(bool)));
	}

	// Loop filter hold falling edge delay
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Falling delay:"));
		HoldFallingDelaySpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		HoldFallingDelaySpinBox->setSuffix(tr(" us"));
		HoldFallingDelaySpinBox->setRange(0.00, 1000000);
		HoldFallingDelaySpinBox->setDecimals(2);
		HoldFallingDelaySpinBox->setSingleStep(1);
		HoldFallingDelaySpinBox->setValue(1);
		HoldFallingDelaySpinBox->setEnabled(false);

		hLayout->addWidget(label);
		hLayout->addWidget(HoldFallingDelaySpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), HoldFallingDelaySpinBox, SLOT(setVisible(bool)));
	}

	// Loop filter hold rising edge delay
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Rising delay:"));
		HoldRisingDelaySpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		HoldRisingDelaySpinBox->setSuffix(tr(" us"));
		HoldRisingDelaySpinBox->setRange(0.00, 1000000);
		HoldRisingDelaySpinBox->setDecimals(2);
		HoldRisingDelaySpinBox->setSingleStep(1);
		HoldRisingDelaySpinBox->setValue(1);
		HoldRisingDelaySpinBox->setEnabled(false);

		hLayout->addWidget(label);
		hLayout->addWidget(HoldRisingDelaySpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), HoldRisingDelaySpinBox, SLOT(setVisible(bool)));
	}

	// Loop filter relock
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		RelockCheckBox = new QCheckBox(tr("Relock:"));
		RelockMinSpinBox = new VariableDoubleSpinBox();
		QLabel *label0 = new QLabel(tr("<"));
		RelockInputComboBox = new QComboBox();
		QLabel *label1 = new QLabel(tr("<"));
		RelockMaxSpinBox = new VariableDoubleSpinBox();
		
		RelockCheckBox->setMinimumSize(labelSize, 0);
		RelockCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		RelockCheckBox->setChecked(false);
		
		RelockMinSpinBox->setSuffix(tr(" V"));
		RelockMinSpinBox->setRange(-10, 10);
		RelockMinSpinBox->setDecimals(3);
		RelockMinSpinBox->setSingleStep(0.1);
		RelockMinSpinBox->setValue(0);
		RelockMinSpinBox->setEnabled(false);

		label0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		label0->setEnabled(false);
		
		for (int i = 0; i < analogSignalNames->length(); i ++)
		{
			RelockInputComboBox->addItem(analogSignalNames->value(i));
		}
		RelockInputComboBox->addItem("DIN0");
		RelockInputComboBox->addItem("DIN1");
		RelockInputComboBox->addItem("DIN2");
		RelockInputComboBox->setEnabled(false);

		label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		label1->setEnabled(false);

		RelockMaxSpinBox->setSuffix(tr(" V"));
		RelockMaxSpinBox->setRange(-10, 10);
		RelockMaxSpinBox->setDecimals(3);
		RelockMaxSpinBox->setSingleStep(0.1);
		RelockMaxSpinBox->setValue(0);
		RelockMaxSpinBox->setEnabled(false);
		
		hLayout->addWidget(RelockCheckBox);
		hLayout->addWidget(RelockMinSpinBox);
		hLayout->addWidget(label0);
		hLayout->addWidget(RelockInputComboBox);
		hLayout->addWidget(label1);
		hLayout->addWidget(RelockMaxSpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), RelockCheckBox, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), RelockMinSpinBox, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label0, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), RelockInputComboBox, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label1, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), RelockMaxSpinBox, SLOT(setVisible(bool)));
	}

	// Loop filter relock sweep
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr(""));
		RelockSweepRateSpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		RelockSweepRateSpinBox->setSuffix(tr(" V/s"));
		RelockSweepRateSpinBox->setRange(0.01, 10000);
		RelockSweepRateSpinBox->setDecimals(2);
		RelockSweepRateSpinBox->setSingleStep(1);
		RelockSweepRateSpinBox->setValue(1);
		RelockSweepRateSpinBox->setEnabled(false);

		hLayout->addWidget(label);
		hLayout->addWidget(RelockSweepRateSpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), RelockSweepRateSpinBox, SLOT(setVisible(bool)));
	}
	
	// Clear integrators when railed
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		ClearIntCheckBox = new QCheckBox(tr("Clear integrators when railed"));
		
		ClearIntCheckBox->setMinimumSize(labelSize, 0);
		ClearIntCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		ClearIntCheckBox->setChecked(false);

		hLayout->addWidget(ClearIntCheckBox, 0, Qt::AlignLeft);
		mainLayout->addLayout(hLayout);
		
		QObject::connect(LoopFilterEditPushButton, SIGNAL(toggled(bool)), ClearIntCheckBox, SLOT(setVisible(bool)));
	}

	// Sweep on/off
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		SweepOnOffPushButton = new QPushButton(tr("Sweep"));
		SweepEditPushButton = new QPushButton(tr("Edit parameters"));
		
		SweepOnOffPushButton->setCheckable(true);

		SweepEditPushButton->setCheckable(true);
		SweepEditPushButton->setChecked(true);

		hLayout->addWidget(SweepOnOffPushButton);
		hLayout->addWidget(SweepEditPushButton);
		mainLayout->addLayout(hLayout);
	}

	// Sweep center
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Center:"));
		SweepCenterSpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		SweepCenterSpinBox->setSuffix(tr(" V"));
		SweepCenterSpinBox->setRange(-10, 10);
		SweepCenterSpinBox->setDecimals(3);
		SweepCenterSpinBox->setSingleStep(0.001);
		SweepCenterSpinBox->setValue(0);
		
		hLayout->addWidget(label);
		hLayout->addWidget(SweepCenterSpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(SweepEditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
		QObject::connect(SweepEditPushButton, SIGNAL(toggled(bool)), SweepCenterSpinBox, SLOT(setVisible(bool)));
	}

	// Sweep amplitude
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Amplitude:"));
		SweepAmplitudeSpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		SweepAmplitudeSpinBox->setSuffix(tr(" V"));
		SweepAmplitudeSpinBox->setRange(0, 10);
		SweepAmplitudeSpinBox->setDecimals(3);
		SweepAmplitudeSpinBox->setSingleStep(0.001);
		SweepAmplitudeSpinBox->setValue(0);
		
		hLayout->addWidget(label);
		hLayout->addWidget(SweepAmplitudeSpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(SweepEditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
		QObject::connect(SweepEditPushButton, SIGNAL(toggled(bool)), SweepAmplitudeSpinBox, SLOT(setVisible(bool)));
	}

	// Sweep frequency
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Frequency:"));
		SweepFrequencySpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		SweepFrequencySpinBox->setSuffix(tr(" Hz"));
		SweepFrequencySpinBox->setRange(0, 10000000);
		SweepFrequencySpinBox->setDecimals(1);
		SweepFrequencySpinBox->setSingleStep(0.1);
		SweepFrequencySpinBox->setValue(10);
		
		hLayout->addWidget(label);
		hLayout->addWidget(SweepFrequencySpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(SweepEditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
		QObject::connect(SweepEditPushButton, SIGNAL(toggled(bool)), SweepFrequencySpinBox, SLOT(setVisible(bool)));
	}

	// Lock-in on/off
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		LockinOnOffPushButton = new QPushButton(tr("Modulation"));
		LockinEditPushButton = new QPushButton(tr("Edit parameters"));
		
		LockinOnOffPushButton->setCheckable(true);

		LockinEditPushButton->setCheckable(true);
		LockinEditPushButton->setChecked(true);

		hLayout->addWidget(LockinOnOffPushButton);
		hLayout->addWidget(LockinEditPushButton);
		mainLayout->addLayout(hLayout);
	}

	// Lock-in amplitude
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("LOCKIN:"));
		LockinAmplitudeSpinBox = new QSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		LockinAmplitudeSpinBox->setSuffix(tr(" dB"));
		LockinAmplitudeSpinBox->setRange(0, 24);
		LockinAmplitudeSpinBox->setSingleStep(1);
		LockinAmplitudeSpinBox->setValue(1);
		
		hLayout->addWidget(label);
		hLayout->addWidget(LockinAmplitudeSpinBox);
		mainLayout->addLayout(hLayout);

		QObject::connect(LockinEditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
		QObject::connect(LockinEditPushButton, SIGNAL(toggled(bool)), LockinAmplitudeSpinBox, SLOT(setVisible(bool)));
	}

	// Hide the expandable menus
	LoopFilterEditPushButton->setChecked(false);
	SweepEditPushButton->setChecked(false);
	LockinEditPushButton->setChecked(false);

	// Hook up on_OutputParametersChanged
	QObject::connect(OutputRangeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_OutputParametersChanged()));
	QObject::connect(OutputMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_OutputParametersChanged()));
	QObject::connect(OutputMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_OutputParametersChanged()));

	// Hook up on_LoopFilterInputParametersChanged
	QObject::connect(LoopFilterOnOffPushButton, SIGNAL(toggled(bool)), this, SLOT(on_LoopFilterInputParametersChanged()));
	QObject::connect(LoopFilterSignComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_LoopFilterInputParametersChanged()));
	QObject::connect(LoopFilterInputComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_LoopFilterInputParametersChanged()));
	QObject::connect(LoopFilterOffsetSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_LoopFilterInputParametersChanged()));

	// Hook up on_LoopFilterHoldParametersChanged
	QObject::connect(HoldCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_LoopFilterHoldParametersChanged()));
	QObject::connect(HoldComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_LoopFilterHoldParametersChanged()));
	QObject::connect(HoldFallingDelaySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_LoopFilterHoldParametersChanged()));
	QObject::connect(HoldRisingDelaySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_LoopFilterHoldParametersChanged()));
	
	// Hook up on_LoopFilterRelockParametersChanged
	QObject::connect(RelockCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_LoopFilterRelockParametersChanged()));
	QObject::connect(RelockMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_LoopFilterRelockParametersChanged()));
	QObject::connect(RelockInputComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_LoopFilterRelockParametersChanged()));
	QObject::connect(RelockMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_LoopFilterRelockParametersChanged()));
	QObject::connect(RelockSweepRateSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_LoopFilterRelockParametersChanged()));
	QObject::connect(ClearIntCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_LoopFilterRelockParametersChanged()));
	
	// Hook up on_SweepParametersChanged
	QObject::connect(SweepOnOffPushButton, SIGNAL(toggled(bool)), this, SLOT(on_SweepParametersChanged()));
	QObject::connect(SweepCenterSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_SweepParametersChanged()));
	QObject::connect(SweepAmplitudeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_SweepParametersChanged()));
	QObject::connect(SweepFrequencySpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_SweepParametersChanged()));

	// Hook up on_LockinParametersChanged
	QObject::connect(LockinOnOffPushButton, SIGNAL(toggled(bool)), this, SLOT(on_LockinParametersChanged()));
	QObject::connect(LockinAmplitudeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_LockinParametersChanged()));

	setAnalogEditable(false);
}

double SimplePIoutput::getOutputRangeMin()
{
	int i = parent->config(configOffset, 131, 130);
	QString outputRangeString = OutputRangeComboBox->itemText(i);
	
	// printf("i = %i, s = %s\n", i, outputRangeString);
	
	if (outputRangeString == "+/-1 V")
		return -1;
	else if (outputRangeString == "+/-2 V")
		return -2;
	else if (outputRangeString == "+/-4 V")
		return -4;
	else if (outputRangeString == "+/-8 V")
		return -8;
	else if (outputRangeString == "+10 V")
		return 0;
	else
		printf("Unidentified output range\n");

	return 0;
}

double SimplePIoutput::getOutputRangeMax()
{
	int i = parent->config(configOffset, 131, 130);
	QString outputRangeString = OutputRangeComboBox->itemText(i);
	if (outputRangeString == "+/-1 V")
		return 1;
	else if (outputRangeString == "+/-2 V")
		return 2;
	else if (outputRangeString == "+/-4 V")
		return 4;
	else if (outputRangeString == "+/-8 V")
		return 8;
	else if (outputRangeString == "+10 V")
		return 10;
	else
		printf("Unidentified output range\n");

	return 0;
}

void SimplePIoutput::analogSignalRangeModified()
{
	on_LoopFilterInputParametersChanged();
	IIR1->setGainMultiplier((parent->getAnalogSignalRangeMax(LoopFilterInputComboBox->currentIndex()) - parent->getAnalogSignalRangeMin(LoopFilterInputComboBox->currentIndex()))/(getOutputRangeMax() - getOutputRangeMin()));
	on_LoopFilterRelockParametersChanged();
	on_SweepParametersChanged();
}

void SimplePIoutput::setAnalogEditable(bool editable)
{
	setNameEditable(editable);

	OutputRangeComboBox->setEnabled(editable);
	OutputMinSpinBox->setEnabled(editable);
	OutputMaxSpinBox->setEnabled(editable);
}

void SimplePIoutput::on_OutputParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset, 131, 130, OutputRangeComboBox->currentIndex());

	OutputMinSpinBox->setRange(getOutputRangeMin(), getOutputRangeMax());
	OutputMaxSpinBox->setRange(getOutputRangeMin(), getOutputRangeMax());
	
	long long int minVal = -32768 + round((65535.0/(getOutputRangeMax() - getOutputRangeMin()))*(OutputMinSpinBox->value() - getOutputRangeMin()));
	long long int maxVal = -32768 + round((65535.0/(getOutputRangeMax() - getOutputRangeMin()))*(OutputMaxSpinBox->value() - getOutputRangeMin()));
	
	/*
	printf("minVal = %lld\n", minVal);
	printf("maxVal = %lld\n", maxVal);
	*/

	parent->set_config(configOffset + 9, 16, 0, minVal);
	parent->set_config(configOffset + 10, 16, 0, maxVal);
	
	parent->analogSignalRangeModified();
}

void SimplePIoutput::on_LoopFilterInputParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset + 11, 0, 0, LoopFilterOnOffPushButton->isChecked());
	parent->set_config(configOffset + 11, 1, 1, LoopFilterSignComboBox->currentIndex());
	parent->set_config(configOffset + 11, 4, 2, LoopFilterInputComboBox->currentIndex());

	double minVal = parent->getAnalogSignalRangeMin(LoopFilterInputComboBox->currentIndex());
	double maxVal = parent->getAnalogSignalRangeMax(LoopFilterInputComboBox->currentIndex());
	LoopFilterOffsetSpinBox->setRange(minVal, maxVal);
	
	long long int offset = -32768 + round((65535.0/(maxVal - minVal))*(LoopFilterOffsetSpinBox->value() - minVal));
	parent->set_config(configOffset + 12, 15, 0, offset);

	IIR1->setGainMultiplier((parent->getAnalogSignalRangeMax(LoopFilterInputComboBox->currentIndex()) - parent->getAnalogSignalRangeMin(LoopFilterInputComboBox->currentIndex()))/(getOutputRangeMax() - getOutputRangeMin()));
}

void SimplePIoutput::on_LoopFilterHoldParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset + 13, 0, 0, HoldCheckBox->isChecked());
	parent->set_config(configOffset + 13, 4, 1, HoldComboBox->currentIndex());
	parent->set_config(configOffset + 75, 32, 0, round(100.0*(HoldFallingDelaySpinBox->value())));
	parent->set_config(configOffset + 77, 32, 0, round(100.0*(HoldRisingDelaySpinBox->value())));
	
	HoldComboBox->setEnabled(HoldCheckBox->isChecked());
	HoldFallingDelaySpinBox->setEnabled(HoldCheckBox->isChecked());
	HoldRisingDelaySpinBox->setEnabled(HoldCheckBox->isChecked());
}

void SimplePIoutput::on_LoopFilterRelockParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset + 73, 0, 0, ClearIntCheckBox->isChecked());
	
	parent->set_config(configOffset + 14, 0, 0, RelockCheckBox->isChecked());
	parent->set_config(configOffset + 14, 4, 1, RelockInputComboBox->currentIndex());

	double minRange = parent->getAnalogSignalRangeMin(RelockInputComboBox->currentIndex());
	double maxRange = parent->getAnalogSignalRangeMax(RelockInputComboBox->currentIndex());
	RelockMinSpinBox->setRange(minRange, maxRange);
	RelockMaxSpinBox->setRange(minRange, maxRange);
	
	// for digital inputs, set minVal and maxVal such that logical 0 means locked
	long long int minVal = -1;
	long long int maxVal = 1;
	if (RelockInputComboBox->currentIndex() < 7)
	{
		// for analog inputs, use the user selected input range
		minVal = -32768 + round((65535.0/(maxRange - minRange))*(RelockMinSpinBox->value() - minRange));
		maxVal = -32768 + round((65535.0/(maxRange - minRange))*(RelockMaxSpinBox->value() - minRange));
	}
	parent->set_config(configOffset + 15, 15, 0, minVal);
	parent->set_config(configOffset + 16, 15, 0, maxVal);

	// printf("minVal = %lld\n", minVal);
	// printf("maxVal = %lld\n", maxVal);

	long long int stepSize = round((1.0995e12/FPGA_CLOCK_HZ)*RelockSweepRateSpinBox->value()/(getOutputRangeMax() - getOutputRangeMin()));
	parent->set_config(configOffset + 17, 31, 0, stepSize);
	
	RelockMinSpinBox->setEnabled(RelockCheckBox->isChecked());
	RelockInputComboBox->setEnabled(RelockCheckBox->isChecked());
	RelockMaxSpinBox->setEnabled(RelockCheckBox->isChecked());
	RelockSweepRateSpinBox->setEnabled(RelockCheckBox->isChecked());
}

void SimplePIoutput::on_SweepParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	double center = SweepCenterSpinBox->value();
	double amplitude = SweepAmplitudeSpinBox->value();
	double frequency = SweepFrequencySpinBox->value();

	long long int minVal = -32768 + round((65535.0/(getOutputRangeMax() - getOutputRangeMin()))*(center - amplitude - getOutputRangeMin()));
	if (minVal < -32768)
		minVal = -32768;
	long long int maxVal = -32768 + round((65535.0/(getOutputRangeMax() - getOutputRangeMin()))*(center + amplitude - getOutputRangeMin()));
	if (maxVal > 32767)
		maxVal = 32767;
	long long int stepSize = round(65535.0*2.0*(maxVal - minVal)/(FPGA_CLOCK_HZ/frequency - 6.0));

	/*
	printf("minVal = %lld\n", minVal);
	printf("maxVal = %lld\n", maxVal);
	printf("stepSize = %lld\n", stepSize);
	*/

	parent->set_config(configOffset + 65, 0, 0, SweepOnOffPushButton->isChecked());
	parent->set_config(configOffset + 66, 15, 0, minVal);
	parent->set_config(configOffset + 67, 15, 0, maxVal);
	parent->set_config(configOffset + 68, 31, 0, stepSize);
	parent->set_config(configOffset + 70, 31, 0, doubleToConfig(frequency));
}

void SimplePIoutput::on_LockinParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	int rs = 24;
	if (LockinOnOffPushButton->isChecked())
	{
		rs = 24 - LockinAmplitudeSpinBox->value();
	}

	parent->set_config(configOffset + 72, 15, 0, rs);
}

void SimplePIoutput::set_widgets_from_config()
{
	SimplePIgroupBox::set_widgets_from_config();
	
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	// Output parameters
	{
		OutputRangeComboBox->setCurrentIndex(parent->config(configOffset, 131, 130));
	
		OutputMinSpinBox->setRange(getOutputRangeMin(), getOutputRangeMax());
		OutputMaxSpinBox->setRange(getOutputRangeMin(), getOutputRangeMax());
		
		double minVal = (double(parent->config(configOffset + 9, 15, 0, 1)) + 32768.0)*(getOutputRangeMax() - getOutputRangeMin())/65535.0 + getOutputRangeMin();
		double maxVal = (double(parent->config(configOffset + 10, 15, 0, 1)) + 32768.0)*(getOutputRangeMax() - getOutputRangeMin())/65535.0 + getOutputRangeMin();
		
		OutputMinSpinBox->setValue(minVal);
		OutputMaxSpinBox->setValue(maxVal);
	}

	// Loop filter input
	{
		LoopFilterOnOffPushButton->setChecked(parent->config(configOffset + 11, 0, 0));
		LoopFilterSignComboBox->setCurrentIndex(parent->config(configOffset + 11, 1, 1));
		LoopFilterInputComboBox->setCurrentIndex(parent->config(configOffset + 11, 4, 2));
	
		double minVal = parent->getAnalogSignalRangeMin(LoopFilterInputComboBox->currentIndex());
		double maxVal = parent->getAnalogSignalRangeMax(LoopFilterInputComboBox->currentIndex());
		LoopFilterOffsetSpinBox->setRange(minVal, maxVal);
		
		double offset = (double(parent->config(configOffset + 12, 15, 0, 1)) + 32768.0)*(maxVal - minVal)/65535.0 + minVal;
		LoopFilterOffsetSpinBox->setValue(offset);
	}

	// Loop filter
	{
		IIR0->set_widgets_from_config();
		IIR1->set_widgets_from_config((parent->getAnalogSignalRangeMax(LoopFilterInputComboBox->currentIndex()) - parent->getAnalogSignalRangeMin(LoopFilterInputComboBox->currentIndex()))/(getOutputRangeMax() - getOutputRangeMin()));
		if (IIR2)
			IIR2->set_widgets_from_config();
		if (IIR3)
			IIR3->set_widgets_from_config();
	}

	// Loop filter hold
	{
		HoldCheckBox->setChecked(parent->config(configOffset + 13, 0, 0));
		HoldComboBox->setCurrentIndex(parent->config(configOffset + 13, 4, 1));
		HoldFallingDelaySpinBox->setValue(0.01*double(parent->config(configOffset + 75, 32, 0, 0)));
		HoldRisingDelaySpinBox->setValue(0.01*double(parent->config(configOffset + 77, 32, 0, 0)));
	
		HoldComboBox->setEnabled(HoldCheckBox->isChecked());
		HoldFallingDelaySpinBox->setEnabled(HoldCheckBox->isChecked());
		HoldRisingDelaySpinBox->setEnabled(HoldCheckBox->isChecked());
	}

	// Loop filter relock
	{
		ClearIntCheckBox->setChecked(parent->config(configOffset + 73, 0, 0));
		
		RelockCheckBox->setChecked(parent->config(configOffset + 14, 0, 0));
		RelockInputComboBox->setCurrentIndex(parent->config(configOffset + 14, 4, 1));

		double minRange = parent->getAnalogSignalRangeMin(RelockInputComboBox->currentIndex());
		double maxRange = parent->getAnalogSignalRangeMax(RelockInputComboBox->currentIndex());
		RelockMinSpinBox->setRange(minRange, maxRange);
		RelockMaxSpinBox->setRange(minRange, maxRange);
	
		double minVal = (double(parent->config(configOffset + 15, 15, 0, 1)) + 32768.0)*(maxRange - minRange)/65535.0 + minRange;
		double maxVal = (double(parent->config(configOffset + 16, 15, 0, 1)) + 32768.0)*(maxRange - minRange)/65535.0 + minRange;
		RelockMinSpinBox->setValue(minVal);
		RelockMaxSpinBox->setValue(maxVal);

		double stepSize = double(parent->config(configOffset + 17, 31, 0))*(getOutputRangeMax() - getOutputRangeMin())*(FPGA_CLOCK_HZ/1.0995e12);
		RelockSweepRateSpinBox->setValue(stepSize);

		RelockMinSpinBox->setEnabled(RelockCheckBox->isChecked());
		RelockInputComboBox->setEnabled(RelockCheckBox->isChecked());
		RelockMaxSpinBox->setEnabled(RelockCheckBox->isChecked());
		RelockSweepRateSpinBox->setEnabled(RelockCheckBox->isChecked());
	}

	// Sweep parameters
	{
		long long int minVal = parent->config(configOffset + 66, 15, 0, 1);
		long long int maxVal = parent->config(configOffset + 67, 15, 0, 1);
		long long int stepSize = parent->config(configOffset + 68, 31, 0);;
		
		double center = getOutputRangeMin() + 0.5*((getOutputRangeMax() - getOutputRangeMin())/65535.0)*double(maxVal + minVal + 65536);
		double amplitude = 0.5*((getOutputRangeMax() - getOutputRangeMin())/65535.0)*double(maxVal - minVal);
		double frequency = FPGA_CLOCK_HZ/(65535.0*2.0*(double(maxVal) - double(minVal))/double(stepSize) + 6.0);
		
		/*
		printf("center = %f\n", center);
		printf("amplitude = %f\n", amplitude);
		printf("frequency = %f, %f\n", frequency, configToDouble(parent->config(configOffset + 70, 31, 0, 1)));
		*/

		SweepOnOffPushButton->setChecked(parent->config(configOffset + 65, 0, 0));
		SweepCenterSpinBox->setValue(center);
		SweepAmplitudeSpinBox->setValue(amplitude);
		if (frequency != frequency) // evaluates to true if frequency is indeterminate
			SweepFrequencySpinBox->setValue(configToDouble(parent->config(configOffset + 70, 31, 0, 1)));
		else
			SweepFrequencySpinBox->setValue(frequency);
	}

	// Lock-in parameters
	{
		int rs = parent->config(configOffset + 72, 15, 0, 0);
		
		if (rs == 24)
			LockinOnOffPushButton->setChecked(false);
		else
			LockinOnOffPushButton->setChecked(true);

		LockinAmplitudeSpinBox->setValue(24 - rs);
	}

	SETTING_WIDGETS_FROM_CONFIG = false;
}