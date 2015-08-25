#include <QtGui>
#include <QtWidgets>

#include "VariableLongLongSpinBox.h"

VariableLongLongSpinBox::VariableLongLongSpinBox(QWidget *parent)
    : QAbstractSpinBox(parent)
{
    m_prefix = "";
	m_suffix = "";
	
	m_minimum = 0;
    m_maximum = 99;
    m_singleStep = 1;
    m_value = m_minimum;

	connect(lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(on_lineEditTextChanged(const QString &)));
}

qint64 VariableLongLongSpinBox::value() const
{
    return m_value;
}

void VariableLongLongSpinBox::setValue(qint64 value)
{
    m_value = value;
	updateEdit();

	emit valueChanged(value);
}

QString VariableLongLongSpinBox::prefix() const
{
    return m_prefix;
}

void VariableLongLongSpinBox::setPrefix(const QString &prefix)
{
    m_prefix = prefix;
	updateEdit();
}

QString VariableLongLongSpinBox::suffix() const
{
    return m_suffix;
}

void VariableLongLongSpinBox::setSuffix(const QString &suffix)
{
    m_suffix = suffix;
	updateEdit();
}

QString VariableLongLongSpinBox::cleanText() const
{
	QString t = lineEdit()->text();

    int from = 0;
    int size = t.size();
    bool changed = false;
    if (prefix().size() && t.startsWith(prefix()))
	{
        from += prefix().size();
        size -= from;
        changed = true;
    }
    if (suffix().size() && t.endsWith(suffix()))
	{
        size -= suffix().size();
        changed = true;
    }
    if (changed)
        t = t.mid(from, size);

	return t;
}

qint64 VariableLongLongSpinBox::singleStep() const
{
    return m_singleStep;
}

void VariableLongLongSpinBox::setSingleStep(qint64 singleStep)
{
    if (singleStep >= 0)
	{
        m_singleStep = singleStep;
    }
}

qint64 VariableLongLongSpinBox::minimum() const
{
    return m_minimum;
}

void VariableLongLongSpinBox::setMinimum(qint64 minimum)
{
    m_minimum = (minimum < m_maximum ? minimum : m_maximum);

	if (value() < m_minimum)
		setValue(m_minimum);
}

qint64 VariableLongLongSpinBox::maximum() const
{
    return m_maximum;
}

void VariableLongLongSpinBox::setMaximum(qint64 maximum)
{
    m_maximum = (maximum > m_minimum ? maximum : m_minimum);

	if (value() > m_maximum)
		setValue(m_maximum);
}

void VariableLongLongSpinBox::setRange(qint64 minimum, qint64 maximum)
{
    m_minimum = minimum;
	m_maximum = (maximum > minimum ? maximum : minimum);

	if (value() < m_minimum)
		setValue(m_minimum);
	else if (value() > m_maximum)
		setValue(m_maximum);
}

QString VariableLongLongSpinBox::textFromValue(qint64 value) const
{
    QString str = locale().toString(value);
    if (qAbs(value) >= 1000 || value == INT_MIN) {
        str.remove(locale().groupSeparator());
    }

    return str;
}

qint64 VariableLongLongSpinBox::valueFromText(const QString &text) const
{
    return text.toLongLong();
}

QValidator::State VariableLongLongSpinBox::validate(QString & input, int & pos) const
{
    QString t = input;
    if (!prefix().isEmpty() && t.startsWith(prefix()))
        t.remove(0, prefix().size());
    if (!suffix().isEmpty() && t.endsWith(suffix()))
        t.chop(suffix().size());
    
	bool ok;
    qint64 v = t.toLongLong(&ok);
    if (!ok)
        return QValidator::Invalid;
    if (v < minimum() || v > maximum())
        return QValidator::Invalid;
    return QValidator::Acceptable;
}

void VariableLongLongSpinBox::fixup(QString &input) const
{
    input.remove(locale().groupSeparator());
}

void VariableLongLongSpinBox::on_lineEditTextChanged(const QString & t)
{
	QString tmp = t;
    
	int pos = lineEdit()->cursorPosition();
    QValidator::State state = validate(tmp, pos);
	if (state == QValidator::Acceptable)
	{
		setValue(valueFromText(cleanText()));
    }
}

void VariableLongLongSpinBox::updateEdit()
{
	const bool sb = lineEdit()->blockSignals(true);
	
	int cp = lineEdit()->cursorPosition();
    
	lineEdit()->setText(prefix() + textFromValue(value()) + suffix());
	
	cp = qBound(prefix().size(), cp, lineEdit()->text().size() - suffix().size());
    lineEdit()->setCursorPosition(cp);

	lineEdit()->blockSignals(sb);
}

QAbstractSpinBox::StepEnabled VariableLongLongSpinBox::stepEnabled() const
{
    if (wrapping())
        return StepUpEnabled | StepDownEnabled;

	StepEnabled se = 0;
    if (value() < maximum()) se |= StepUpEnabled;
    if (value() > minimum()) se |= StepDownEnabled;
    return se;
}

void VariableLongLongSpinBox::stepBy(int steps)
{
	qint64 oldValue = value();
	
	// Default new value
	int i = -99;
	qint64 newValue = oldValue + singleStep() * qint64(steps);

	// If a single digit is selected, increment or decrement that digit
	if (lineEdit()->hasSelectedText())
	{
		if (lineEdit()->selectedText().length() == 1)
		{
			if (lineEdit()->selectedText()[0].isDigit())
			{
				i = cleanText().length() - lineEdit()->selectionStart();
				int j = cleanText().length() - lineEdit()->selectionStart();
				if (j >= 0)
				{
					j--;
				}
				newValue = oldValue + qint64(pow(10.0, j)) * qint64(steps);
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