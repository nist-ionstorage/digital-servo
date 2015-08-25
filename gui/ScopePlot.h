#ifndef SCOPEPLOT_H
#define SCOPEPLOT_H

#include <QLabel>
#include <QComboBox>
#include "VariableDoubleSpinBox.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#define N_PLOT_CHANNELS 5
#define N_PLOT_DATA 101

class LockBox;

//! Lock box
class ScopePlot : public QWidget
{
    Q_OBJECT

public:
    ScopePlot(LockBox *parent, unsigned int configOffset, QStringList *channelNames, double *p_dataIn, int N_dataIn);

	void set_widgets_from_config();
	void updatePlot();

protected:
	LockBox *parent;
	unsigned int configOffset;
	bool SETTING_WIDGETS_FROM_CONFIG;

	void alignScales();

	QwtPlot *plot;
	QVector<QwtPlotCurve*> curves;

	QLabel *triggerChannelSelectLabel, *triggerLevelLabel;
	QComboBox *modeComboBox, *triggerChannelSelectComboBox;
	VariableDoubleSpinBox *triggerLevelSpinBox;

    QVector<QComboBox*> channelSelectComboBoxes;
	QVector<VariableDoubleSpinBox*> channelScaleSpinBoxes;
	QVector<VariableDoubleSpinBox*> channelOffsetSpinBoxes;

	double data_x[N_PLOT_DATA];
    double data_y[N_PLOT_CHANNELS][N_PLOT_DATA];

	double *p_dataIn;
	int N_dataIn;

private slots:
	void on_displayParametersChanged();

	void on_modeComboBox_currentIndexChanged(int index);
	void on_channelSelectComboBox_currentIndexChanged(int index);
};

#endif // SCOPEPLOT_H