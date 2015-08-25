#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class LockBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int deviceIndex = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void setDeviceName();
    bool sendCommand();
	void loadConfigFromFlash();
    void saveConfigToFlash();
	bool loadConfigFromFile();
    bool saveConfigToFile();
	bool recordData();
	bool measureTransferFunction();
	bool optimizeLTC2195();
	bool optimizeAD9783();
	void about();
	void lockboxModified();
    
private:
	void createActions();
    void createMenus();
	bool okToContinue();
        
    LockBox *lockbox;

	QMenu *deviceMenu;
    QMenu *configurationMenu;
	QMenu *toolsMenu;
	QMenu *optionsMenu;
    QMenu *helpMenu;
	QAction *setNameAction;
    QAction *sendCommandAction;
	QAction *loadFromFlashAction;
    QAction *saveToFlashAction;
    QAction *loadFromFileAction;
    QAction *saveToFileAction;
    QAction *recordDataAction;
	QAction *measureTransferFunctionAction;
	QAction *optimizeLTC2195Action;
    QAction *optimizeAD9783Action;
	QAction *editAnalogAction;
    QAction *logDataAction;
    QAction *aboutAction;

	unsigned int sendCommandAddr, sendCommandData1, sendCommandData2;
	int recordDataSampleRate, recordDataNsamples;
	double transferFunModFreqStart, transferFunModFreqStop;
	int transferFunModFreqSteps, transferFunModAmp, transferFunModOutChan;
};

#endif // MAINWINDOW_H
