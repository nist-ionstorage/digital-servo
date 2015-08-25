#ifndef SIMPLEPIPHASEDETECTOR_H
#define SIMPLEPIPHASEDETECTOR_H

#include "SimplePIgroupBox.h"
#include <QCheckBox>
#include <QComboBox>
#include "VariableDoubleSpinBox.h"
#include "VariableLongLongSpinBox.h"
#include "IIRfilter1stOrder.h"

// #define DEBUG_PHASEDETECTOR_PLL

class LockBox;

class SimplePIphasedetector : public SimplePIgroupBox
{
    Q_OBJECT

public:
    SimplePIphasedetector(LockBox *parent, unsigned int configOffset, QString inputName);

	double getInputRangeMin();
	double getInputRangeMax();
	
	void set_widgets_from_config();

	void setAnalogEditable(bool editable);
	
protected:
	QComboBox *phasedetectorInputComboBox;
	VariableDoubleSpinBox *inputFreqSpinBox;
	IIRfilter1stOrder *IIR0;
	QCheckBox *useExternalClockCheckBox;

	#ifdef DEBUG_PHASEDETECTOR_PLL
	VariableLongLongSpinBox *updatePLLeverySpinBox, *updatePLLrspSpinBox, *updatePLLrsiSpinBox, *rstInSpinBox;
	#endif // DEBUG_PHASEDETECTOR_PLL

private slots:
	void on_phasedetectorInputChanged();
	void on_inputFreqChanged();
	void on_externalClockChanged();

	#ifdef DEBUG_PHASEDETECTOR_PLL
	void on_debugParametersChanged();
	#endif // DEBUG_PHASEDETECTOR_PLL
};

#endif // SIMPLEPIPHASEDETECTOR_H