#include <QtGui>
#include <QtWidgets>

#include "RecordDataDialog.h"

RecordDataDialog::RecordDataDialog(QWidget *parent)
{
	sampleRateSpinBox = new QSpinBox();
	sampleRateSpinBox->setMinimum(1000);
	sampleRateSpinBox->setMaximum(6250000);
	sampleRateSpinBox->setValue(1000000);
	sampleRateSpinBox->setSingleStep(1000);
	sampleRateSpinBox->setSuffix(" Hz");

	NsamplesSpinBox = new QSpinBox();
	NsamplesSpinBox->setMinimum(1024);
	NsamplesSpinBox->setMaximum(4194304);
	NsamplesSpinBox->setValue(16384);
	NsamplesSpinBox->setSingleStep(1024);
	
	connect(NsamplesSpinBox, SIGNAL(valueChanged(int)), this, SLOT(NsamplesChanged(int)));
	
	okButton = new QPushButton(tr("OK"));
	cancelButton = new QPushButton(tr("Cancel"));

	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *sampleRateLayout = new QHBoxLayout;
	sampleRateLayout->addWidget(new QLabel(tr("Sample rate:")));
	sampleRateLayout->addWidget(sampleRateSpinBox);
	
	QHBoxLayout *NsamplesLayout = new QHBoxLayout;
	NsamplesLayout->addWidget(new QLabel(tr("Number of samples:")));
	NsamplesLayout->addWidget(NsamplesSpinBox);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(sampleRateLayout);
	mainLayout->addLayout(NsamplesLayout);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);

	setWindowTitle(tr("Record data"));
    setWindowIcon(QIcon(":/images/icon.ico"));
	setModal(1);
}

int RecordDataDialog::getSampleRate()
{
	return sampleRateSpinBox->value();
}

int RecordDataDialog::getNsamples()
{
	return NsamplesSpinBox->value();
}

void RecordDataDialog::setSampleRate(int sampleRate)
{
	sampleRateSpinBox->setValue(sampleRate);
}

void RecordDataDialog::setNsamples(int Nsamples)
{
	NsamplesSpinBox->setValue(Nsamples);
}

void RecordDataDialog::NsamplesChanged(int newValue)
{
	if ((newValue % 1024) != 0)
	{
		NsamplesSpinBox->setValue(newValue - (newValue % 1024));
	}
}