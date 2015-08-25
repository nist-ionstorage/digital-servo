#ifndef LOCKBOXSIMPLEPI_H
#define LOCKBOXSIMPLEPI_H

#include <QVector>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>

#include "LockBox.h"
#include "SimplePIinput.h"
#include "SimplePIlockin.h"
#include "SimplePIphasedetector.h"
#include "SimplePIoutput.h"
#include "ScopePlot.h"

#include "NPlocker.h"
#include "MonitorDataLogger.h"

//! Lock box simple PI
class LockBoxSimplePI : public LockBox
{
    Q_OBJECT

public:
    LockBoxSimplePI(QWidget *parent = 0, int deviceIndex = 0);

	virtual void load_config_from_flash();
	virtual bool load_config_from_file(const QString &fileName);
	void set_widgets_from_config();

	virtual double getAnalogSignalRangeMin(int i);
	virtual double getAnalogSignalRangeMax(int i);
	virtual void analogSignalRangeModified();
	
protected:
	QStringList monitorNames;

	virtual void timerEvent(QTimerEvent *e);

	QVector<QLabel*> configLabels;
    QVector<QSpinBox*> configSpinBoxes;

	SimplePIinput *AIN0, *AIN1;
	SimplePIlockin *LOCKIN;
	SimplePIphasedetector *PHASEDET;
	SimplePIoutput *AOUT0, *AOUT1, *AOUT2;
	NPlocker *lockNP;

	ScopePlot *plot;
	MonitorDataLogger *logger;

	bool SETTING_WIDGETS_FROM_CONFIG;

private slots:
	void on_configSpinBox_valueChanged(int new_configuration);
	void setAnalogEditable(bool editable);
	void setLogData(bool bLogData);
};

#endif // LOCKBOXSIMPLEPI_H