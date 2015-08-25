#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "LockBox.h"
#include "SimplePIlockin.h"
#include "common.h"

SimplePIlockin::SimplePIlockin(LockBox *parent, unsigned int configOffset, QString inputName) :
	SimplePIgroupBox(parent, configOffset, inputName, 70, 130)
{
	int labelSize = 70;
	int widgetSize = 130;
	
	// Lockin filter input
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Input:"));
		lockinInputComboBox = new QComboBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		lockinInputComboBox->addItem(tr("AIN0"));
		lockinInputComboBox->addItem(tr("AIN1"));

		lockinInputComboBox->setMinimumSize(widgetSize, 0);
		lockinInputComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		hLayout->addWidget(label);
		hLayout->addWidget(lockinInputComboBox);
		mainLayout->addLayout(hLayout);
	}

	// 2nd order IIR filter
	{
		QStringList filterTypes;
		filterTypes << QString("LOW PASS");
		filterTypes << QString("HIGH PASS");
		filterTypes << QString("NOTCH");
		filterTypes << QString("P");
		filterTypes << QString("CUSTOM");
		
		IIR0 = new IIRfilter2ndOrder(parent, configOffset + 9, 32, 26, false, &filterTypes, labelSize, widgetSize);
		mainLayout->addWidget(IIR0);
	}

	// Input frequency
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Frequency:"));
		InputFreqSpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		InputFreqSpinBox->setSuffix(tr(" Hz"));
		// InputFreqSpinBox->setRange(10, 10000000);
		InputFreqSpinBox->setRange(0, 10000000);
		InputFreqSpinBox->setDecimals(0);
		InputFreqSpinBox->setSingleStep(10);
		InputFreqSpinBox->setValue(1000);

		InputFreqSpinBox->setMinimumSize(widgetSize, 0);
		InputFreqSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(InputFreqSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// Input phase
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Phase:"));
		InputPhaseSpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		InputPhaseSpinBox->setSuffix(tr(" deg"));
		InputPhaseSpinBox->setRange(0, 360);
		InputPhaseSpinBox->setDecimals(0);
		InputPhaseSpinBox->setSingleStep(1);
		InputPhaseSpinBox->setValue(0);

		InputPhaseSpinBox->setMinimumSize(widgetSize, 0);
		InputPhaseSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(InputPhaseSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// 2nd order IIR filter
	{
		QStringList filterTypes;
		filterTypes << QString("LOW PASS");
		filterTypes << QString("HIGH PASS");
		filterTypes << QString("NOTCH");
		filterTypes << QString("P");
		filterTypes << QString("CUSTOM");
		
		IIR1 = new IIRfilter2ndOrder(parent, configOffset + 29, 32, 26, false, &filterTypes, labelSize, widgetSize);
		mainLayout->addWidget(IIR1);
	}

	// Hook up on_LockinInputChanged
	QObject::connect(lockinInputComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_LockinInputChanged()));
	
	// Hook up on_InputFreqChanged
	QObject::connect(InputFreqSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_InputFreqChanged()));
	
	// Hook up on_InputPhaseChanged
	QObject::connect(InputPhaseSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_InputPhaseChanged()));

	setAnalogEditable(false);
}

double SimplePIlockin::getInputRangeMin()
{
	return parent->getAnalogSignalRangeMin(parent->config(configOffset, 128, 128));
}

double SimplePIlockin::getInputRangeMax()
{
	return parent->getAnalogSignalRangeMax(parent->config(configOffset, 128, 128));
}

void SimplePIlockin::setAnalogEditable(bool editable)
{
	setNameEditable(editable);
}

void SimplePIlockin::on_LockinInputChanged()
{
	parent->set_config(configOffset + 8, 0, 0, lockinInputComboBox->currentIndex());
}

void SimplePIlockin::on_InputFreqChanged()
{
	long long int pinc = round((0x1LL << 24)*InputFreqSpinBox->value()/FPGA_CLOCK_HZ);
	parent->set_config(configOffset + 25, 24, 0, pinc);
}

void SimplePIlockin::on_InputPhaseChanged()
{
	long long int poff = round((0x1LL << 24)*InputPhaseSpinBox->value()/360.0);
	parent->set_config(configOffset + 27, 24, 0, poff);
}

void SimplePIlockin::set_widgets_from_config()
{
	SimplePIgroupBox::set_widgets_from_config();
	
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	// Input
	{
		lockinInputComboBox->setCurrentIndex(parent->config(configOffset + 8, 0, 0));
	}
	// IIR filters
	{
		IIR0->set_widgets_from_config();
		IIR1->set_widgets_from_config();
	}

	// Frequency and phase
	{
		long long int pinc = parent->config(configOffset + 25, 24, 0, 0);
		InputFreqSpinBox->setValue(FPGA_CLOCK_HZ*double(pinc)/(0x1LL << 24));

		long long int poff = parent->config(configOffset + 27, 24, 0, 0);
		InputPhaseSpinBox->setValue(360.0*double(poff)/(0x1LL << 24));
	}

	SETTING_WIDGETS_FROM_CONFIG = false;
}