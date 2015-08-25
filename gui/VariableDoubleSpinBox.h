#ifndef VARIABLEDOUBLESPINBOX_H
#define VARIABLEDOUBLESPINBOX_H

#include <QSpinBox>

class QRegExpValidator;

class VariableDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

	Q_PROPERTY(QString maximumValueText READ maximumValueText WRITE setMaximumValueText)

public:
    VariableDoubleSpinBox(QWidget *parent = 0);

	virtual void stepBy(int steps);
	
	QString maximumValueText() const;
	void setMaximumValueText(const QString &maximumValueText);

protected:
	QString m_maximumValueText;

	QString textFromValue(double value) const;
};

#endif
