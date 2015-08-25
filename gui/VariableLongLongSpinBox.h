#ifndef VARIABLELONGLONGSPINBOX_H
#define VARIABLELONGLONGSPINBOX_H

#include <QAbstractSpinBox>

class VariableLongLongSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
    Q_PROPERTY(QString cleanText READ cleanText)
    Q_PROPERTY(qint64 minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(qint64 maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(qint64 singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(qint64 value READ value WRITE setValue NOTIFY valueChanged USER true)

public:
    VariableLongLongSpinBox(QWidget *parent = 0);

    qint64 value() const;

    QString prefix() const;
    void setPrefix(const QString &prefix);

    QString suffix() const;
    void setSuffix(const QString &suffix);

    QString cleanText() const;

    qint64 singleStep() const;
    void setSingleStep(qint64 val);

    qint64 minimum() const;
    void setMinimum(qint64 min);

    qint64 maximum() const;
    void setMaximum(qint64 max);

    void setRange(qint64 min, qint64 max);

	virtual void stepBy(int steps);

	virtual QValidator::State validate (QString & input, int & pos) const;
	virtual void fixup(QString &str) const;
protected:
	QString m_prefix, m_suffix;
	qint64 m_minimum, m_maximum, m_singleStep, m_value;

    virtual qint64 valueFromText(const QString &text) const;
    virtual QString textFromValue(qint64 val) const;

	void updateEdit();

	virtual StepEnabled stepEnabled() const;

public Q_SLOTS:
    void setValue(qint64 val);

Q_SIGNALS:
    void valueChanged(qint64);

private slots:
	void on_lineEditTextChanged(const QString &);
};

#endif // VARIABLELONGLONGSPINBOX_H
