#include <QtGui>
#include <QtWidgets>

#include "MeasureTransferFunctionDialog.h"

MeasureTransferFunctionDialog::MeasureTransferFunctionDialog(QWidget *parent)
{
	int labelSize = 200;
	int widgetSize = 200;
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	setLayout(mainLayout);

	// Modulation frequency start
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Modulation frequency start:"));
		modFreqStartSpinBox = new QDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		modFreqStartSpinBox->setSuffix(tr(" Hz"));
		modFreqStartSpinBox->setRange(10, 2000000);
		modFreqStartSpinBox->setDecimals(0);
		modFreqStartSpinBox->setSingleStep(10);
		modFreqStartSpinBox->setValue(1000);
		
		hLayout->addWidget(label);
		hLayout->addWidget(modFreqStartSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// Modulation frequency stop
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Modulation frequency stop:"));
		modFreqStopSpinBox = new QDoubleSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		modFreqStopSpinBox->setSuffix(tr(" Hz"));
		modFreqStopSpinBox->setRange(10, 2000000);
		modFreqStopSpinBox->setDecimals(0);
		modFreqStopSpinBox->setSingleStep(10);
		modFreqStopSpinBox->setValue(1000000);
		
		hLayout->addWidget(label);
		hLayout->addWidget(modFreqStopSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// Modulation frequency steps
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Modulation frequency steps:"));
		modFreqStepsSpinBox = new QSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		modFreqStepsSpinBox->setRange(0, 1000);
		modFreqStepsSpinBox->setSingleStep(1);
		modFreqStepsSpinBox->setValue(100);
		
		hLayout->addWidget(label);
		hLayout->addWidget(modFreqStepsSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// Modulation amplitude
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Modulation amplitude:"));
		modAmpSpinBox = new QSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		modAmpSpinBox->setSuffix(tr(" dB"));
		modAmpSpinBox->setRange(0, 24);
		modAmpSpinBox->setSingleStep(1);
		modAmpSpinBox->setValue(0);
		
		hLayout->addWidget(label);
		hLayout->addWidget(modAmpSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// Output channel
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Modulation output channel:"));
		modOutChanComboBox = new QComboBox();

		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		modOutChanComboBox->addItem("AOUT0");
		modOutChanComboBox->addItem("AOUT1");
		modOutChanComboBox->addItem("AOUT2");
		modOutChanComboBox->setMinimumSize(widgetSize, 0);
		modOutChanComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		
		hLayout->addWidget(label);
		hLayout->addWidget(modOutChanComboBox);
		mainLayout->addLayout(hLayout);
	}

	// Sample rate
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Sample rate:"));
		sampleRateSpinBox = new QSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		sampleRateSpinBox->setMinimum(1000);
		sampleRateSpinBox->setMaximum(6250000);
		sampleRateSpinBox->setValue(1000000);
		sampleRateSpinBox->setSingleStep(1000);
		sampleRateSpinBox->setSuffix(" Hz");

		hLayout->addWidget(label);
		hLayout->addWidget(sampleRateSpinBox);
		mainLayout->addLayout(hLayout);
	}

	// Number of samples
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(tr("Number of samples (per frequency):"));
		NsamplesSpinBox = new QSpinBox();
		
		label->setMinimumSize(labelSize, 0);
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		NsamplesSpinBox->setMinimum(1024);
		NsamplesSpinBox->setMaximum(4194304);
		NsamplesSpinBox->setValue(16384);
		NsamplesSpinBox->setSingleStep(1024);

		hLayout->addWidget(label);
		hLayout->addWidget(NsamplesSpinBox);
		mainLayout->addLayout(hLayout);

		connect(NsamplesSpinBox, SIGNAL(valueChanged(int)), this, SLOT(NsamplesChanged(int)));
	}

	// OK button
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		okButton = new QPushButton(tr("Start"));
		cancelButton = new QPushButton(tr("Cancel"));

		hLayout->addWidget(okButton);
		hLayout->addWidget(cancelButton);
		mainLayout->addLayout(hLayout);

		connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	}

	setWindowTitle(tr("Measure transfer function"));
    setWindowIcon(QIcon(":/images/icon.ico"));
	setModal(1);
}

double MeasureTransferFunctionDialog::getModFreqStart()
{
	return modFreqStartSpinBox->value();
}

double MeasureTransferFunctionDialog::getModFreqStop()
{
	return modFreqStopSpinBox->value();
}

int MeasureTransferFunctionDialog::getModFreqSteps()
{
	return modFreqStepsSpinBox->value();
}

int MeasureTransferFunctionDialog::getModAmp()
{
	return modAmpSpinBox->value();
}

int MeasureTransferFunctionDialog::getModOutChan()
{
	return modOutChanComboBox->currentIndex();
}

int MeasureTransferFunctionDialog::getSampleRate()
{
	return sampleRateSpinBox->value();
}

int MeasureTransferFunctionDialog::getNsamples()
{
	return NsamplesSpinBox->value();
}

void MeasureTransferFunctionDialog::setModFreqStart(double modFreqStart)
{
	modFreqStartSpinBox->setValue(modFreqStart);
}

void MeasureTransferFunctionDialog::setModFreqStop(double modFreqStop)
{
	modFreqStopSpinBox->setValue(modFreqStop);
}

void MeasureTransferFunctionDialog::setModFreqSteps(int modFreqSteps)
{
	modFreqStepsSpinBox->setValue(modFreqSteps);
}

void MeasureTransferFunctionDialog::setModAmp(int modAmp)
{
	modAmpSpinBox->setValue(modAmp);
}

void MeasureTransferFunctionDialog::setModOutChan(int modOutChan)
{
	modOutChanComboBox->setCurrentIndex(modOutChan);
}

void MeasureTransferFunctionDialog::setSampleRate(int sampleRate)
{
	sampleRateSpinBox->setValue(sampleRate);
}

void MeasureTransferFunctionDialog::setNsamples(int Nsamples)
{
	NsamplesSpinBox->setValue(Nsamples);
}

void MeasureTransferFunctionDialog::NsamplesChanged(int newValue)
{
	if ((newValue % 1024) != 0)
	{
		NsamplesSpinBox->setValue(newValue - (newValue % 1024));
	}
}