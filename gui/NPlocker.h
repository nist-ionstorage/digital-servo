#ifndef NPLOCKER_H
#define NPLOCKER_H

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>

class NP_adjuster;
class LockBoxSimplePI;

class NPlocker : public QWidget
{
    Q_OBJECT

public:
    NPlocker(LockBoxSimplePI *parent, unsigned int configOffset, QString outputName, QStringList *channelNames, double *p_dataIn, int N_dataIn);

	void updateLock();

	void set_widgets_from_config();
	void setAnalogEditable(bool editable);
	
protected:
	double Tmin, Tmax;

	NP_adjuster *adjNP;

	LockBoxSimplePI *parent;
	unsigned int configOffset;
	bool SETTING_WIDGETS_FROM_CONFIG;

	QSpinBox *PortSpinBox;
	QLineEdit *CommandLineEdit;
	QDoubleSpinBox *LaserTemp, *OffsetSpinBox, *GainSpinBox;
	QPushButton *OnOffPushButton, *EditPushButton;
	QComboBox *SignComboBox, *InputComboBox, *HoldComboBox;
	QCheckBox *HoldCheckBox;

	double *p_dataIn;
	int N_dataIn;

protected slots:
	void on_LaserParametersChanged();
	void on_InputParametersChanged();
	void on_GainParametersChanged();
	void on_HoldParametersChanged();

	void setLaserTemp(double);
};

#endif // NPLOCKER_H