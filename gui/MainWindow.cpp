#include <QtGui>
#include <QtWidgets>

#include "MainWindow.h"
#include "LockBoxSimplePI.h"

#ifdef CONFIG_FEEDFORWARD
#include "LockBoxFeedForward.h"
#endif // CONFIG_FEEDFORWARD

#include "common.h"
#include "SendCommandDialog.h"
#include "RecordDataDialog.h"
#include "MeasureTransferFunctionDialog.h"
#include "OptimizeTimingDialog.h"

#include <time.h>
#include <ionizer_utils.h>

MainWindow::MainWindow(int deviceIndex) :
sendCommandAddr(0),
sendCommandData1(0),
sendCommandData2(0),
recordDataSampleRate(4000000),
recordDataNsamples(163840),
transferFunModFreqStart(1000),
transferFunModFreqStop(1000000),
transferFunModFreqSteps(100),
transferFunModAmp(20),
transferFunModOutChan(0)
{
	#ifdef CONFIG_SIMPLEPI
	lockbox = new LockBoxSimplePI(0, deviceIndex);
	#endif
	#ifdef CONFIG_FEEDFORWARD
	lockbox = new LockBoxFeedForward(0, deviceIndex);
	#endif
	#ifdef CONFIG_SHB
	lockbox = new LockBoxSHB(0, deviceIndex);
	#endif
	setCentralWidget(lockbox);

	setWindowTitle(tr("%1[*] - %2").arg(lockbox->getID())
                                   .arg(tr("SuperLaserLand")));

	createActions();
    createMenus();
	statusBar();

	connect(lockbox, SIGNAL(modified()), this, SLOT(lockboxModified()));

    setWindowIcon(QIcon(":/images/icon.ico"));
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToContinue())
	{
        event->accept();
    }
	else
	{
        event->ignore();
    }
}

void MainWindow::createActions()
{
    setNameAction = new QAction(tr("&Set name"), this);
    setNameAction->setStatusTip(tr("Set device name"));
    connect(setNameAction, SIGNAL(triggered()), this, SLOT(setDeviceName()));

	sendCommandAction = new QAction(tr("&Send command"), this);
    sendCommandAction->setStatusTip(tr("Send command to FPGA"));
    connect(sendCommandAction, SIGNAL(triggered()), this, SLOT(sendCommand()));

	loadFromFlashAction = new QAction(tr("&Load from FLASH"), this);
    loadFromFlashAction->setStatusTip(tr("Load device configuration from onboard FLASH memory"));
    connect(loadFromFlashAction, SIGNAL(triggered()), this, SLOT(loadConfigFromFlash()));

	saveToFlashAction = new QAction(tr("&Save to FLASH"), this);
    saveToFlashAction->setStatusTip(tr("Save device configuration to onboard FLASH memory"));
    connect(saveToFlashAction, SIGNAL(triggered()), this, SLOT(saveConfigToFlash()));

	loadFromFileAction = new QAction(tr("&Load from file ..."), this);
    loadFromFileAction->setIcon(QIcon(":/images/open.png"));
    loadFromFileAction->setStatusTip(tr("Load device configuration from file"));
    connect(loadFromFileAction, SIGNAL(triggered()), this, SLOT(loadConfigFromFile()));

	saveToFileAction = new QAction(tr("&Save to file ..."), this);
    saveToFileAction->setIcon(QIcon(":/images/save.png"));
    saveToFileAction->setStatusTip(tr("Save device configuration to file"));
    connect(saveToFileAction, SIGNAL(triggered()), this, SLOT(saveConfigToFile()));

	recordDataAction = new QAction(tr("&Record data"), this);
    recordDataAction->setIcon(QIcon(":/images/save.png"));
    recordDataAction->setStatusTip(tr("Record monitor data to file"));
    connect(recordDataAction, SIGNAL(triggered()), this, SLOT(recordData()));

	measureTransferFunctionAction = new QAction(tr("&Measure transfer function"), this);
    measureTransferFunctionAction->setStatusTip(tr("Measure system transfer functions"));
    connect(measureTransferFunctionAction, SIGNAL(triggered()), this, SLOT(measureTransferFunction()));

	optimizeLTC2195Action = new QAction(tr("&Optimize LTC2195 timing"), this);
    optimizeLTC2195Action->setStatusTip(tr("Optimize LTC2195 parallel port timing"));
    connect(optimizeLTC2195Action, SIGNAL(triggered()), this, SLOT(optimizeLTC2195()));

	optimizeAD9783Action = new QAction(tr("&Optimize AD9783 timing"), this);
    optimizeAD9783Action->setStatusTip(tr("Optimize AD9783 parallel port timing"));
    connect(optimizeAD9783Action, SIGNAL(triggered()), this, SLOT(optimizeAD9783()));

	editAnalogAction = new QAction(tr("&Edit analog configuration"), this);
	editAnalogAction->setCheckable(true);
	editAnalogAction->setChecked(false);
    editAnalogAction->setStatusTip(tr("Allow changes to the analog input and output names and ranges"));
    connect(editAnalogAction, SIGNAL(toggled(bool)), lockbox, SLOT(setAnalogEditable(bool)));

	logDataAction = new QAction(tr("&Log data"), this);
	logDataAction->setCheckable(true);
	logDataAction->setChecked(false);
    logDataAction->setStatusTip(tr("Periodically log monitor data to file"));
    connect(logDataAction, SIGNAL(toggled(bool)), lockbox, SLOT(setLogData(bool)));

	aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    deviceMenu = menuBar()->addMenu(tr("&Device"));
    deviceMenu->addAction(setNameAction);
    deviceMenu->addAction(sendCommandAction);
    deviceMenu->addAction(optimizeLTC2195Action);
    deviceMenu->addAction(optimizeAD9783Action);
    
    configurationMenu = menuBar()->addMenu(tr("&Config"));
    configurationMenu->addAction(loadFromFlashAction);
    configurationMenu->addAction(saveToFlashAction);
    configurationMenu->addAction(loadFromFileAction);
    configurationMenu->addAction(saveToFileAction);
    
    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(recordDataAction);
    toolsMenu->addAction(measureTransferFunctionAction);
    
	optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(editAnalogAction);
	optionsMenu->addAction(logDataAction);
    
    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
}

bool MainWindow::okToContinue()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("SuperLaserLand"),
                        tr("The configuration has been modified.\n"
                           "Do you want to save the configuration to FLASH?"),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
			lockbox->save_config_to_flash();
            return true;
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MainWindow::setDeviceName()
{
	QString deviceName = QInputDialog::getText(this,
						tr("Set device name"),
						tr("Name:"));
	
	if (deviceName.isEmpty())
	{
        return;
	}
	
	lockbox->setID(deviceName);
	setWindowTitle(tr("%1[*] - %2").arg(deviceName)
                                   .arg(tr("SuperLaserLand")));
}

bool MainWindow::sendCommand()
{
	SendCommandDialog *dialog = new SendCommandDialog(this);
	dialog->setCmdAddr(sendCommandAddr);
	dialog->setCmdData1(sendCommandData1);
	dialog->setCmdData2(sendCommandData2);
	int ok = dialog->exec();
	if (!ok)
	{
		return false;
	}
	sendCommandAddr = dialog->getCmdAddr();
	sendCommandData1 = dialog->getCmdData1();
	sendCommandData2 = dialog->getCmdData2();
	
	lockbox->send_command(sendCommandAddr, sendCommandData1, sendCommandData2);
	statusBar()->showMessage(tr("Sent cmd = ") + sendCommandAddr + tr(", arg1 = ") + sendCommandData1 + tr(", arg2 = ") + sendCommandData2, 2000);

	printf("----- Send Command -----\n");
	printf("Command address = %0.4X\n", sendCommandAddr);
	printf("Command data in 1 = %0.4X\n", sendCommandData1);
	printf("Command data in 2 = %0.4X\n", sendCommandData2);
	printf("Command data out M25P32 = %0.4X\n", lockbox->read_wire_out(0));
	printf("Command data out AD9783 = %0.4X\n", lockbox->read_wire_out(1));
	printf("Command data out LTC2195 = %0.4X\n", lockbox->read_wire_out(2));
	
	return true;
}

void MainWindow::loadConfigFromFlash()
{
	lockbox->load_config_from_flash();
	setWindowModified(false);
	statusBar()->showMessage(tr("Loaded configuration from FLASH"), 2000);
}

void MainWindow::saveConfigToFlash()
{
	lockbox->save_config_to_flash();
	setWindowModified(false);
	statusBar()->showMessage(tr("Saved configuration to FLASH"), 2000);
}

bool MainWindow::loadConfigFromFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Load configuration"), "../config/" + lockbox->getID() + ".lbc",
                               tr("Configuration files (*.lbc)"));
    if (fileName.isEmpty())
	{
        statusBar()->showMessage(tr("Loading canceled"), 2000);
		return false;
	}

	if (!lockbox->load_config_from_file(fileName))
	{
		statusBar()->showMessage(tr("Loading canceled"), 2000);
        return false;
    }

	setWindowModified(true);
	statusBar()->showMessage(tr("Loaded configuration from file"), 2000);
    return true;
}

bool MainWindow::saveConfigToFile()
{
	QString fileName = QFileDialog::getSaveFileName(this,
                               tr("Save configuration"), "../config/" + lockbox->getID() + ".lbc",
                               tr("Configuration files (*.lbc)"));
    if (fileName.isEmpty())
	{
        statusBar()->showMessage(tr("Saving canceled"), 2000);
		return false;
	}

	if (!lockbox->save_config_to_file(fileName))
	{
		statusBar()->showMessage(tr("Saving canceled"), 2000);
        return false;
    }

	statusBar()->showMessage(tr("Saved configuration to file") + fileName, 2000);
    return true;
}

bool MainWindow::recordData()
{
	RecordDataDialog *dialog = new RecordDataDialog(this);
	dialog->setSampleRate(recordDataSampleRate);
	dialog->setNsamples(recordDataNsamples);
	int ok = dialog->exec();
	if (!ok)
	{
		return false;
	}
	recordDataSampleRate = dialog->getSampleRate();
	recordDataNsamples = dialog->getNsamples();
	
	double t0 = CurrentTime_s();
	QString sDir = "C:/data/SuperLaserLand/";
	QString fileName = sDir + QString::fromStdString(GetDateTimeString(t0)) + ".dat";
	// printf("%s\n", fileName.toLatin1().constData());
	
	lockbox->record_data_to_file(fileName, recordDataSampleRate, recordDataNsamples);
	statusBar()->showMessage(tr("Recorded monitor data to file") + fileName, 5000);
	return true;
}

bool MainWindow::measureTransferFunction()
{
	// Get the measurement configuration from the user
	MeasureTransferFunctionDialog *dialog = new MeasureTransferFunctionDialog(this);
	dialog->setModFreqStart(transferFunModFreqStart);
	dialog->setModFreqStop(transferFunModFreqStop);
	dialog->setModFreqSteps(transferFunModFreqSteps);
	dialog->setModAmp(transferFunModAmp);
	dialog->setModOutChan(transferFunModOutChan);
	dialog->setSampleRate(recordDataSampleRate);
	dialog->setNsamples(recordDataNsamples);
	int ok = dialog->exec();
	if (!ok)
	{
		return false;
	}
	transferFunModFreqStart = dialog->getModFreqStart();
	transferFunModFreqStop = dialog->getModFreqStop();
	transferFunModFreqSteps = dialog->getModFreqSteps();
	transferFunModAmp = dialog->getModAmp();
	transferFunModOutChan = dialog->getModOutChan();
	recordDataSampleRate = dialog->getSampleRate();
	recordDataNsamples = dialog->getNsamples();
	
	// Set the modulation frequency to transferFunModFreqStart and turn it on
	long long int pinc = round((0x1LL << 32)*transferFunModFreqStart/FPGA_CLOCK_HZ);
	lockbox->send_command(0x4000, (0x0000FFFFLL & pinc), ((0xFFFF0000LL & pinc) >> 16));
	for (int i = 0; i < 3; i++)
	{
		lockbox->send_command(0x4100 | i, 0x1F);
	}
	lockbox->send_command(0x4100 | transferFunModOutChan, 24 - transferFunModAmp);
	
	// Create the data directory
	double t0 = CurrentTime_s();
	QString sDir = "C:/data/SuperLaserLand/" + QString::fromStdString(GetDateTimeString(t0)) + "/";
	// QString sDir = "../data/" + QString::fromStdString(GetDateTimeString(t0)) + "/";
	QDir().mkdir(sDir);

	// Sweep over the modulation frequencies
	double modFreq;
	QString fileName;
	for (int i = 0; i < transferFunModFreqSteps; i++)
	{
		// Set the modulation frequency
		modFreq = transferFunModFreqStart*exp(i*log(transferFunModFreqStop/transferFunModFreqStart)/(transferFunModFreqSteps - 1.0));
		pinc = round((0x1LL << 32)*modFreq/FPGA_CLOCK_HZ);
		lockbox->send_command(0x4000, (0x0000FFFFLL & pinc), ((0xFFFF0000LL & pinc) >> 16));

		modFreq = FPGA_CLOCK_HZ*double(pinc)/(0x1LL << 32);
		printf("Modulation frequency = %f\n", modFreq);

		// Generate the file name
		fileName = sDir + QString::number(modFreq, 'i', 3).rightJustified(11, '0').replace(".", "_") + ".dat";
		printf("%s\n", fileName.toLatin1().constData());
	
		// Record the data
		lockbox->record_data_to_file(fileName, recordDataSampleRate, recordDataNsamples);
	}

	// Turn the modulation off
	lockbox->send_command(0x4100 | transferFunModOutChan, 0x1F);

	statusBar()->showMessage(tr("Recorded transfer function to folder") + sDir, 5000);
	return true;
}

bool MainWindow::optimizeLTC2195()
{
	QStringList headers;
	headers << tr("ENC delay") << tr("FR") << tr("ADC0") << tr("ADC1");
	
	QList< QList<unsigned int> > timingArray = lockbox->getLTC2195timingArray();

	OptimizeTimingDialog *dialog = new OptimizeTimingDialog(this, tr("Optimize LTC2195 timing"), headers, timingArray, tr("Set ENC delay:"), 75, 444, 420);
	int ok = dialog->exec();
	if (!ok)
	{
		return false;
	}
	int encDly = dialog->getTiming();

	lockbox->setLTC2195encDly(encDly);
	
	statusBar()->showMessage(tr("Set LTC2195 ENC delay to ") + QString::number(encDly), 2000);
	return true;
}

bool MainWindow::optimizeAD9783()
{
	QStringList headers;
	headers << tr("SMP_DLY") << tr("SEEK") << tr("SET") << tr("HLD");
	
	QList< QList<unsigned int> > timingArray = lockbox->getAD9783timingArray();

	OptimizeTimingDialog *dialog = new OptimizeTimingDialog(this, tr("Optimize AD9783 timing"), headers, timingArray, tr("Set SMP_DLY:"), 0, 31, 15);
	int ok = dialog->exec();
	if (!ok)
	{
		return false;
	}
	int smpDly = dialog->getTiming();

	lockbox->setAD9783smpDly(smpDly);
	
	statusBar()->showMessage(tr("Set AD9783 SMP_DLY to ") + QString::number(smpDly), 2000);
	return true;
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About SuperLaserLand"),
            tr("<h2>SuperLaserLand 1.0</h2>"
               "<p>This software is the PC interface to the NIST digital servo V1.0."
               "<p>For more information email david.leibrandt@nist.gov."));
}

void MainWindow::lockboxModified()
{
    setWindowModified(true);
}