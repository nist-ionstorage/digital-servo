#include <QMessageBox>
#include <QInputDialog>

#include "XEM6010.h"
#include <stdio.h>
#include <iostream>

using namespace std;

XEM6010::XEM6010(int deviceIndex)
{
  // Initialize the DLL and load all entrypoints.
  if (FALSE == okFrontPanelDLL_LoadLib("okFrontPanel.dll")) {
    printf("ERROR: FrontPanel DLL could not be initialized.\n");
//		return -1;
    }

    // Initialize the FPGA
    dev = new okCFrontPanel;
    dev->GetDeviceCount();
    dev->OpenBySerial(dev->GetDeviceListSerial(deviceIndex));

    cout << "Opal Kelly model: " << dev->GetBoardModelString( dev->GetBoardModel() ) << endl;
}

int XEM6010::selectDevice(QWidget *parent)
{
  // Initialize the DLL and load all entrypoints.
  if (FALSE == okFrontPanelDLL_LoadLib("okFrontPanel.dll")) {
    printf("ERROR: FrontPanel DLL could not be initialized.\n");
//		return -1;
    }

    // Get a list of available devices
    okCFrontPanel dev;
  int n_modules = dev.GetDeviceCount();
  if (n_modules == 0)
  {
    QMessageBox::warning(parent, QObject::tr("SuperLaserLand"),
                             QObject::tr("There are no devices connected"),
                             QMessageBox::Ok);
    return -1;
  }
  else if (n_modules == 1)
  {
    return 0;
  }

  QStringList items;
  for (int i = 0; i < n_modules; i++)
  {
    dev.OpenBySerial(dev.GetDeviceListSerial(i));
    items << QString(dev.GetDeviceID().c_str()) + " (" + QString(dev.GetSerialNumber().c_str()) + ")";
  }

  // Ask the user to select a device
  bool ok;
  QString item = QInputDialog::getItem(parent, QObject::tr("SuperLaserLand"),
                                         QObject::tr("Select device:"), items, 0, false, &ok);
  if (!ok)
  {
    return -1;
  }

  return items.indexOf(item);
}

QString XEM6010::getID()
{
  return QString(dev->GetDeviceID().c_str());
}

void XEM6010::setID(const QString &ID)
{
  dev->SetDeviceID(ID.toStdString());
}
