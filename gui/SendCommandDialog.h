#ifndef SENDCOMMANDDIALOG_H
#define SENDCOMMANDDIALOG_H

#include <QDialog>
#include "HexSpinBox.h"
#include <QPushButton>

class SendCommandDialog : public QDialog
{
    Q_OBJECT

public:
    SendCommandDialog(QWidget *parent = 0);

	int getCmdAddr();
	int getCmdData1();
	int getCmdData2();

	void setCmdAddr(int i);
	void setCmdData1(int i);
	void setCmdData2(int i);
	
private:
	HexSpinBox *CmdAddrSpinBox;
	HexSpinBox *CmdData1SpinBox;
	HexSpinBox *CmdData2SpinBox;
	QPushButton *okButton;
	QPushButton *cancelButton;
};

#endif // SENDCOMMANDDIALOG_H
