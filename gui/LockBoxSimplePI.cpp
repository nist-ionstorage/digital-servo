#include <QGroupBox>
#include <QScrollArea>
#include <QInputDialog>

#include <windows.h>

#include "LockBoxSimplePI.h"

#define N_CONFIG_SPIN_BOXES 0

LockBoxSimplePI::LockBoxSimplePI(QWidget *parent, int deviceIndex) :
	LockBox(parent, deviceIndex),
	configLabels(N_CONFIG_SPIN_BOXES),
	configSpinBoxes(N_CONFIG_SPIN_BOXES),
	lockNP(0),
	SETTING_WIDGETS_FROM_CONFIG(false)
{
    // Names of monitor channels
	monitorNames << QString("AIN0");
	monitorNames << QString("AIN1");
	monitorNames << QString("LOCKIN");
	monitorNames << QString("PHASEDET");
	monitorNames << QString("AOUT0");
	monitorNames << QString("AOUT1");
	monitorNames << QString("AOUT2");
	monitorNames << QString("DIN0");
	monitorNames << QString("DIN1");
	monitorNames << QString("DIN2");
	monitorNames << QString("LOOP FILTER 0");
	monitorNames << QString("LOOP FILTER 1");
	monitorNames << QString("LOOP FILTER 2");
	
	// first the extra configuration spinboxes
	for (int i = 0; i < N_CONFIG_SPIN_BOXES; i++)
	{
		configLabels[i] = new QLabel("Configuration[" + QString::number(i) + "]:");
		configSpinBoxes[i] = new QSpinBox();
		configSpinBoxes[i]->setRange(0, 65535);

		QObject::connect(configSpinBoxes[i], SIGNAL(valueChanged(int)), this, SLOT(on_configSpinBox_valueChanged(int)));
	}
	
	QVBoxLayout *vLayout = new QVBoxLayout();
	for (int i = 0; i < N_CONFIG_SPIN_BOXES; i++)
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		hLayout->addWidget(configLabels[i]);
		hLayout->addWidget(configSpinBoxes[i]);
		
		vLayout->addLayout(hLayout);
	}
	vLayout->addStretch();
	
	// now the ADCs
	QStringList inputRanges;
	inputRanges << QString("+/-0.5 V");
	inputRanges << QString("+/-1 V");
	inputRanges << QString("+/-2 V");
	inputRanges << QString("+/-4 V");

	AIN0 = new SimplePIinput(this, 128, "AIN0", &inputRanges);
	AIN1 = new SimplePIinput(this, 192, "AIN1", &inputRanges);

	QVBoxLayout *ADClayout = new QVBoxLayout();
	ADClayout->addWidget(AIN0);
	ADClayout->addWidget(AIN1);
	
	// now the lockin
	LOCKIN = new SimplePIlockin(this, 256, "LOCKIN");
	ADClayout->addWidget(LOCKIN);

	// now the phase detector
	PHASEDET = new SimplePIphasedetector(this, 320, "PHASEDET");
	ADClayout->addWidget(PHASEDET);

	ADClayout->addStretch();

	// now the DACs
	QStringList analogSignalNames;
	analogSignalNames << QString("AIN0");
	analogSignalNames << QString("AIN1");
	analogSignalNames << QString("AIN0-AIN1");
	analogSignalNames << QString("LOCKIN");
	analogSignalNames << QString("PHASEDET");
	analogSignalNames << QString("AOUT0");
	analogSignalNames << QString("AOUT1");
	analogSignalNames << QString("AOUT2");

	QStringList digitalSignalNames;
	digitalSignalNames << QString("LOOP FILTER 0");
	digitalSignalNames << QString("LOOP FILTER 1");
	digitalSignalNames << QString("LOOP FILTER 2");
	digitalSignalNames << QString("DIN0");
	digitalSignalNames << QString("DIN1");
	digitalSignalNames << QString("DIN2");
	digitalSignalNames << QString("!DIN0");
	digitalSignalNames << QString("!DIN1");
	digitalSignalNames << QString("!DIN2");

	QStringList fastOutputRanges;
	fastOutputRanges << QString("+/-1 V");
	fastOutputRanges << QString("+/-2 V");
	fastOutputRanges << QString("+/-4 V");
	fastOutputRanges << QString("+/-8 V");

	QStringList slowOutputRanges;
	slowOutputRanges << QString("+10 V");
	
	AOUT0 = new SimplePIoutput(this, 384, "AOUT0", &fastOutputRanges, &analogSignalNames, &digitalSignalNames);
	AOUT1 = new SimplePIoutput(this, 512, "AOUT1", &fastOutputRanges, &analogSignalNames, &digitalSignalNames);
	AOUT2 = new SimplePIoutput(this, 640, "AOUT2", &slowOutputRanges, &analogSignalNames, &digitalSignalNames);

	QVBoxLayout *DAClayout = new QVBoxLayout();
	DAClayout->addWidget(AOUT0);
	DAClayout->addWidget(AOUT1);
	DAClayout->addWidget(AOUT2);
	
	// now the fiber laser
	if (getID().toStdString().find("(NP)") != std::string::npos)
	{
		lockNP = new NPlocker(this, 96, "Fiber Laser", &monitorNames, &monitorData[0][0], N_MONITOR_DATA);
		DAClayout->addWidget(lockNP);
	}

	logger = new MonitorDataLogger(&monitorNames, &monitorData[0][0], N_MONITOR_DATA);

	DAClayout->addStretch();

	// now the plot
    plot = new ScopePlot(this, 0, &monitorNames, &monitorData[0][0], N_MONITOR_DATA);

	QHBoxLayout *scrollLayout = new QHBoxLayout();
	scrollLayout->addLayout(vLayout);
	scrollLayout->addLayout(ADClayout);
	scrollLayout->addLayout(DAClayout);
	
	QGroupBox *scrollGroupBox = new QGroupBox();
	scrollGroupBox->setLayout(scrollLayout);
	scrollGroupBox->setMinimumSize(0, 2000);
	scrollGroupBox->setFlat(true);

	QScrollArea *scrollArea = new QScrollArea();
	scrollArea->setWidget(scrollGroupBox);
	scrollArea->setMinimumSize(634, 0);
	
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(scrollArea);
	mainLayout->addWidget(plot);

	for (int i = 0; i < N_CONFIG; i++)
	{
		get_config(i);
	}

	set_widgets_from_config();
}

void LockBoxSimplePI::on_configSpinBox_valueChanged(int new_configuration)
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;
	
	// not pretty, but this figures out which spinbox changed
	QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
	int i_spinBox = 0;
	for (int i = 0; i < N_CONFIG_SPIN_BOXES; i++)
	{
		if (spinBox == configSpinBoxes[i])
		{
			i_spinBox = i;
		}
	}
	
	set_config(i_spinBox, new_configuration);
}

void LockBoxSimplePI::setAnalogEditable(bool editable)
{
	AIN0->setAnalogEditable(editable);
	AIN1->setAnalogEditable(editable);
	LOCKIN->setAnalogEditable(editable);
	PHASEDET->setAnalogEditable(editable);
	AOUT0->setAnalogEditable(editable);
	AOUT1->setAnalogEditable(editable);
	AOUT2->setAnalogEditable(editable);
	
	if (lockNP)
		lockNP->setAnalogEditable(editable);
}

void LockBoxSimplePI::setLogData(bool bLogData)
{
	unsigned int logEvery = 0;
	
	if (bLogData)
	{
		double logPeriod = QInputDialog::getDouble(this,
						tr("Log data"),
						tr("Period [s]:"),
						1, 0.2, 1000, 1);
		logEvery = qRound(logPeriod*1000.0/T_MONITOR_TIMER);
	}

	logger->setConfig(bLogData, logEvery);
}

void LockBoxSimplePI::load_config_from_flash()
{
	LockBox::load_config_from_flash();

	set_widgets_from_config();
}

bool LockBoxSimplePI::load_config_from_file(const QString &fileName)
{
	bool success = LockBox::load_config_from_file(fileName);

	set_widgets_from_config();

	return success;
}

void LockBoxSimplePI::set_widgets_from_config()
{
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	for (int i = 0; i < N_CONFIG_SPIN_BOXES; i++)
	{
		configSpinBoxes[i]->setValue(configuration[i]);
	}

	AIN0->set_widgets_from_config();
	AIN1->set_widgets_from_config();
	
	LOCKIN->set_widgets_from_config();
	PHASEDET->set_widgets_from_config();
	
	AOUT0->set_widgets_from_config();
	AOUT1->set_widgets_from_config();
	AOUT2->set_widgets_from_config();

	plot->set_widgets_from_config();

	if (lockNP)
		lockNP->set_widgets_from_config();
	
	SETTING_WIDGETS_FROM_CONFIG = false;
}

double LockBoxSimplePI::getAnalogSignalRangeMin(int i)
{
	switch(i)
	{
	case 0:
		return AIN0->getInputRangeMin();
	case 1:
		return AIN1->getInputRangeMin();
	case 2:
		return AIN0->getInputRangeMin();
	case 3:
		return LOCKIN->getInputRangeMin();
	case 4:
		return PHASEDET->getInputRangeMin();
	case 5:
		return AOUT0->getOutputRangeMin();
	case 6:
		return AOUT1->getOutputRangeMin();
	case 7:
		return AOUT2->getOutputRangeMin();
	default:
		return 0;
	}
}

double LockBoxSimplePI::getAnalogSignalRangeMax(int i)
{
	switch(i)
	{
	case 0:
		return AIN0->getInputRangeMax();
	case 1:
		return AIN1->getInputRangeMax();
	case 2:
		return AIN0->getInputRangeMax();
	case 3:
		return LOCKIN->getInputRangeMax();
	case 4:
		return PHASEDET->getInputRangeMax();
	case 5:
		return AOUT0->getOutputRangeMax();
	case 6:
		return AOUT1->getOutputRangeMax();
	case 7:
		return AOUT2->getOutputRangeMax();
	default:
		return 0;
	}
}

void LockBoxSimplePI::analogSignalRangeModified()
{
	AIN0->analogSignalRangeModified();
	AIN1->analogSignalRangeModified();
	AOUT0->analogSignalRangeModified();
	AOUT1->analogSignalRangeModified();
	AOUT2->analogSignalRangeModified();
}

void LockBoxSimplePI::timerEvent(QTimerEvent *e)
{
	if(timerId < 0)
		return;

	time++;

	// Get new monitor data from the XEM6010
	unsigned char rawData[2*N_MONITOR_CHANNELS*N_MONITOR_DATA];
	dev->ReadFromPipeOut(0xA0, 2*N_MONITOR_CHANNELS*N_MONITOR_DATA, rawData);
	
	// printf("%i, %i, %i, %i, %i, %i, %i, %i\n", int(rawData[0]), int(rawData[1]), int(rawData[2]), int(rawData[3]), int(rawData[30]), int(rawData[31]), int(rawData[2*N_MONITOR_CHANNELS*N_MONITOR_DATA-2]), int(rawData[2*N_MONITOR_CHANNELS*N_MONITOR_DATA-1]));
	
	// first stuff the analog channels
	for (int i = 0; i < N_MONITOR_DATA; i++)
    {
		// AIN
        for(int j = 0; j < 2; j++)
		{
			monitorData[j][i] = 256.*double(rawData[2*(j + 3) + 2*N_MONITOR_CHANNELS*i + 0]) + double(rawData[2*(j + 3) + 2*N_MONITOR_CHANNELS*i + 1]);
			
			// take care of twos compliment negative numbers
			if (monitorData[j][i] > 32767)
			{
				monitorData[j][i] = -(65536 - monitorData[j][i]);
			}

			// scale to voltage
			monitorData[j][i] = (getAnalogSignalRangeMin(j) + getAnalogSignalRangeMax(j))/2.0 + (getAnalogSignalRangeMax(j) - getAnalogSignalRangeMin(j))*monitorData[j][i]/65536.0;
		}

		// LOCKIN
        {
			int j = 2;
			monitorData[j][i] = 256.*double(rawData[2*(j + 3) + 2*N_MONITOR_CHANNELS*i + 0]) + double(rawData[2*(j + 3) + 2*N_MONITOR_CHANNELS*i + 1]);
			
			// take care of twos compliment negative numbers
			if (monitorData[j][i] > 32767)
			{
				monitorData[j][i] = -(65536 - monitorData[j][i]);
			}

			// scale to voltage
			monitorData[j][i] = (getAnalogSignalRangeMin(j+1) + getAnalogSignalRangeMax(j+1))/2.0 + (getAnalogSignalRangeMax(j+1) - getAnalogSignalRangeMin(j+1))*monitorData[j][i]/65536.0;
		}

		// PHASEDET
		{
			int j = 3;
			monitorData[j][i] = 256.*double(rawData[2*6 + 2*N_MONITOR_CHANNELS*i + 0]) +        double(rawData[2*6 + 2*N_MONITOR_CHANNELS*i + 1]) +
						   16777216.*double(rawData[2*7 + 2*N_MONITOR_CHANNELS*i + 0]) + 65536.*double(rawData[2*7 + 2*N_MONITOR_CHANNELS*i + 1]);
		
			// take care of twos compliment negative numbers
			if (monitorData[j][i] > 2147483647)
			{
				monitorData[j][i] = -(4294967296. - monitorData[j][i]);
			}

			// scale to voltage
			monitorData[j][i] = (getAnalogSignalRangeMin(j+1) + getAnalogSignalRangeMax(j+1))/2.0 + (getAnalogSignalRangeMax(j+1) - getAnalogSignalRangeMin(j+1))*monitorData[j][i]/65536.0;
		}

		// AOUT
		for(int j = 4; j < 7; j++)
		{
			monitorData[j][i] = 256.*double(rawData[2*(j + 4) + 2*N_MONITOR_CHANNELS*i + 0]) + double(rawData[2*(j + 4) + 2*N_MONITOR_CHANNELS*i + 1]);
			
			// take care of twos compliment negative numbers
			if (monitorData[j][i] > 32767)
			{
				monitorData[j][i] = -(65536 - monitorData[j][i]);
			}

			// scale to voltage
			monitorData[j][i] = (getAnalogSignalRangeMin(j+1) + getAnalogSignalRangeMax(j+1))/2.0 + (getAnalogSignalRangeMax(j+1) - getAnalogSignalRangeMin(j+1))*monitorData[j][i]/65536.0;
		}
    }

	// printf("%f, %f, %f, %f, %f, %f, %i, %i, %i, %i, %i, %i\n", monitorData[0][0],  monitorData[0][1],  monitorData[0][2],  monitorData[0][253],  monitorData[0][254],  monitorData[0][255], int(rawData[30]), int(rawData[31]), int(rawData[2*N_MONITOR_CHANNELS*(N_MONITOR_DATA-1)-2]), int(rawData[2*N_MONITOR_CHANNELS*(N_MONITOR_DATA-1)-1]), int(rawData[2*N_MONITOR_CHANNELS*N_MONITOR_DATA-2]), int(rawData[2*N_MONITOR_CHANNELS*N_MONITOR_DATA-1]));
	
	// then stuff the digital channels
	for (int i = 0; i < N_MONITOR_DATA; i++)
    {
        for(int j = 7; j < 13; j++)
		{
			monitorData[j][i] = double((rawData[2*2 + 2*N_MONITOR_CHANNELS*i + 1] >> (j - 7)) & 0x01);
		}
    }

	// check that the data arrived intact
	if ((rawData[30] == 0x23) && (rawData[31] == 0x23) && (rawData[2*N_MONITOR_CHANNELS*(N_MONITOR_DATA-1)-2] == 0x23) && (rawData[2*N_MONITOR_CHANNELS*(N_MONITOR_DATA-1)-1] == 0x23))
	{
		plot->updatePlot(); // update the plot
		logger->updateLog(); // update the logger

		if (lockNP)
			lockNP->updateLock(); // update the laser temperature servo
	}
	else
		printf("Bad monitor data received\n");

	
}