#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "LockBox.h"
#include "SimplePIinput.h"
#include "common.h"

SimplePIinput::SimplePIinput(LockBox *parent, unsigned int configOffset, QString inputName, QStringList *inputRanges) :
  SimplePIgroupBox(parent, configOffset, inputName, 70, 130)
{
  int labelSize = 70;
  int widgetSize = 130;

  // Input range
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    QLabel *label = new QLabel(tr("Input range:"));
    InputRangeComboBox = new QComboBox();

    label->setMinimumSize(labelSize, 0);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    for (int i = 0; i < inputRanges->length(); i ++)
    {
      InputRangeComboBox->addItem(inputRanges->value(i));
    }
    InputRangeComboBox->setMinimumSize(widgetSize, 0);
    InputRangeComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(label);
    hLayout->addWidget(InputRangeComboBox);
    mainLayout->addLayout(hLayout);
  }

  // 1st order IIR filter
  {
    QStringList filterTypes;
    filterTypes << QString("LOW PASS");
    filterTypes << QString("HIGH PASS");
    filterTypes << QString("ALL PASS");
    filterTypes << QString("P");
    filterTypes << QString("CUSTOM");

    IIR0 = new IIRfilter1stOrder(parent, configOffset + 9, 26, false, &filterTypes, labelSize, widgetSize);
    mainLayout->addWidget(IIR0);
  }

  // Hook up on_InputParametersChanged
  QObject::connect(InputRangeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_InputParametersChanged()));

  setAnalogEditable(false);
}

double SimplePIinput::getInputRangeMin()
{
  int i = parent->config(configOffset, 129, 128);
  QString inputRangeString = InputRangeComboBox->itemText(i);
  if (inputRangeString == "+/-0.5 V")
    return -0.5;
  else if (inputRangeString == "+/-1 V")
    return -1;
  else if (inputRangeString == "+/-2 V")
    return -2;
  else if (inputRangeString == "+/-4 V")
    return -4;
  else
    printf("Unidentified input range\n");

  return 0;
}

double SimplePIinput::getInputRangeMax()
{
  int i = parent->config(configOffset, 129, 128);
  QString inputRangeString = InputRangeComboBox->itemText(i);
  if (inputRangeString == "+/-0.5 V")
    return 0.5;
  else if (inputRangeString == "+/-1 V")
    return 1;
  else if (inputRangeString == "+/-2 V")
    return 2;
  else if (inputRangeString == "+/-4 V")
    return 4;
  else
    printf("Unidentified input range\n");

  return 0;
}

void SimplePIinput::analogSignalRangeModified()
{

}

void SimplePIinput::setAnalogEditable(bool editable)
{
  setNameEditable(editable);

  InputRangeComboBox->setEnabled(editable);
}

void SimplePIinput::on_InputParametersChanged()
{
  if (SETTING_WIDGETS_FROM_CONFIG)
    return;

  parent->set_config(configOffset, 129, 128, InputRangeComboBox->currentIndex());
  parent->analogSignalRangeModified();
}

void SimplePIinput::set_widgets_from_config()
{
  SimplePIgroupBox::set_widgets_from_config();

  SETTING_WIDGETS_FROM_CONFIG = true;

  InputRangeComboBox->setCurrentIndex(parent->config(configOffset, 129, 128));
  IIR0->set_widgets_from_config();

  SETTING_WIDGETS_FROM_CONFIG = false;
}
