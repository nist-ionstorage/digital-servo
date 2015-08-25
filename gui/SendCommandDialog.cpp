#include <QtGui>
#include <QtWidgets>

#include "SendCommandDialog.h"

SendCommandDialog::SendCommandDialog(QWidget *parent)
{
	CmdAddrSpinBox = new HexSpinBox();
	CmdAddrSpinBox->setMinimum(0);
	CmdAddrSpinBox->setMaximum(65535);
	CmdAddrSpinBox->setValue(0);

	CmdData1SpinBox = new HexSpinBox();
	CmdData1SpinBox->setMinimum(0);
	CmdData1SpinBox->setMaximum(65535);
	CmdData1SpinBox->setValue(0);

	CmdData2SpinBox = new HexSpinBox();
	CmdData2SpinBox->setMinimum(0);
	CmdData2SpinBox->setMaximum(65535);
	CmdData2SpinBox->setValue(0);
	
	okButton = new QPushButton(tr("Send command"));
	cancelButton = new QPushButton(tr("Cancel"));

	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *CmdAddrLayout = new QHBoxLayout;
	CmdAddrLayout->addWidget(new QLabel(tr("Command address:")));
	CmdAddrLayout->addWidget(CmdAddrSpinBox);
	
	QHBoxLayout *CmdData1Layout = new QHBoxLayout;
	CmdData1Layout->addWidget(new QLabel(tr("Command data 1:")));
	CmdData1Layout->addWidget(CmdData1SpinBox);
	
	QHBoxLayout *CmdData2Layout = new QHBoxLayout;
	CmdData2Layout->addWidget(new QLabel(tr("Command data 2:")));
	CmdData2Layout->addWidget(CmdData2SpinBox);
	
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(CmdAddrLayout);
	mainLayout->addLayout(CmdData1Layout);
	mainLayout->addLayout(CmdData2Layout);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);

	setWindowTitle(tr("Send command"));
    setWindowIcon(QIcon(":/images/icon.ico"));
	setModal(1);
}

int SendCommandDialog::getCmdAddr()
{
	return CmdAddrSpinBox->value();
}

int SendCommandDialog::getCmdData1()
{
	return CmdData1SpinBox->value();
}

int SendCommandDialog::getCmdData2()
{
	return CmdData2SpinBox->value();
}

void SendCommandDialog::setCmdAddr(int i)
{
	CmdAddrSpinBox->setValue(i);
}

void SendCommandDialog::setCmdData1(int i)
{
	CmdData1SpinBox->setValue(i);
}

void SendCommandDialog::setCmdData2(int i)
{
	CmdData2SpinBox->setValue(i);
}
