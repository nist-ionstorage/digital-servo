#include "MonitorDataLogger.h"

#include <stdio.h>
#include <iostream>

#include <time.h>
#include <ionizer_utils.h>

MonitorDataLogger::MonitorDataLogger(QStringList *channelNames, double *p_dataIn, int N_dataIn) :
	bLogData(false),
	logEvery(0),
	logCounter(0),
	channelNames(channelNames),
	p_dataIn(p_dataIn),
	N_dataIn(N_dataIn)
{
	elapsed_time[0] = 10000;
	elapsed_time[0] = 20000;

	data_good[0] = false;
	data_good[1] = false;
}

void MonitorDataLogger::setConfig(bool bLogData_in, unsigned int logEvery_in)
{
	if (bLogData_in && !bLogData)
	{
		// generate the file name
		double t0 = CurrentTime_s();
		QString sDir = "C:/data/SuperLaserLand/";
		QString fileName = sDir + QString::fromStdString(GetDateTimeString(t0)) + ".csv";
		
		// Set up the file
		logFile.setFileName(fileName);
		if (logFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			printf("Starting log file %s\n", fileName.toLatin1().constData());

			logTextStream.setDevice(&logFile);
			logTextStream << "TIME [s], ";
			for (int i = 0; i < 7; i ++)
			{
				logTextStream << channelNames->value(i);
				if (i < 6)
					logTextStream << ", ";
			}
			logTextStream << "\n";
			logTextStream.flush();
			
			bLogData = true;
			logEvery = logEvery_in;

			logCounter = 0;

			timer.restart();
		}
		else
		{
			printf("Cannot open log file %s\n", fileName.toLatin1().constData());
		}
	}
	else
	{
		bLogData = false;
		logFile.close();
	}
}

void MonitorDataLogger::updateLog()
{
	if (bLogData)
	{
		elapsed_time[0] = elapsed_time[1];
		elapsed_time[1] = timer.elapsed();
		
		if (data_good[0])
		{
			logCounter++;
			if (logCounter == 1)
			{
				for (int i = 0; i < 7; i++)
				{
					data_mean[i] = (*(p_dataIn + N_dataIn*i))/logEvery;
					data_min[i] = *(p_dataIn + N_dataIn*i + 1);
					data_max[i] = *(p_dataIn + N_dataIn*i + 2);
				}
			}
			else
			{
				for (int i = 0; i < 7; i++)
				{
					data_mean[i] += (*(p_dataIn + N_dataIn*i))/logEvery;
					data_min[i] = std::min(data_min[i], *(p_dataIn + N_dataIn*i + 1));
					data_max[i] = std::max(data_max[i], *(p_dataIn + N_dataIn*i + 2));
				}
			}
			
			if (logCounter >= logEvery)
			{
				logCounter = 0;

				logTextStream << elapsed_time[1]/1000.0 << ", ";
				for (int i = 0; i < 7; i ++)
				{
					logTextStream << data_mean[i] << ", " << data_min[i] << ", " << data_max[i];
					if (i < 6)
						logTextStream << ", ";
				}
				logTextStream << "\n";
				logTextStream.flush();
			}
		}

		// if the time interval between this entry and the previous one is abnormal, the entry two from now is bad data
		data_good[0] = data_good[1];
		int dt = elapsed_time[1] - elapsed_time[0];
		if ((150 < dt) && (dt < 250))
			data_good[1] = true;
		else
			data_good[1] = false;
	}
}
