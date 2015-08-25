#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

#include "LockBoxSimplePI.h"
#include "NPlocker.h"
#include "NP_adjuster.h"
#include "common.h"

#include <stdexcept>

using namespace std;

NPlocker::NPlocker(LockBoxSimplePI *parent, unsigned int configOffset, QString outputName, QStringList *channelNames, double *p_dataIn, int N_dataIn) :
  QWidget(0),
  parent(parent),
  configOffset(configOffset),
  p_dataIn(p_dataIn),
  N_dataIn(N_dataIn),
  Tmin(50),
  Tmax(60),
  SETTING_WIDGETS_FROM_CONFIG(false),
  adjNP(0)
{
  // Make the widget
  int labelSize = 70;
  int widgetSize = 230;

  QGroupBox *mainGroupBox = new QGroupBox(outputName);
  {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mainGroupBox);
  }

  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainGroupBox->setLayout(mainLayout);

  // Port
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
	QLabel *label = new QLabel(tr("Port: COM"));
    PortSpinBox = new QSpinBox();

    label->setMinimumSize(labelSize, 0);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    PortSpinBox->setRange(0, 99);
	PortSpinBox->setValue(99);
    // QObject::connect(Port, SIGNAL(valueChanged(double)), this, SLOT(setLaserTemp(double)));

    hLayout->addWidget(label);
    hLayout->addWidget(PortSpinBox);
    mainLayout->addLayout(hLayout);
  }

	// Input name
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Command:"));
		CommandLineEdit = new QLineEdit();

		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		CommandLineEdit->setMaxLength(16);
		CommandLineEdit->setMinimumSize(widgetSize, 0);
		CommandLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		hLayout->addWidget(label);
		hLayout->addWidget(CommandLineEdit);
		mainLayout->addLayout(hLayout);
	}
  
  // Laser temperature
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    QLabel *label = new QLabel(tr("Laser temp:"));
    LaserTemp = new QDoubleSpinBox();

    label->setMinimumSize(labelSize, 0);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    LaserTemp->setSuffix(tr(" C"));
    LaserTemp->setRange(Tmin, Tmax);
    LaserTemp->setDecimals(3);
    LaserTemp->setSingleStep(0.002);
    QObject::connect(LaserTemp, SIGNAL(valueChanged(double)), this, SLOT(setLaserTemp(double)));

    hLayout->addWidget(label);
    hLayout->addWidget(LaserTemp);
    mainLayout->addLayout(hLayout);
  }

  // Servo on/off
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    OnOffPushButton = new QPushButton(tr("Servo laser temp"));
    EditPushButton = new QPushButton(tr("Edit parameters"));

    OnOffPushButton->setCheckable(true);

    EditPushButton->setCheckable(true);
    EditPushButton->setChecked(true);

    hLayout->addWidget(OnOffPushButton);
    hLayout->addWidget(EditPushButton);
    mainLayout->addLayout(hLayout);
  }

  // Input
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    QLabel *label0 = new QLabel(tr("Input:"));
    SignComboBox = new QComboBox();
    QLabel *label1 = new QLabel(tr("("));
    InputComboBox = new QComboBox();
    QLabel *label2 = new QLabel(tr("-"));
    OffsetSpinBox = new QDoubleSpinBox();
    QLabel *label3 = new QLabel(tr(")"));

    label0->setMinimumSize(labelSize, 0);
    label0->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label3->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    SignComboBox->addItem(tr("+"));
    SignComboBox->addItem(tr("-"));
    SignComboBox->setCurrentIndex(1);

    for (int i = 0; i < channelNames->length(); i ++)
    {
      InputComboBox->addItem(channelNames->value(i));
    }
    InputComboBox->setCurrentIndex(6);

    OffsetSpinBox->setSuffix(tr(" V"));
    OffsetSpinBox->setRange(-10, 10);
    OffsetSpinBox->setDecimals(1);
    OffsetSpinBox->setSingleStep(0.1);
    OffsetSpinBox->setValue(5);

    hLayout->addWidget(label0);
    hLayout->addWidget(SignComboBox);
    hLayout->addWidget(label1);
    hLayout->addWidget(InputComboBox);
    hLayout->addWidget(label2);
    hLayout->addWidget(OffsetSpinBox);
    hLayout->addWidget(label3);
    mainLayout->addLayout(hLayout);

    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), label0, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), SignComboBox, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), label1, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), InputComboBox, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), label2, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), OffsetSpinBox, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), label3, SLOT(setVisible(bool)));
  }

  // Gain
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    GainSpinBox = new QDoubleSpinBox();
    QLabel *label = new QLabel(tr("Gain:"));

    label->setMinimumSize(labelSize, 0);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    GainSpinBox->setSuffix(tr(" mK/V"));
    GainSpinBox->setRange(0.1, 100);
    GainSpinBox->setDecimals(1);
    GainSpinBox->setSingleStep(1);
    GainSpinBox->setValue(0.5);

    hLayout->addWidget(label);
    hLayout->addWidget(GainSpinBox);
    mainLayout->addLayout(hLayout);

    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), label, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), GainSpinBox, SLOT(setVisible(bool)));
  }

  // Hold
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    HoldCheckBox = new QCheckBox(tr("Hold:"));
    HoldComboBox = new QComboBox();

    HoldCheckBox->setMinimumSize(labelSize, 0);
    HoldCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    HoldCheckBox->setChecked(true);

    for (int i = 0; i < channelNames->length(); i ++)
    {
      HoldComboBox->addItem(channelNames->value(i));
    }
    HoldComboBox->setCurrentIndex(12);
    HoldComboBox->setEnabled(true);

    hLayout->addWidget(HoldCheckBox);
    hLayout->addWidget(HoldComboBox);
    mainLayout->addLayout(hLayout);

    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), HoldCheckBox, SLOT(setVisible(bool)));
    QObject::connect(EditPushButton, SIGNAL(toggled(bool)), HoldComboBox, SLOT(setVisible(bool)));
  }

  // Hide the expandable menu
  EditPushButton->setChecked(false);

	// Hook up on_LaserParametersChanged
	QObject::connect(PortSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_LaserParametersChanged()));
	QObject::connect(CommandLineEdit, SIGNAL(returnPressed()), this, SLOT(on_LaserParametersChanged()));
	
	// Hook up on_InputParametersChanged
	QObject::connect(OnOffPushButton, SIGNAL(toggled(bool)), this, SLOT(on_InputParametersChanged()));
	QObject::connect(SignComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_InputParametersChanged()));
	QObject::connect(InputComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_InputParametersChanged()));
	QObject::connect(OffsetSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_InputParametersChanged()));

	// Hook up on_GainParametersChanged
	QObject::connect(GainSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_GainParametersChanged()));
	
	// Hook up on_HoldParametersChanged
	QObject::connect(HoldCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_HoldParametersChanged()));
	QObject::connect(HoldComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_HoldParametersChanged()));

	setAnalogEditable(false);
}

void NPlocker::setLaserTemp(double)
{
	if (!adjNP)
		return;

	adjNP->setTemperature(LaserTemp->value());
}

void NPlocker::updateLock()
{
	if (PortSpinBox->value() == 99)
		return;

	if (!adjNP)
	{
		// Establish communication with the laser
		char buff [50];
		int n = sprintf(buff, "COM%d", PortSpinBox->value());
		const std::string port_string(buff, n);
		// printf("port_string = %s\n", port_string.c_str());
		adjNP = new NP_adjuster(port_string, 57600, CommandLineEdit->text().toStdString(), Tmin, Tmax, 0.002); // ":SYST:TCON2"
	}

	LaserTemp->setValue(adjNP->getTemperature());

  // If the lock is on
  if (OnOffPushButton->isChecked())
  {
    // If we're not holding
    double hold = *(p_dataIn + N_dataIn*(HoldComboBox->currentIndex()));
    // printf("NPlocker::updateLock: hold = %f\n", hold);
    if (!(HoldCheckBox->isChecked()) || (hold == 0))
    {
      // If the last adjustment was > 20 s ago
      if (adjNP->timeOfLastAdj.elapsed() > 20000)
      {
        double input = *(p_dataIn + N_dataIn*(InputComboBox->currentIndex()));
        double errorSignal;
        if (SignComboBox->currentIndex() == 0)
          errorSignal = input - OffsetSpinBox->value();
        else
          errorSignal = -(input - OffsetSpinBox->value());
        // printf("NPlocker::updateLock: input = %f, errorSignal = %f\n", input, errorSignal);

        try
        {
          if (fabs(errorSignal) > 1)
            adjNP->shiftTemperature(0.001*(GainSpinBox->value())*errorSignal);
        }
        catch (runtime_error e) {}
      }
    }
  }
}

void NPlocker::setAnalogEditable(bool editable)
{
	PortSpinBox->setEnabled(editable);
	CommandLineEdit->setEnabled(editable);
}

void NPlocker::on_LaserParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset + 10, 15, 0, PortSpinBox->value());

	QString command = CommandLineEdit->text();
	command = command.leftJustified(16, ' ');
	QByteArray commandAscii = command.toLatin1();
	for (int i = 0; i < 8; i++)
	{
		parent->set_config(configOffset + i, (commandAscii[2*i+1] << 8) | commandAscii[2*i]);
	}
}

void NPlocker::on_InputParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset + 11, 0, 0, OnOffPushButton->isChecked());
	parent->set_config(configOffset + 11, 1, 1, SignComboBox->currentIndex());
	parent->set_config(configOffset + 11, 4, 2, InputComboBox->currentIndex());

	double minVal = -10;
	double maxVal = 10;
	long long int offset = -32768 + round((65535.0/(maxVal - minVal))*(OffsetSpinBox->value() - minVal));
	parent->set_config(configOffset + 12, 15, 0, offset);
}

void NPlocker::on_GainParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	double minVal = 0;
	double maxVal = 100;
	long long int gain = -32768 + round((65535.0/(maxVal - minVal))*(GainSpinBox->value() - minVal));
	parent->set_config(configOffset + 14, 15, 0, gain);
}

void NPlocker::on_HoldParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	parent->set_config(configOffset + 13, 0, 0, HoldCheckBox->isChecked());
	parent->set_config(configOffset + 13, 4, 1, HoldComboBox->currentIndex());
	
	HoldComboBox->setEnabled(HoldCheckBox->isChecked());
}

void NPlocker::set_widgets_from_config()
{
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	// Laser
	{
		PortSpinBox->setValue(parent->config(configOffset + 10, 15, 0, 1));

		QByteArray commandAscii;
		commandAscii.resize(16);
		for (int i = 0; i < 8; i++)
		{
			commandAscii[2*i] = parent->config(configOffset + i) & 0x00FF;
			commandAscii[2*i+1] = (parent->config(configOffset + i) & 0xFF00) >> 8;
		}
		QString command = QString::fromLatin1(commandAscii, 16);
		CommandLineEdit->setText(command);
	}
	
	// Loop filter input
	{
		OnOffPushButton->setChecked(parent->config(configOffset + 11, 0, 0));
		SignComboBox->setCurrentIndex(parent->config(configOffset + 11, 1, 1));
		InputComboBox->setCurrentIndex(parent->config(configOffset + 11, 4, 2));
	
		double minVal = -10;
		double maxVal = 10;
		double offset = (double(parent->config(configOffset + 12, 15, 0, 1)) + 32768.0)*(maxVal - minVal)/65535.0 + minVal;
		OffsetSpinBox->setValue(offset);
	}

	// Gain
	{
		double minVal = 0;
		double maxVal = 100;
		double gain = (double(parent->config(configOffset + 14, 15, 0, 1)) + 32768.0)*(maxVal - minVal)/65535.0 + minVal;
		GainSpinBox->setValue(gain);
	}

	// Loop filter hold
	{
		HoldCheckBox->setChecked(parent->config(configOffset + 13, 0, 0));
		HoldComboBox->setCurrentIndex(parent->config(configOffset + 13, 4, 1));
	
		HoldComboBox->setEnabled(HoldCheckBox->isChecked());
	}

	SETTING_WIDGETS_FROM_CONFIG = false;
}