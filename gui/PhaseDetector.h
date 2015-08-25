#ifndef PHASEDETECTOR_H
#define PHASEDETECTOR_H

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include "VariableDoubleSpinBox.h"

class LockBox;

class PhaseDetector : public QWidget
{
    Q_OBJECT

public:
    PhaseDetector(LockBox *parent, unsigned int configOffset, QString inputName, QStringList *inputRanges);

	void set_widgets_from_config();
	
protected:
	LockBox *parent;
	unsigned int configOffset;
	bool SETTING_WIDGETS_FROM_CONFIG;

	QComboBox *InputRangeComboBox;
	QCheckBox *LPCheckBox;
	VariableDoubleSpinBox *LPFreqSpinBox, *InputFreqSpinBox;

private slots:
	void on_InputParametersChanged();
	void on_InputFreqChanged();
	void on_LPParametersChanged();
};

#endif // PHASEDETECTOR_H