#ifndef OPTIMIZETIMINGDIALOG_H
#define OPTIMIZETIMINGDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QSpinBox>
#include <QPushButton>

class OptimizeTimingDialog : public QDialog
{
    Q_OBJECT

public:
    OptimizeTimingDialog(QWidget *parent, QString windowTitle, QStringList headerLabels, QList< QList<unsigned int> > timingArray, QString timingLabel, int timingMin, int timingMax, int timingValue);

	int getTiming();

//	void setHorizontalHeaderLabels(const QStringList & labels);
//	void setItem(int row, int column, const QString & text);

private slots:

private:
	QTableWidget *timingTableWidget;
	QSpinBox *timingSpinBox;
	QPushButton *okButton;
	QPushButton *cancelButton;
};

#endif // OPTIMIZETIMINGDIALOG_H
