#ifndef SIMPLEPIINPUT_H
#define SIMPLEPIINPUT_H

#include "SimplePIgroupBox.h"
#include <QComboBox>
#include "IIRfilter1stOrder.h"

class LockBox;

class SimplePIinput : public SimplePIgroupBox
{
    Q_OBJECT

public:
    SimplePIinput(LockBox *parent, unsigned int configOffset, QString inputName, QStringList *inputRanges);

	void set_widgets_from_config();

	double getInputRangeMin();
	double getInputRangeMax();
	void analogSignalRangeModified();
	void setAnalogEditable(bool editable);
	
protected:
	QComboBox *InputRangeComboBox;
	IIRfilter1stOrder *IIR0;

private slots:
	void on_InputParametersChanged();
};

#endif // SIMPLEPIINPUT_H