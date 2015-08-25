#ifndef IIRFILTER2NDORDER_H
#define IIRFILTER2NDORDER_H

#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "VariableDoubleSpinBox.h"
#include "VariableLongLongSpinBox.h"
#include <QSpinBox>

class LockBox;

class IIRfilter2ndOrder : public QWidget
{
    Q_OBJECT

public:
    IIRfilter2ndOrder(LockBox *parent, unsigned int configOffset, int a0shift, int updateEvery, bool alwaysOn, QStringList *filterTypes, int labelSize, int widgetSize);

	void set_widgets_from_config();

protected:
	LockBox *parent;
	unsigned int configOffset;
	bool SETTING_WIDGETS_FROM_CONFIG, SETTING_WIDGETS_VISIBLE, SETTING_COEFFICIENTS_FROM_PARAMETERS, SETTING_PARAMETERS_FROM_COEFFICIENTS;
	
	QCheckBox *FilterOnCheckBox;
	QComboBox *FilterTypeComboBox;
	QLabel *freqLabel, *qualityFactorLabel, *gainLabel, *gainLimitLabel, *a0Label, *a0SpinBox, *a1Label, *a2Label, *b0Label, *b1Label, *b2Label;
	VariableDoubleSpinBox *freqSpinBox, *qualityFactorSpinBox, *gainSpinBox, *gainLimitSpinBox;
	VariableLongLongSpinBox *a1SpinBox, *a2SpinBox, *b0SpinBox, *b1SpinBox, *b2SpinBox;

	qint64 a0;
	double updateEvery;
	bool alwaysOn;

	void set_widgets_visible();
	void set_config_from_coefficients();
	void set_coefficients_from_parameters();
	void set_parameters_from_coefficients();

private slots:
	void on_FilterTypeChanged();
	void on_FilterParametersChanged();
	void on_CoefficientsChanged();
};

#endif // IIRFILTER2NDORDER_H