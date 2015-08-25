#include <QtGui>
#include <QtWidgets>

#include "OptimizeTimingDialog.h"

OptimizeTimingDialog::OptimizeTimingDialog(QWidget *parent, QString windowTitle, QStringList headerLabels, QList< QList<unsigned int> > timingArray, QString timingLabel, int timingMin, int timingMax, int timingValue)
{
	unsigned int Nrow = timingArray.size();
	unsigned int Ncolumn = timingArray[0].size();
	
	timingTableWidget = new QTableWidget(Nrow, Ncolumn);
	timingTableWidget->setHorizontalHeaderLabels(headerLabels);

	for (unsigned int row = 0; row < Nrow; row++)
	{
		QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(timingArray[row][0], 10));
		timingTableWidget->setItem(row, 0, newItem);
			
		for (unsigned int column = 1; column < Ncolumn; column++)
		{
			QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(timingArray[row][column], 2));
			timingTableWidget->setItem(row, column, newItem);
		}
	}
	
	timingSpinBox = new QSpinBox();
	timingSpinBox->setMinimum(timingMin);
	timingSpinBox->setMaximum(timingMax);
	timingSpinBox->setValue(timingValue);
	timingSpinBox->setSingleStep(1);
	
	okButton = new QPushButton(tr("OK"));
	cancelButton = new QPushButton(tr("Cancel"));

	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *timingLayout = new QHBoxLayout;
	timingLayout->addWidget(new QLabel(timingLabel));
	timingLayout->addWidget(timingSpinBox);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(timingTableWidget);
	mainLayout->addLayout(timingLayout);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);

	setWindowTitle(windowTitle);
    setWindowIcon(QIcon(":/images/icon.ico"));
	setModal(1);
}

int OptimizeTimingDialog::getTiming()
{
	return timingSpinBox->value();
}