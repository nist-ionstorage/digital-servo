#ifndef MONITORDATALOGGER_H
#define MONITORDATALOGGER_H

#include "LockBox.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QTime>

//! Logs monitor data
class MonitorDataLogger
{
public:
    MonitorDataLogger(QStringList *channelNames, double *p_dataIn, int N_dataIn);

	void setConfig(bool bLogData_in, unsigned int logEvery_in);
	void updateLog();

protected:
    bool bLogData;
	unsigned int logEvery, logCounter;

	QStringList *channelNames;
	double *p_dataIn;
	int N_dataIn;

	double data_mean[N_MONITOR_CHANNELS];
	double data_min[N_MONITOR_CHANNELS];
	double data_max[N_MONITOR_CHANNELS];

	int elapsed_time[2];
	bool data_good[2];

	QTime timer;

	QFile logFile;
	QTextStream logTextStream;
};

#endif // MONITORDATALOGGER_H
