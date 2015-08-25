#ifndef SIMPLEPIGROUPBOX_H
#define SIMPLEPIGROUPBOX_H

#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

class LockBox;

class SimplePIgroupBox : public QWidget
{
    Q_OBJECT

public:
    SimplePIgroupBox(LockBox *parent, unsigned int configOffset, QString GroupBoxName, int labelSize, int widgetSize);

	void set_widgets_from_config();
	void setNameEditable(bool editable);
	
protected:
	LockBox *parent;
	QString GroupBoxName;
	unsigned int configOffset;
	bool SETTING_WIDGETS_FROM_CONFIG;

	QGroupBox *mainGroupBox;
	QVBoxLayout *mainLayout;
	QLabel *NameLabel;
	QLineEdit *NameLineEdit;

private slots:
	void on_NameChanged();
};

#endif // SIMPLEPIGROUPBOX_H