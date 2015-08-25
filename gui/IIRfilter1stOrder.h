#ifndef IIRFILTER1STORDER_H
#define IIRFILTER1STORDER_H

#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "VariableDoubleSpinBox.h"
#include "VariableLongLongSpinBox.h"
#include <QSpinBox>

class LockBox;

class IIRfilter1stOrder : public QWidget
{
    Q_OBJECT

public:
    IIRfilter1stOrder(LockBox *parent, unsigned int configOffset, int a0shift, bool alwaysOn, QStringList *filterTypes, int labelSize, int widgetSize);

	void set_widgets_from_config();
	void set_widgets_from_config(double gainMultiplierIn);

	void setGainMultiplier(double gainMultiplierIn);
	
protected:
	LockBox *parent;
	unsigned int configOffset;
	bool SETTING_WIDGETS_FROM_CONFIG, SETTING_WIDGETS_VISIBLE, SETTING_COEFFICIENTS_FROM_PARAMETERS, SETTING_PARAMETERS_FROM_COEFFICIENTS;
	
	QCheckBox *FilterOnCheckBox;
	QComboBox *FilterTypeComboBox;
	QLabel *freqLabel, *gainLabel, *gainLimitLabel, *a0Label, *a0SpinBox, *a1Label, *b0Label, *b1Label;
	VariableDoubleSpinBox *freqSpinBox, *gainSpinBox, *gainLimitSpinBox;
	VariableLongLongSpinBox *a1SpinBox, *b0SpinBox, *b1SpinBox;

	qint64 a0;
	bool alwaysOn;
	double gainMultiplier;

	void set_widgets_visible();
	void set_config_from_coefficients();
	void set_coefficients_from_parameters();
	void set_parameters_from_coefficients();

private slots:
	void on_FilterTypeChanged();
	void on_FilterParametersChanged();
	void on_CoefficientsChanged();
};

#endif // IIRFILTER1STORDER_H