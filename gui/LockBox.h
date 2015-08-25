#ifndef LOCKBOX_H
#define LOCKBOX_H

#include <QWidget>
#include <QVector>

#include "XEM6010.h"

// number of 16 bit config variables
#ifdef CONFIG_FEEDFORWARD
#define N_CONFIG 384 // FEEDFORWARD
#else
#define N_CONFIG 768 // SIMPLEPI
#endif

// regularly poll the XEM6010 for monitor data
#define N_MONITOR_CHANNELS 16
#define N_MONITOR_DATA 256

#define T_MONITOR_TIMER 200 // in ms

//! Lock box
class LockBox : public QWidget, public XEM6010
{
    Q_OBJECT

public:
    LockBox(QWidget *parent = 0, int deviceIndex = 0);

    unsigned int config(unsigned int i);
    long long int config(unsigned int i, unsigned int msb, unsigned int lsb, bool extendSignBit = 0);
    unsigned int get_config(unsigned int i);
    void set_config(unsigned int i, unsigned int value);
	void set_config(unsigned int i, unsigned int msb, unsigned int lsb, long long int value);
    
	virtual void load_config_from_flash();
	void save_config_to_flash();
	virtual bool load_config_from_file(const QString &fileName);
	bool save_config_to_file(const QString &fileName);

	bool send_command(unsigned int cmd, unsigned int arg);
	bool send_command(unsigned int cmd, unsigned int arg1, unsigned int arg2);
	unsigned int read_wire_out(unsigned int i);

	bool record_data_to_file(const QString &fileName, int sampleRate, int Nsamples);

	QList< QList<unsigned int> > getLTC2195timingArray();
	void setLTC2195encDly(unsigned int value);
	unsigned int getLTC2195frame();

	QList< QList<unsigned int> > getAD9783timingArray();
	void setAD9783smpDly(unsigned int value);
	void setAD9783setANDhld(unsigned int SET, unsigned int HLD);
	unsigned int getAD9783seek();

	virtual double getAnalogSignalRangeMin(int i);
	virtual double getAnalogSignalRangeMax(int i);
	virtual void analogSignalRangeModified();

signals:
    void modified();

protected:
	void setTimerInterval(double ms);
	virtual void timerEvent(QTimerEvent *e);

	QVector<unsigned int> configuration;

	double monitorData[N_MONITOR_CHANNELS][N_MONITOR_DATA];

	int timerInterval;
	int timerId;
	unsigned int time;

private:
	enum { MagicNumber = 0x6C53A186 };

private slots:
	virtual void setAnalogEditable(bool editable);
	virtual void setLogData(bool bLogData);
};

#endif // LOCKBOX_H