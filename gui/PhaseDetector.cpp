#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

#include "LockBox.h"
#include "PhaseDetector.h"
#include "common.h"

PhaseDetector::PhaseDetector(LockBox *parent, unsigned int configOffset, QString inputName, QStringList *inputRanges) :
	QWidget(0),
	parent(parent),
	configOffset(configOffset),
	SETTING_WIDGETS_FROM_CONFIG(false)
{
	int labelSize = 70;
	int widgetSize = 80;
	
	QGroupBox *mainGroupBox = new QGroupBox(inputName);
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->addWidget(mainGroupBox);
	}

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainGroupBox->setLayout(mainLayout);

	// Input range
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Input range:"));
		InputRangeComboBox = new QComboBox();

		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		for (int i = 0; i < inputRanges->length(); i ++)
		{
			InputRangeComboBox->addItem(inputRanges->value(i));
		}
		InputRangeComboBox->setMinimumSize(widgetSize, 0);
		InputRangeComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(InputRangeComboBox);
		mainLayout->addLayout(hLayout);
	}

	// Input frequency
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Frequency:"));
		InputFreqSpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		InputFreqSpinBox->setSuffix(tr(" Hz"));
		InputFreqSpinBox->setRange(10, 10000000);
		InputFreqSpinBox->setDecimals(0);
		InputFreqSpinBox->setSingleStep(10);
		InputFreqSpinBox->setValue(1000);
		
		hLayout->addWidget(label);
		hLayout->addWidget(InputFreqSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// LP filter
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		LPCheckBox = new QCheckBox(tr("LP filter:"));
		LPFreqSpinBox = new VariableDoubleSpinBox();
		
		LPCheckBox->setMinimumSize(labelSize, 0);
		LPCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		LPCheckBox->setChecked(false);
		
		LPFreqSpinBox->setSuffix(tr(" Hz"));
		LPFreqSpinBox->setRange(1, 10000000);
		LPFreqSpinBox->setDecimals(0);
		LPFreqSpinBox->setSingleStep(1);
		LPFreqSpinBox->setValue(1000000);
		LPFreqSpinBox->setEnabled(false);

		hLayout->addWidget(LPCheckBox);
		hLayout->addWidget(LPFreqSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// Hook up on_InputParametersChanged
	QObject::connect(InputRangeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_InputParametersChanged()));

	// Hook up on_InputFreqChanged
	QObject::connect(InputFreqSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_InputFreqChanged()));
	
	// Hook up on_LPParametersChanged
	QObject::connect(LPCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_LPParametersChanged()));
	QObject::connect(LPFreqSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_LPParametersChanged()));
}

void PhaseDetector::on_InputParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset, 3, 2, InputRangeComboBox->currentIndex());
}

void PhaseDetector::on_InputFreqChanged()
{
	long long int pinc = round((0x1LL << 32)*InputFreqSpinBox->value()/FPGA_CLOCK_HZ);
	parent->set_config(configOffset + 8, 31, 0, pinc);
}

void PhaseDetector::on_LPParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset + 1, 0, 0, LPCheckBox->isChecked());
	LPFreqSpinBox->setEnabled(LPCheckBox->isChecked());

	double fLP = LPFreqSpinBox->value();
	
	long long int a1 = round((0x1LL << 32)*(1.0 - 2.0*M_PI*fLP*FPGA_CLOCK_T/(1.0 + 2.0*M_PI*fLP*FPGA_CLOCK_T)));
	long long int b0 = round((0x1LL << 32)*(2.0*M_PI*fLP*FPGA_CLOCK_T/(1.0 + 2.0*M_PI*fLP*FPGA_CLOCK_T)));
	
	/*
	printf("fLP = %f\n", fPI);

	printf("a1 = %lld\n", a1);
	printf("b0 = %lld\n", b0);
	*/

	parent->set_config(configOffset + 2, 34, 0, a1);
	parent->set_config(configOffset + 5, 34, 0, b0);
}

void PhaseDetector::set_widgets_from_config()
{
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	// Input parameters
	{
		InputRangeComboBox->setCurrentIndex(parent->config(configOffset, 3, 2));
	}

	// Frequency
	{
		long long int pinc = parent->config(configOffset + 8, 31, 0, 0);
		InputFreqSpinBox->setValue(FPGA_CLOCK_HZ*double(pinc)/(0x1LL << 32));
	}

	// LP parameters
	{
		LPCheckBox->setChecked(parent->config(configOffset + 1, 0, 0));
		LPFreqSpinBox->setEnabled(LPCheckBox->isChecked());

		long long int a1 = parent->config(configOffset + 2, 34, 0, 1);
		long long int b0 = parent->config(configOffset + 5, 34, 0, 1);
		
		double fLP = (1.0/(2.0*M_PI*FPGA_CLOCK_T))*(double(b0)/(0x1LL << 32))/(1.0 - (double(b0)/(0x1LL << 32)));
	
		LPFreqSpinBox->setValue(fLP);
	}

	SETTING_WIDGETS_FROM_CONFIG = false;
}