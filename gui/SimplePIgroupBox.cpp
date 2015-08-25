#include <QHBoxLayout>
#include <QLabel>

#include "LockBox.h"
#include "SimplePIgroupBox.h"
#include "common.h"

SimplePIgroupBox::SimplePIgroupBox(LockBox *parent, unsigned int configOffset, QString GroupBoxName, int labelSize, int widgetSize) :
	QWidget(0),
	parent(parent),
	GroupBoxName(GroupBoxName),
	configOffset(configOffset),
	SETTING_WIDGETS_FROM_CONFIG(false)
{
	mainGroupBox = new QGroupBox(GroupBoxName);
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->addWidget(mainGroupBox);
	}

	mainLayout = new QVBoxLayout();
	mainGroupBox->setLayout(mainLayout);

	// Input name
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		NameLabel = new QLabel(tr("Name:"));
		NameLineEdit = new QLineEdit();

		NameLabel->setMinimumSize(labelSize, 0);
		NameLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		NameLineEdit->setMaxLength(16);
		NameLineEdit->setMinimumSize(widgetSize, 0);
		NameLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		hLayout->addWidget(NameLabel);
		hLayout->addWidget(NameLineEdit);
		mainLayout->addLayout(hLayout);
	}

	// Hook up on_NameChanged
	QObject::connect(NameLineEdit, SIGNAL(returnPressed()), this, SLOT(on_NameChanged()));
	
	setNameEditable(false);
}

void SimplePIgroupBox::setNameEditable(bool editable)
{
	NameLabel->setVisible(editable);
	NameLineEdit->setVisible(editable);
}

void SimplePIgroupBox::on_NameChanged()
{
	QString name = NameLineEdit->text();
	mainGroupBox->setTitle(GroupBoxName + " - " + name);

	name = name.leftJustified(16, ' ');
	QByteArray nameAscii = name.toLatin1();
	for (int i = 0; i < 8; i++)
	{
		parent->set_config(configOffset + i, (nameAscii[2*i+1] << 8) | nameAscii[2*i]);
	}
}

void SimplePIgroupBox::set_widgets_from_config()
{
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	// Name
	QByteArray nameAscii;
	nameAscii.resize(16);
	for (int i = 0; i < 8; i++)
	{
		nameAscii[2*i] = parent->config(configOffset + i) & 0x00FF;
		nameAscii[2*i+1] = (parent->config(configOffset + i) & 0xFF00) >> 8;
	}
	QString name = QString::fromLatin1(nameAscii, 16);
	NameLineEdit->setText(name);
	mainGroupBox->setTitle(GroupBoxName + " - " + name);
	
	SETTING_WIDGETS_FROM_CONFIG = false;
}