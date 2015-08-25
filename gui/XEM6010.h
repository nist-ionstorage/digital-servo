#ifndef XEM6010_H
#define XEM6010_H

#include <QString>

#include "okFrontPanelDLL.h"

//! Opal Kelly XEM6010 FPGA with analog input and output
class XEM6010
{
public:
    XEM6010(int deviceIndex = 0);

	static int selectDevice(QWidget *parent = 0);

	QString getID();
	void setID(const QString &ID);

protected:
    okCFrontPanel *dev;
};

#endif // XEM6010_H
