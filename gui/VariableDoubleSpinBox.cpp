#include <QtGui>
#include <QtWidgets>

#include "VariableDoubleSpinBox.h"

VariableDoubleSpinBox::VariableDoubleSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{
	
}

void VariableDoubleSpinBox::stepBy(int steps)
{
    double oldValue = value();
	
	// Default new value
	int i = -99;
	double newValue = oldValue + singleStep() * steps;

	// If a single digit is selected, increment or decrement that digit
	if (lineEdit()->hasSelectedText())
	{
		if (lineEdit()->selectedText().length() == 1)
		{
			if (lineEdit()->selectedText()[0].isDigit())
			{
				i = cleanText().length() - lineEdit()->selectionStart();
				int j = cleanText().length() - lineEdit()->selectionStart() - decimals();
				if (decimals() > 0)
				{
					j--;
				}
				if (j >= 0)
				{
					j--;
				}
				newValue = oldValue + pow(10.0, j) * steps;
			}
		}
	}

	// Check for out of bounds and set the new value
	if (newValue < minimum())
	{
		return;
	}
	else if (newValue > maximum())
	{
		return;
	}
	setValue(newValue);

	// For unknown reasons, after the new value is set the selected text changes in unpredictable ways
	// Fix it
	if (i == -99)
	{
		lineEdit()->selectAll();
	}
	else
	{
		i = cleanText().length() - i;
		if (i < 0)
		{
			i = 0;
		}
		lineEdit()->setSelection(i, 1);
	}
}

QString VariableDoubleSpinBox::maximumValueText() const
{
	return m_maximumValueText;
}

void VariableDoubleSpinBox::setMaximumValueText(const QString &maximumValueText)
{
	m_maximumValueText = maximumValueText;
}

QString VariableDoubleSpinBox::textFromValue(double value) const
{
    if (value == maximum() && !maximumValueText().isEmpty())
	{
		return maximumValueText();
	}

	return QDoubleSpinBox::textFromValue(value);
}