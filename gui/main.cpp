#include <QApplication>

#include "XEM6010.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	int deviceIndex = XEM6010::selectDevice();
    if (deviceIndex == -1)
	{
		return -1;
	}
	
	MainWindow mw(deviceIndex);
    mw.show();

    return app.exec();
}
