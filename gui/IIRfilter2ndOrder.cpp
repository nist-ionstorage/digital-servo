#include <QHBoxLayout>
#include <QVBoxLayout>

#include "LockBox.h"
#include "IIRfilter2ndOrder.h"
#include "common.h"

IIRfilter2ndOrder::IIRfilter2ndOrder(LockBox *parent, unsigned int configOffset, int a0shift, int updateEvery, bool alwaysOn, QStringList *filterTypes, int labelSize, int widgetSize) :
  QWidget(0),
  parent(parent),
  configOffset(configOffset),
  updateEvery(updateEvery),
  alwaysOn(alwaysOn),
  SETTING_WIDGETS_FROM_CONFIG(false),
  SETTING_WIDGETS_VISIBLE(false),
  SETTING_COEFFICIENTS_FROM_PARAMETERS(false),
  SETTING_PARAMETERS_FROM_COEFFICIENTS(false)
{
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  a0 = 0x1LL << a0shift;

  // Filter on/off and type
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    FilterOnCheckBox = new QCheckBox(tr("IIR filter 2:"));
    FilterTypeComboBox = new QComboBox();

    FilterOnCheckBox->setMinimumSize(labelSize, 0);
    FilterOnCheckBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    if (alwaysOn)
    {
      FilterOnCheckBox->setChecked(true);
      FilterOnCheckBox->setEnabled(false);
    }

    for (int i = 0; i < filterTypes->length(); i ++)
    {
      FilterTypeComboBox->addItem(filterTypes->value(i));
    }

    FilterTypeComboBox->setMinimumSize(widgetSize, 0);
    FilterTypeComboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(FilterOnCheckBox);
    hLayout->addWidget(FilterTypeComboBox);
    mainLayout->addLayout(hLayout);
  }

  // Corner frequency spin box
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    freqLabel = new QLabel(tr("Corner freq:"));
    freqSpinBox = new VariableDoubleSpinBox();

    freqLabel->setMinimumSize(labelSize, 0);
    freqLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    freqSpinBox->setSuffix(tr(" Hz"));
    freqSpinBox->setDecimals(0);
    freqSpinBox->setSingleStep(10);

    freqSpinBox->setMinimumSize(widgetSize, 0);
    freqSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(freqLabel);
    hLayout->addWidget(freqSpinBox);
    mainLayout->addLayout(hLayout);
  }

  // Q
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    qualityFactorLabel = new QLabel(tr("Q:"));
    qualityFactorSpinBox = new VariableDoubleSpinBox();

    qualityFactorLabel->setMinimumSize(labelSize, 0);
    qualityFactorLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    qualityFactorSpinBox->setDecimals(2);
    qualityFactorSpinBox->setSingleStep(1);

    hLayout->addWidget(qualityFactorLabel);
    hLayout->addWidget(qualityFactorSpinBox);
    mainLayout->addLayout(hLayout);
  }

  // Gain
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    gainLabel = new QLabel(tr("Gain:"));
    gainSpinBox = new VariableDoubleSpinBox();

    gainLabel->setMinimumSize(labelSize, 0);
    gainLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    gainSpinBox->setSuffix(tr(" dB"));
    gainSpinBox->setDecimals(1);
    gainSpinBox->setSingleStep(1);

    hLayout->addWidget(gainLabel);
    hLayout->addWidget(gainSpinBox);
    mainLayout->addLayout(hLayout);
  }

  // Gain limit
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    gainLimitLabel = new QLabel(tr("Gain limit:"));
    gainLimitSpinBox = new VariableDoubleSpinBox();

    gainLimitLabel->setMinimumSize(labelSize, 0);
    gainLimitLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    gainLimitSpinBox->setSuffix(tr(" dB"));
    gainLimitSpinBox->setDecimals(0);
    gainLimitSpinBox->setSingleStep(5);

    hLayout->addWidget(gainLimitLabel);
    hLayout->addWidget(gainLimitSpinBox);
    mainLayout->addLayout(hLayout);
  }

  // a0 spin box
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    a0Label = new QLabel(tr("a0:"));
    a0SpinBox = new QLabel("2<sup>" + locale().toString(a0shift) + "</sup>");

    a0Label->setMinimumSize(labelSize, 0);
    a0Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    a0SpinBox->setMinimumSize(widgetSize, 0);
    a0SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(a0Label);
    hLayout->addWidget(a0SpinBox);
    mainLayout->addLayout(hLayout);
  }

  // a1 spin box
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    a1Label = new QLabel(tr("a1:"));
    a1SpinBox = new VariableLongLongSpinBox();

    a1Label->setMinimumSize(labelSize, 0);
    a1Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    a1SpinBox->setRange(-17179869184, 17179869183);
    a1SpinBox->setSingleStep(1);
    a1SpinBox->setValue(0);

    a1SpinBox->setMinimumSize(widgetSize, 0);
    a1SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(a1Label);
    hLayout->addWidget(a1SpinBox);
    mainLayout->addLayout(hLayout);
  }

  // a2 spin box
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    a2Label = new QLabel(tr("a2:"));
    a2SpinBox = new VariableLongLongSpinBox();

    a2Label->setMinimumSize(labelSize, 0);
    a2Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    a2SpinBox->setRange(-17179869184, 17179869183);
    a2SpinBox->setSingleStep(1);
    a2SpinBox->setValue(0);

    a2SpinBox->setMinimumSize(widgetSize, 0);
    a2SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(a2Label);
    hLayout->addWidget(a2SpinBox);
    mainLayout->addLayout(hLayout);
  }

  // b0 spin box
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    b0Label = new QLabel(tr("b0:"));
    b0SpinBox = new VariableLongLongSpinBox();

    b0Label->setMinimumSize(labelSize, 0);
    b0Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    b0SpinBox->setRange(-17179869184, 17179869183);
    b0SpinBox->setSingleStep(1);
    b0SpinBox->setValue(0);

    b0SpinBox->setMinimumSize(widgetSize, 0);
    b0SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(b0Label);
    hLayout->addWidget(b0SpinBox);
    mainLayout->addLayout(hLayout);
  }

  // b1 spin box
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    b1Label = new QLabel(tr("b1:"));
    b1SpinBox = new VariableLongLongSpinBox();

    b1Label->setMinimumSize(labelSize, 0);
    b1Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    b1SpinBox->setRange(-17179869184, 17179869183);
    b1SpinBox->setSingleStep(1);
    b1SpinBox->setValue(0);

    b1SpinBox->setMinimumSize(widgetSize, 0);
    b1SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(b1Label);
    hLayout->addWidget(b1SpinBox);
    mainLayout->addLayout(hLayout);
  }

  // b2 spin box
  {
    QHBoxLayout *hLayout = new QHBoxLayout();
    b2Label = new QLabel(tr("b2:"));
    b2SpinBox = new VariableLongLongSpinBox();

    b2Label->setMinimumSize(labelSize, 0);
    b2Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    b2SpinBox->setRange(-17179869184, 17179869183);
    b2SpinBox->setSingleStep(1);
    b2SpinBox->setValue(0);

    b2SpinBox->setMinimumSize(widgetSize, 0);
    b2SpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    hLayout->addWidget(b2Label);
    hLayout->addWidget(b2SpinBox);
    mainLayout->addLayout(hLayout);
  }

  // Hook up on_FilterTypeChanged
  QObject::connect(FilterOnCheckBox, SIGNAL(toggled(bool)), this, SLOT(on_FilterTypeChanged()));
  QObject::connect(FilterTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_FilterTypeChanged()));

  // Hook up on_FilterParametersChanged
  QObject::connect(freqSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_FilterParametersChanged()));
  QObject::connect(qualityFactorSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_FilterParametersChanged()));
  QObject::connect(gainSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_FilterParametersChanged()));
  QObject::connect(gainLimitSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_FilterParametersChanged()));

  // Hook up on_CoefficientsChanged
  QObject::connect(a1SpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_CoefficientsChanged()));
  QObject::connect(a2SpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_CoefficientsChanged()));
  QObject::connect(b0SpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_CoefficientsChanged()));
  QObject::connect(b1SpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_CoefficientsChanged()));
  QObject::connect(b2SpinBox, SIGNAL(valueChanged(qint64)), this, SLOT(on_CoefficientsChanged()));
}

void IIRfilter2ndOrder::set_widgets_from_config()
{
  SETTING_WIDGETS_FROM_CONFIG = true;

  // On/off and type
  if (alwaysOn)
    parent->set_config(configOffset, 0, 0, true);
  else
    FilterOnCheckBox->setChecked(parent->config(configOffset, 0, 0));
  FilterTypeComboBox->setCurrentIndex(parent->config(configOffset, 3, 1));
  set_widgets_visible();

  // IIR coefficients
  a1SpinBox->setValue(parent->config(configOffset + 1, 34, 0, 1));
  a2SpinBox->setValue(parent->config(configOffset + 4, 34, 0, 1));
  b0SpinBox->setValue(parent->config(configOffset + 7, 34, 0, 1));
  b1SpinBox->setValue(parent->config(configOffset + 10, 34, 0, 1));
  b2SpinBox->setValue(parent->config(configOffset + 13, 34, 0, 1));

  // Filter parameters
  if (!(FilterTypeComboBox->currentText() == "CUSTOM"))
    set_parameters_from_coefficients();

  SETTING_WIDGETS_FROM_CONFIG = false;
}

void IIRfilter2ndOrder::on_FilterTypeChanged()
{
  if (SETTING_WIDGETS_FROM_CONFIG || SETTING_WIDGETS_VISIBLE)
    return;

  // On/off and type
  parent->set_config(configOffset, 0, 0, FilterOnCheckBox->isChecked());
  parent->set_config(configOffset, 3, 1, FilterTypeComboBox->currentIndex());
  set_widgets_visible();

  if (!(FilterTypeComboBox->currentText() == "CUSTOM"))
    set_coefficients_from_parameters();
  set_config_from_coefficients();
  if (!(FilterTypeComboBox->currentText() == "CUSTOM"))
    set_parameters_from_coefficients();
}

void IIRfilter2ndOrder::set_config_from_coefficients()
{
  parent->set_config(configOffset, 0, 0, false);

  parent->set_config(configOffset + 1, 34, 0, a1SpinBox->value());
  parent->set_config(configOffset + 4, 34, 0, a2SpinBox->value());
  parent->set_config(configOffset + 7, 34, 0, b0SpinBox->value());
  parent->set_config(configOffset + 10, 34, 0, b1SpinBox->value());
  parent->set_config(configOffset + 13, 34, 0, b2SpinBox->value());

  parent->set_config(configOffset, 0, 0, FilterOnCheckBox->isChecked());
}

void IIRfilter2ndOrder::on_FilterParametersChanged()
{
  if (SETTING_WIDGETS_FROM_CONFIG || SETTING_WIDGETS_VISIBLE || SETTING_PARAMETERS_FROM_COEFFICIENTS)
    return;

  // IIR coefficients
  set_coefficients_from_parameters();
  set_config_from_coefficients();
  set_parameters_from_coefficients();
}

void IIRfilter2ndOrder::on_CoefficientsChanged()
{
  if (SETTING_WIDGETS_FROM_CONFIG || SETTING_WIDGETS_VISIBLE || SETTING_COEFFICIENTS_FROM_PARAMETERS)
    return;

  // IIR coefficients
  set_config_from_coefficients();
}

void IIRfilter2ndOrder::set_widgets_visible()
{
  SETTING_WIDGETS_VISIBLE = true;

  FilterTypeComboBox->setEnabled(false);

  freqLabel->setVisible(false);
  freqSpinBox->setVisible(false);
  qualityFactorLabel->setVisible(false);
  qualityFactorSpinBox->setVisible(false);
  gainLabel->setVisible(false);
  gainSpinBox->setVisible(false);
  gainLimitLabel->setVisible(false);
  gainLimitSpinBox->setVisible(false);

  a0Label->setVisible(false);
  a0SpinBox->setVisible(false);
  a1Label->setVisible(false);
  a1SpinBox->setVisible(false);
  a2Label->setVisible(false);
  a2SpinBox->setVisible(false);
  b0Label->setVisible(false);
  b0SpinBox->setVisible(false);
  b1Label->setVisible(false);
  b1SpinBox->setVisible(false);
  b2Label->setVisible(false);
  b2SpinBox->setVisible(false);

  if (FilterOnCheckBox->isChecked())
  {
    FilterTypeComboBox->setEnabled(true);

    QString type = FilterTypeComboBox->currentText();
    if ((type == "CUSTOM"))
    {
      a0Label->setVisible(true);
      a0SpinBox->setVisible(true);
      a1Label->setVisible(true);
      a1SpinBox->setVisible(true);
      a2Label->setVisible(true);
      a2SpinBox->setVisible(true);
      b0Label->setVisible(true);
      b0SpinBox->setVisible(true);
      b1Label->setVisible(true);
      b1SpinBox->setVisible(true);
      b2Label->setVisible(true);
      b2SpinBox->setVisible(true);
    }
    else
    {
      if (type == "LOW PASS")
      {
        freqLabel->setVisible(true);
        freqSpinBox->setVisible(true);
        freqSpinBox->setRange(1e2, 1e6);

        qualityFactorLabel->setVisible(true);
        qualityFactorSpinBox->setVisible(true);
        qualityFactorSpinBox->setRange(0.5, 1e2);

        gainLabel->setVisible(true);
        gainSpinBox->setVisible(true);
        gainSpinBox->setRange(0, 0);
      }
      else if (type == "HIGH PASS")
      {
        freqLabel->setVisible(true);
        freqSpinBox->setVisible(true);
        freqSpinBox->setRange(1e3, 1e5);

        qualityFactorLabel->setVisible(true);
        qualityFactorSpinBox->setVisible(true);
        qualityFactorSpinBox->setRange(0.5, 1e2);

        gainLabel->setVisible(true);
        gainSpinBox->setVisible(true);
        gainSpinBox->setRange(0, 0);
      }
      else if (type == "NOTCH")
      {
        freqLabel->setVisible(true);
        freqSpinBox->setVisible(true);
		if (a0 == 32)
	        freqSpinBox->setRange(1e2, 1e6);
		else
			freqSpinBox->setRange(1e2, 1e5);

        qualityFactorLabel->setVisible(true);
        qualityFactorSpinBox->setVisible(true);
        qualityFactorSpinBox->setRange(0.5, 10);

        gainLabel->setVisible(true);
        gainSpinBox->setVisible(true);
        gainSpinBox->setRange(0, 0);
      }
      else if (type == "P")
      {
        gainLabel->setVisible(true);
        gainSpinBox->setVisible(true);
        gainSpinBox->setRange(-200, 200);
      }
      else if (type == "I/HO")
      {
        freqLabel->setVisible(true);
        freqSpinBox->setVisible(true);
        freqSpinBox->setRange(1e2, 1e5);

        qualityFactorLabel->setVisible(true);
        qualityFactorSpinBox->setVisible(true);
        qualityFactorSpinBox->setRange(1e-2, 1e2);

        gainLabel->setVisible(true);
        gainSpinBox->setVisible(true);
        gainSpinBox->setRange(0, 0);

        gainLimitLabel->setVisible(true);
        gainLimitSpinBox->setVisible(true);
        gainLimitSpinBox->setRange(20, 40);
      }
      else
      {
        printf("IIRfilter2ndOrder::set_widgets_visible: Unidentified filter type\n");
      }
    }
  }
  SETTING_WIDGETS_VISIBLE = false;
}

void IIRfilter2ndOrder::set_coefficients_from_parameters()
{
  SETTING_COEFFICIENTS_FROM_PARAMETERS = true;

  double pft = M_PI*(freqSpinBox->value())*FPGA_CLOCK_T*updateEvery;
  double pft2 = pft*pft;
  double Q = qualityFactorSpinBox->value();
  double K = pow(10.0, (gainSpinBox->value())/20.0);
  double g = pow(10.0, (gainLimitSpinBox->value())/20.0);

  QString type = FilterTypeComboBox->currentText();
  if (type == "LOW PASS")
  {
    a1SpinBox->setValue(round(a0*(2.0)*(1.0 - pft2)/(1.0 + pft/Q + pft2)));
    a2SpinBox->setValue(round(a0*(-1.0)*(1.0 - pft/Q + pft2)/(1.0 + pft/Q + pft2)));
    b0SpinBox->setValue(round(a0*(K*pft2)/(1.0 + pft/Q + pft2)));
    b1SpinBox->setValue(round(a0*(2.0*K*pft2)/(1.0 + pft/Q + pft2)));
    b2SpinBox->setValue(round(a0*(K*pft2)/(1.0 + pft/Q + pft2)));
  }
  else if (type == "HIGH PASS")
  {
    a1SpinBox->setValue(round(a0*(2.0)*(1.0 - pft2)/(1.0 + pft/Q + pft2)));
    a2SpinBox->setValue(round(a0*(-1.0)*(1.0 - pft/Q + pft2)/(1.0 + pft/Q + pft2)));
    b0SpinBox->setValue(round(a0*(K)/(1.0 + pft/Q + pft2)));
    b1SpinBox->setValue(round(a0*(-2.0*K)/(1.0 + pft/Q + pft2)));
    b2SpinBox->setValue(round(a0*(K)/(1.0 + pft/Q + pft2)));
  }
  else if (type == "NOTCH")
  {
    a1SpinBox->setValue(round(a0*(2.0)*(1.0 - pft2)/(1.0 + pft/Q + pft2)));
    a2SpinBox->setValue(round(a0*(-1.0)*(1.0 - pft/Q + pft2)/(1.0 + pft/Q + pft2)));
    b0SpinBox->setValue(round(a0*(K)*(1.0 + pft2)/(1.0 + pft/Q + pft2)));
    b1SpinBox->setValue(round(a0*(-2.0*K)*(1.0 - pft2)/(1.0 + pft/Q + pft2)));
    b2SpinBox->setValue(round(a0*(K)*(1.0 + pft2)/(1.0 + pft/Q + pft2)));
  }
  else if (type == "P")
  {
    a1SpinBox->setValue(round(a0*(0.0)));
    a2SpinBox->setValue(round(a0*(0.0)));
    b0SpinBox->setValue(round(a0*(K)));
    b1SpinBox->setValue(round(a0*(0.0)));
    b2SpinBox->setValue(round(a0*(0.0)));
  }
  else if (type == "I/HO")
  {
    a1SpinBox->setValue(round(a0*(2.0)/(1.0 + pft*g)));
    a2SpinBox->setValue(round(a0*(-1.0)*(1.0 - pft*g)/(1.0 + pft*g)));
    b0SpinBox->setValue(round(a0*(K)*(1.0 + pft/Q + pft2)/(1.0/g + pft)));
    b1SpinBox->setValue(round(a0*(-2.0*K)*(1.0 - pft2)/(1.0/g + pft)));
    b2SpinBox->setValue(round(a0*(K)*(1.0 - pft/Q + pft2)/(1.0/g + pft)));
  }
  else
  {
    printf("IIRfilter2ndOrder::set_coefficients_from_parameters: Unidentified filter type\n");
  }

  /*
  printf("a1 = %lld\n", a1SpinBox->value());
  printf("a2 = %lld\n", a2SpinBox->value());
  printf("b0 = %lld\n", b0SpinBox->value());
  printf("b1 = %lld\n", b1SpinBox->value());
  printf("b2 = %lld\n", b2SpinBox->value());
  */

  SETTING_COEFFICIENTS_FROM_PARAMETERS = false;
}

void IIRfilter2ndOrder::set_parameters_from_coefficients()
{
  SETTING_PARAMETERS_FROM_COEFFICIENTS = true;

  double a1 = double(a1SpinBox->value())/double(a0);
  double a2 = double(a2SpinBox->value())/double(a0);
  double b0 = double(b0SpinBox->value())/double(a0);
  double b1 = double(b1SpinBox->value())/double(a0);
  double b2 = double(b2SpinBox->value())/double(a0);

  double pft = 0.0;
  double Q = 0.0;
  double K = 0.0;
  double g = 0.0;

  QString type = FilterTypeComboBox->currentText();
  if (type == "LOW PASS")
  {
    pft = sqrt((1.0 - a1 - a2)/(1.0 + a1 - a2));
    Q = sqrt((1.0 - a1 - a2)*(1.0 + a1 - a2))/(2.0*(1.0 + a2));
    K = 1.0;
  }
  else if (type == "HIGH PASS")
  {
    pft = sqrt((1.0 - a1 - a2)/(1.0 + a1 - a2));
    Q = sqrt((1.0 - a1 - a2)*(1.0 + a1 - a2))/(2.0*(1.0 + a2));
    K = 1.0;
  }
  else if (type == "NOTCH")
  {
    pft = sqrt((1.0 - a1 - a2)/(1.0 + a1 - a2));
    Q = sqrt((1.0 - a1 - a2)*(1.0 + a1 - a2))/(2.0*(1.0 + a2));
    K = 1.0;
  }
  else if (type == "P")
  {
    K = b0;
  }
  else if (type == "I/HO")
  {
    pft = (sqrt(b1*b1 + 4.0*(a1 - 2.0)*(a1 - 2.0)) + b1)/(2.0*(2.0 - a1));
    Q = (2.0 - a1)/(b0 - b2);
    g = (2.0/a1 - 1.0)/pft;
    K = 1.0;
  }
  else
  {
    printf("IIRfilter2ndOrder::set_parameters_from_coefficients: Unidentified filter type\n");
  }

  freqSpinBox->setValue(pft/(M_PI*FPGA_CLOCK_T*updateEvery));
  qualityFactorSpinBox->setValue(Q);
  gainSpinBox->setValue(20.0*log10(K));
  gainLimitSpinBox->setValue(20.0*log10(g));

  /*
  printf("f0 = %f\n", pft/(M_PI*FPGA_CLOCK_T));
  printf("Q = %f\n", Q);
  printf("K = %f\n", K);
  printf("g = %f\n", g);
  */

  SETTING_PARAMETERS_FROM_COEFFICIENTS = false;
}
