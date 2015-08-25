#include <QtGui>

#include <windows.h>

#include <QMessageBox>
#include <QApplication>

#include "LockBox.h"
#include "common.h"

LockBox::LockBox(QWidget *parent, int deviceIndex) :
  QWidget(parent),
  XEM6010(deviceIndex),
  configuration(N_CONFIG),
  timerInterval(0),
  timerId(-1),
  time(0)
{
	// Check for firmware/hardware match
	dev->UpdateWireOuts();
	unsigned int FWversion = dev->GetWireOutValue(0x25);
	if (SW_VER != FWversion)
		QMessageBox::warning(parent, QObject::tr("SuperLaserLand"),
                             QObject::tr("Software version does not match firmware version.  Proceed at your own risk..."),
                             QMessageBox::Ok);
	
	// Initialize monitor data
    for (int i = 0; i < N_MONITOR_DATA; i++)
    {
        for(int j = 0; j < N_MONITOR_CHANNELS; j++)
      monitorData[j][i] = 0.0+0.5*j;
    }

  // Start the timer
  setTimerInterval(T_MONITOR_TIMER);
}

unsigned int LockBox::config(unsigned int i)
{
  return configuration[i];
}

long long int LockBox::config(unsigned int i, unsigned int msb, unsigned int lsb, bool extendSignBit)
{
  long long int value;

  while (lsb >= 16)
  {
    i++;
    msb -= 16;
    lsb -= 16;
  }

  if (msb < 16)
  {
    value = (static_cast<long long int>(configuration[i]) & (0xFFFFLL << lsb) & (0xFFFFLL >> (15 - msb))) >> lsb;
  }
  else if (msb < 32)
  {
    value = (
        (static_cast<long long int>(configuration[i]) & (0xFFFFLL << lsb) & 0xFFFFLL) |
        ((static_cast<long long int>(configuration[i+1]) & 0xFFFFLL & (0xFFFFLL >> (31 - msb))) << 16)
        ) >> lsb;
  }
  else if (msb < 48)
  {
    value = (
        (static_cast<long long int>(configuration[i]) & (0xFFFFLL << lsb) & 0xFFFFLL) |
        ((static_cast<long long int>(configuration[i+1]) & 0xFFFFLL) << 16) |
        ((static_cast<long long int>(configuration[i+2]) & 0xFFFFLL & (0xFFFFLL >> (47 - msb))) << 32)
        ) >> lsb;
  }
  else
  {
    value = (
        (static_cast<long long int>(configuration[i]) & (0xFFFFLL << lsb) & 0xFFFFLL) |
        ((static_cast<long long int>(configuration[i+1]) & 0xFFFFLL) << 16) |
        ((static_cast<long long int>(configuration[i+2]) & 0xFFFFLL) << 32) |
        ((static_cast<long long int>(configuration[i+3]) & 0xFFFFLL & (0xFFFFLL >> (63 - msb))) << 48)
        ) >> lsb;
  }

  if (extendSignBit)
  {
    if (value & (0x1LL << (msb - lsb)))
    {
      value = value | (0xFFFFFFFFFFFFFFFFLL << (msb - lsb + 1));
    }
  }

  return value;
}

#ifdef CONFIG_FEEDFORWARD

unsigned int LockBox::get_config(unsigned int i)
{
	unsigned int cmd = 0x0000;
	dev->SetWireInValue(0x00, cmd | i);
	dev->UpdateWireIns();
	dev->ActivateTriggerIn(0x40, 1);
	dev->UpdateWireOuts();
	configuration[i] = dev->GetWireOutValue(0x20);

	printf("Configuration[%i] = %i\n", i, configuration[i]);

	return configuration[i];
}

void LockBox::set_config(unsigned int i, unsigned int value)
{
	if (value != configuration[i])
	{
		configuration[i] = value;
		
		unsigned int cmd = 0x0100;
		dev->SetWireInValue(0x00, cmd | i);
		dev->SetWireInValue(0x01, configuration[i]);
		dev->UpdateWireIns();
		dev->ActivateTriggerIn(0x40, 1);

		emit modified();
	
		printf("Configuration[%i] = %i\n", i, configuration[i]);
	}
}

#else

unsigned int LockBox::get_config(unsigned int i)
{
  unsigned int cmd = 0x0000;
  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, i);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);
  dev->UpdateWireOuts();
  configuration[i] = dev->GetWireOutValue(0x20);

  // printf("Configuration[%i] = %i\n", i, configuration[i]);

  return configuration[i];
}

void LockBox::set_config(unsigned int i, unsigned int value)
{
  if (value != configuration[i])
  {
    configuration[i] = value;

    unsigned int cmd = 0x0100;
    dev->SetWireInValue(0x00, cmd);
    dev->SetWireInValue(0x01, i);
    dev->SetWireInValue(0x02, configuration[i]);
    dev->UpdateWireIns();
    dev->ActivateTriggerIn(0x40, 1);

    emit modified();

    // printf("Configuration[%i] = %i\n", i, configuration[i]);
  }
}

#endif

void LockBox::set_config(unsigned int i, unsigned int msb, unsigned int lsb, long long int value)
{
  while (lsb >= 16)
  {
    i++;
    msb -= 16;
    lsb -= 16;
  }

  if (msb < 16)
  {
    set_config(i,
        (static_cast<long long int>(configuration[i]) & ((0xFFFFLL >> (16 - lsb)) | (0xFFFFLL << (msb + 1)))) |
        ((value << lsb) & (0xFFFFLL << lsb) & (0xFFFFLL >> (15 - msb)))
        );
  }
  else if (msb < 32)
  {
    set_config(i,
        (static_cast<long long int>(configuration[i]) & (0xFFFFLL >> (16 - lsb))) |
        ((value << lsb) & (0xFFFFLL << lsb))
        );
    set_config(i + 1,
        (static_cast<long long int>(configuration[i + 1]) & (0xFFFFLL << (msb - 15))) |
        ((value >> (16 - lsb)) & (0xFFFFLL >> (31 - msb)))
        );
  }
  else if (msb < 48)
  {
    set_config(i,
        (static_cast<long long int>(configuration[i]) & (0xFFFFLL >> (16 - lsb))) |
        ((value << lsb) & (0xFFFFLL << lsb))
        );
    set_config(i + 1, value >> (16 - lsb));
    set_config(i + 2,
        (static_cast<long long int>(configuration[i + 2]) & (0xFFFFLL << (msb - 31))) |
        ((value >> (32 - lsb)) & (0xFFFFLL >> (47 - msb)))
        );
  }
  else
  {
    set_config(i,
        (static_cast<long long int>(configuration[i]) & (0xFFFFLL >> (16 - lsb))) |
        ((value << lsb) & (0xFFFFLL << lsb))
        );
    set_config(i + 1, value >> (16 - lsb));
    set_config(i + 2, value >> (32 - lsb));
    set_config(i + 3,
        (static_cast<long long int>(configuration[i + 3]) & (0xFFFFLL << (msb - 47))) |
        ((value >> (48 - lsb)) & (0xFFFFLL >> (63 - msb)))
        );
  }
}

void LockBox::load_config_from_flash()
{
  unsigned int cmd = 0x0200;
  dev->SetWireInValue(0x00, cmd);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);
  Sleep(10);

  for (int i = 0; i < N_CONFIG; i++)
  {
    get_config(i);
  }
}

void LockBox::save_config_to_flash()
{
  unsigned int cmd = 0x0300;
  dev->SetWireInValue(0x00, cmd);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);
}

bool LockBox::load_config_from_file(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("SuperLaserLand"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_3);

    quint32 magic;
    in >> magic;
    if (magic != MagicNumber) {
        QMessageBox::warning(this, tr("SuperLaserLand"),
                             tr("The file is not a SuperLaserLand file."));
        return false;
    }

    quint16 value;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    int i = 0;
  while (!in.atEnd()) {
        in >> value;
        set_config(i, value);
    i++;
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool LockBox::save_config_to_file(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("SuperLaserLand"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_3);

    out << quint32(MagicNumber);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int i = 0; i < N_CONFIG; i++)
  {
        out << quint16(configuration[i]);
    }
  QApplication::restoreOverrideCursor();
    return true;
}

bool LockBox::send_command(unsigned int cmd, unsigned int arg)
{
  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, arg);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);

  return true;
}

bool LockBox::send_command(unsigned int cmd, unsigned int arg1, unsigned int arg2)
{
  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, arg1);
  dev->SetWireInValue(0x02, arg2);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);

  return true;
}

unsigned int LockBox::read_wire_out(unsigned int i)
{
  dev->UpdateWireOuts();
  unsigned int result = dev->GetWireOutValue(0x20 | (0x0F & i));

  return result;
}

bool LockBox::record_data_to_file(const QString &fileName, int sampleRate, int Nsamples)
{
  // Set up the file
  QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("SuperLaserLand"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_3);

    QApplication::setOverrideCursor(Qt::WaitCursor);

  // Set the clock divider
  unsigned int cmd = 0x1002;
  unsigned int arg = 0xFFFF & ((100000000/sampleRate) >> 1); // clock divider

  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, arg);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);

  // Record data to XEM6010 DDR2 RAM
  cmd = 0x1001;
  arg = 0xFFFF & ((Nsamples >> 10) + 1); // number of data points to record

  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, arg);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);

  Sleep((1010*Nsamples)/sampleRate + 10);

  // Read data from XEM6010 DDR2 RAM and save to the file
  cmd = 0x1000;

  dev->SetWireInValue(0x00, cmd);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);

  unsigned char rawData[2*16*1024];
  float voltage;

  for (int i = 0; i < (Nsamples >> 10); i++)
  {
    dev->ReadFromPipeOut(0xA1, 2*16*1024, rawData);

    for (int j = 0; j < 2*16*1024; j++)
    {
      out << rawData[j];
    }

    /*
    for (int j = 0; j < 16*1024; j++)
    {
        voltage = 256.*double(rawData[2*j + 1]) + double(rawData[2*j + 0]);

      // take care of twos compliment negative numbers
      if (voltage > 32767)
      {
        voltage = -(65536 - voltage);
      }

      // scale to "voltage"
      // voltage = 0.01*voltage;

      out << voltage;

      // printf("voltage = %2.2x%2.2x = %f\n", rawData[2*j + 1], rawData[2*j + 0], voltage);
      }
    */
  }

  QApplication::restoreOverrideCursor();

  return true;
}

QList< QList<unsigned int> > LockBox::getLTC2195timingArray()
{
  QList< QList<unsigned int> > timingArray;

  // for (unsigned int ENC_DLY = 0; ENC_DLY < 512; ENC_DLY++)

  unsigned int i = 0;
  for (unsigned int ENC_DLY = 75; ENC_DLY < 445; ENC_DLY++)
  {
    QList<unsigned int> list;
    timingArray.append(list);

    setLTC2195encDly(ENC_DLY);


    dev->UpdateWireOuts();
    unsigned int frame = (dev->GetWireOutValue(0x22)) & 0x000F;
    unsigned int adc0 = dev->GetWireOutValue(0x23);
    unsigned int adc1 = dev->GetWireOutValue(0x24);

    timingArray[i].append(ENC_DLY);
    timingArray[i].append(frame);
    timingArray[i].append(adc0);
    timingArray[i].append(adc1);

    i++;
  }

  return timingArray;
}

void LockBox::setLTC2195encDly(unsigned int value)
{
  unsigned int cmd = 0x3200;
  unsigned int arg = value & 0x01FF;

  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, arg);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);

  // printf("ENC DLY = %i\n", value);
}

unsigned int LockBox::getLTC2195frame()
{
  dev->UpdateWireOuts();
  return (dev->GetWireOutValue(0x22)) & 0x000F;
}

QList< QList<unsigned int> > LockBox::getAD9783timingArray()
{
  QList< QList<unsigned int> > timingArray;
  for (unsigned int SMP_DLY = 0; SMP_DLY < 32; SMP_DLY++)
  {
    QList<unsigned int> list;
    timingArray.append(list);

    setAD9783smpDly(SMP_DLY);
    setAD9783setANDhld(0, 0);
    unsigned int SEEK = getAD9783seek();

    unsigned int SET = 0;
    unsigned int SEEK_P = SEEK;
    while ((SET < 16) && (SEEK_P == SEEK))
    {
      SET++;
      setAD9783setANDhld(SET, 0);
      SEEK_P = getAD9783seek();
    }

    unsigned int HLD = 0;
    SEEK_P = SEEK;
    while ((HLD < 16) && (SEEK_P == SEEK))
    {
      HLD++;
      setAD9783setANDhld(0, HLD);
      SEEK_P = getAD9783seek();
    }

    timingArray[SMP_DLY].append(SMP_DLY);
    timingArray[SMP_DLY].append(SEEK);
    timingArray[SMP_DLY].append(SET);
    timingArray[SMP_DLY].append(HLD);
  }

  return timingArray;
}

void LockBox::setAD9783smpDly(unsigned int value)
{
  unsigned int cmd = 0x2105;
  unsigned int arg = value & 0x001F;

  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, arg);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);
}

void LockBox::setAD9783setANDhld(unsigned int SET, unsigned int HLD)
{
  unsigned int cmd = 0x2104;
  unsigned int arg = ((SET & 0x000F) << 4) | (HLD & 0x000F);

  dev->SetWireInValue(0x00, cmd);
  dev->SetWireInValue(0x01, arg);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);
}

unsigned int LockBox::getAD9783seek()
{
  unsigned int cmd = 0x2006;

  dev->SetWireInValue(0x00, cmd);
  dev->UpdateWireIns();
  dev->ActivateTriggerIn(0x40, 1);
  dev->UpdateWireOuts();
  return (dev->GetWireOutValue(0x21)) & 0x0001;
}

double LockBox::getAnalogSignalRangeMin(int i)
{
	return 0;
}

double LockBox::getAnalogSignalRangeMax(int i)
{
	return 0;
}

void LockBox::analogSignalRangeModified()
{
}

void LockBox::setTimerInterval(double ms)
{
    timerInterval = qRound(ms);

    if (timerId >= 0)
    {
        killTimer(timerId);
        timerId = -1;
    }
    if (timerInterval >= 0)
        timerId = startTimer(timerInterval);
}

void LockBox::timerEvent(QTimerEvent *e)
{
  if(timerId < 0)
    return;

  time++;

  // Get new monitor data from the XEM6010
  unsigned char rawData[2*N_MONITOR_CHANNELS*N_MONITOR_DATA];
  dev->ReadFromPipeOut(0xA0, 2*N_MONITOR_CHANNELS*N_MONITOR_DATA, rawData);
  // printf("%i, %i, %i, %i, %i, %i, %i, %i\n", int(rawData[0]), int(rawData[1]), int(rawData[2]), int(rawData[3]), int(rawData[28]), int(rawData[29]), int(rawData[30]), int(rawData[31]));

  for (int i = 0; i < N_MONITOR_DATA; i++)
    {
        for(int j = 0; j < N_MONITOR_CHANNELS; j++)
    {
      monitorData[j][i] = 256.*double(rawData[2*j + 2*N_MONITOR_CHANNELS*i + 0]) + double(rawData[2*j + 2*N_MONITOR_CHANNELS*i + 1]);

      // take care of twos compliment negative numbers
      if (monitorData[j][i] > 32767)
      {
        monitorData[j][i] = -(65536 - monitorData[j][i]);
      }
    }
    }
  // printf("%f, %f, %f, %f\n", monitorData[0][0], monitorData[1][0], monitorData[2][0], monitorData[3][0]);
}

void LockBox::setAnalogEditable(bool editable)
{

}

void LockBox::setLogData(bool bLogData)
{

}