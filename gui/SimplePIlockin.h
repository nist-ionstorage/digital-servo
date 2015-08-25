#ifndef SIMPLEPILOCKIN_H
#define SIMPLEPILOCKIN_H

#include "SimplePIgroupBox.h"
#include <QComboBox>
#include "VariableDoubleSpinBox.h"
#include "IIRfilter2ndOrder.h"

class LockBox;

class SimplePIlockin : public SimplePIgroupBox
{
    Q_OBJECT

public:
    SimplePIlockin(LockBox *parent, unsigned int configOffset, QString inputName);

	double getInputRangeMin();
	double getInputRangeMax();
	
	void set_widgets_from_config();

	void setAnalogEditable(bool editable);
	
protected:
	QComboBox *lockinInputComboBox;
	VariableDoubleSpinBox *InputFreqSpinBox, *InputPhaseSpinBox;
	IIRfilter2ndOrder *IIR0, *IIR1;

private slots:
	void on_LockinInputChanged();
	void on_InputFreqChanged();
	void on_InputPhaseChanged();
};

#endif // SIMPLEPILOCKIN_H