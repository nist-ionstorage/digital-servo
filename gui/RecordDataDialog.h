#ifndef RECORDDATADIALOG_H
#define RECORDDATADIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>

class RecordDataDialog : public QDialog
{
    Q_OBJECT

public:
    RecordDataDialog(QWidget *parent = 0);

	int getSampleRate();
	int getNsamples();

	void setSampleRate(int sampleRate);
	void setNsamples(int Nsamples);

private slots:
	void NsamplesChanged(int newValue);

private:
	QSpinBox *sampleRateSpinBox;
	QSpinBox *NsamplesSpinBox;
	QPushButton *okButton;
	QPushButton *cancelButton;
};

#endif // RECORDDATADIALOG_H
