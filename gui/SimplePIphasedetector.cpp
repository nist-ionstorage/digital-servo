#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "LockBox.h"
#include "SimplePIphasedetector.h"
#include "common.h"

SimplePIphasedetector::SimplePIphasedetector(LockBox *parent, unsigned int configOffset, QString inputName) :
	SimplePIgroupBox(parent, configOffset, inputName, 70, 130)
{
	int labelSize = 70;
	int widgetSize = 130;
	
	// Phase detector filter input
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Input:"));
		phasedetectorInputComboBox = new QComboBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		phasedetectorInputComboBox->addItem(tr("AIN0"));
		phasedetectorInputComboBox->addItem(tr("AIN1"));

		phasedetectorInputComboBox->setMinimumSize(widgetSize, 0);
		phasedetectorInputComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		hLayout->addWidget(label);
		hLayout->addWidget(phasedetectorInputComboBox);
		mainLayout->addLayout(hLayout);
	}

	// Input frequency
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Frequency:"));
		inputFreqSpinBox = new VariableDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		inputFreqSpinBox->setSuffix(tr(" Hz"));
		inputFreqSpinBox->setRange(10, 10000000);
		inputFreqSpinBox->setDecimals(0);
		inputFreqSpinBox->setSingleStep(10);
		inputFreqSpinBox->setValue(1000);

		inputFreqSpinBox->setMinimumSize(widgetSize, 0);
		inputFreqSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(inputFreqSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// 1st order IIR filter, low pass only
	{
		QStringList filterTypes;
		filterTypes << QString("LOW PASS*");
		
		IIR0 = new IIRfilter1stOrder(parent, configOffset + 11, 32, true, &filterTypes, labelSize, widgetSize);
		mainLayout->addWidget(IIR0);
	}

	// External clock check box
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		useExternalClockCheckBox = new QCheckBox(tr("Use external clock"));
		
		useExternalClockCheckBox->setMinimumSize(labelSize, 0);
		useExternalClockCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		hLayout->addWidget(useExternalClockCheckBox);
		mainLayout->addLayout(hLayout);
	}

	#ifdef DEBUG_PHASEDETECTOR_PLL	
	// UPDATE_PLL_EVERY = configOffset + 21, 32 bits
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Update PLL every:"));
		updatePLLeverySpinBox = new VariableLongLongSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		updatePLLeverySpinBox->setRange(0, 4294967295);
		updatePLLeverySpinBox->setSingleStep(1);
		updatePLLeverySpinBox->setValue(0);

		updatePLLeverySpinBox->setMinimumSize(widgetSize, 0);
		updatePLLeverySpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(updatePLLeverySpinBox);
		mainLayout->addLayout(hLayout);
	}

	// UPDATE_PLL_RS_P = configOffset + 23, 8 bits
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Update PLL RS P:"));
		updatePLLrspSpinBox = new VariableLongLongSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		updatePLLrspSpinBox->setRange(0, 255);
		updatePLLrspSpinBox->setSingleStep(1);
		updatePLLrspSpinBox->setValue(0);

		updatePLLrspSpinBox->setMinimumSize(widgetSize, 0);
		updatePLLrspSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(updatePLLrspSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// UPDATE_PLL_RS_I = configOffset + 23, 8 bits
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Update PLL RS I:"));
		updatePLLrsiSpinBox = new VariableLongLongSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		updatePLLrsiSpinBox->setRange(0, 255);
		updatePLLrsiSpinBox->setSingleStep(1);
		updatePLLrsiSpinBox->setValue(0);

		updatePLLrsiSpinBox->setMinimumSize(widgetSize, 0);
		updatePLLrsiSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(updatePLLrsiSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// rst_in = configOffset + 24, 1 bit
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Reset:"));
		rstInSpinBox = new VariableLongLongSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		rstInSpinBox->setRange(0, 1);
		rstInSpinBox->setSingleStep(1);
		rstInSpinBox->setValue(0);

		rstInSpinBox->setMinimumSize(widgetSize, 0);
		rstInSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(rstInSpinBox);
		mainLayout->addLayout(hLayout);
	}

	QObject::connect(updatePLLeverySpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_debugParametersChanged()));
	QObject::connect(updatePLLrspSpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_debugParametersChanged()));
	QObject::connect(updatePLLrsiSpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_debugParametersChanged()));
	QObject::connect(rstInSpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_debugParametersChanged()));
	#endif // DEBUG_PHASEDETECTOR_PLL

	// Hook up on_phasedetectorInputChanged
	QObject::connect(phasedetectorInputComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_phasedetectorInputChanged()));
	
	// Hook up on_inputFreqChanged
	QObject::connect(inputFreqSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_inputFreqChanged()));

	// Hook up on_externalClockChanged
	QObject::connect(useExternalClockCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_externalClockChanged()));

	setAnalogEditable(false);
}

double SimplePIphasedetector::getInputRangeMin()
{
	return parent->getAnalogSignalRangeMin(parent->config(configOffset, 128, 128));
}

double SimplePIphasedetector::getInputRangeMax()
{
	return parent->getAnalogSignalRangeMax(parent->config(configOffset, 128, 128));
}

void SimplePIphasedetector::setAnalogEditable(bool editable)
{
	setNameEditable(editable);
}

void SimplePIphasedetector::on_phasedetectorInputChanged()
{
	parent->set_config(configOffset + 8, 0, 0, phasedetectorInputComboBox->currentIndex());
}

void SimplePIphasedetector::on_inputFreqChanged()
{
	long long int pinc = round((0x1LL << 30)*inputFreqSpinBox->value()/EXT_CLOCK_HZ);
	parent->set_config(configOffset + 9, 31, 0, pinc);
}

void SimplePIphasedetector::on_externalClockChanged()
{
	parent->set_config(configOffset + 8, 1, 1, useExternalClockCheckBox->isChecked());
}

#ifdef DEBUG_PHASEDETECTOR_PLL
void SimplePIphasedetector::on_debugParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;
	
	parent->set_config(configOffset + 21, 31, 0, updatePLLeverySpinBox->value());
	parent->set_config(configOffset + 23, 7, 0, updatePLLrspSpinBox->value());
	parent->set_config(configOffset + 23, 15, 8, updatePLLrsiSpinBox->value());
	parent->set_config(configOffset + 24, 0, 0, rstInSpinBox->value());
}
#endif // DEBUG_PHASEDETECTOR_PLL

void SimplePIphasedetector::set_widgets_from_config()
{
	SimplePIgroupBox::set_widgets_from_config();
	
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	// Input
	{
		phasedetectorInputComboBox->setCurrentIndex(parent->config(configOffset + 8, 0, 0));
	}

	// IIR filters
	{
		IIR0->set_widgets_from_config();
	}

	// Frequency
	{
		long long int pinc = parent->config(configOffset + 9, 31, 0, 0);
		inputFreqSpinBox->setValue(EXT_CLOCK_HZ*double(pinc)/(0x1LL << 30));
	}

	// External clock
	{
		useExternalClockCheckBox->setChecked(parent->config(configOffset + 8, 1, 1));
	}

	#ifdef DEBUG_PHASEDETECTOR_PLL
	{
		updatePLLeverySpinBox->setValue(parent->config(configOffset + 21, 31, 0));
		updatePLLrspSpinBox->setValue(parent->config(configOffset + 23, 7, 0));
		updatePLLrsiSpinBox->setValue(parent->config(configOffset + 23, 15, 8));
		rstInSpinBox->setValue(parent->config(configOffset + 24, 0, 0));
	}
	#endif // DEBUG_PHASEDETECTOR_PLL

	SETTING_WIDGETS_FROM_CONFIG = false;
}