#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPicture>
#include <QPainter>
#include <QPixmap>

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>

#include "ScopePlot.h"
#include "LockBox.h"
#include "common.h"

ScopePlot::ScopePlot(LockBox *parent, unsigned int configOffset, QStringList *channelNames, double *p_dataIn, int N_dataIn) :
	QWidget(0),
	parent(parent),
	configOffset(configOffset),
	SETTING_WIDGETS_FROM_CONFIG(false),
	curves(N_PLOT_CHANNELS),
	channelSelectComboBoxes(N_PLOT_CHANNELS),
	channelScaleSpinBoxes(N_PLOT_CHANNELS),
	channelOffsetSpinBoxes(N_PLOT_CHANNELS),
	p_dataIn(p_dataIn),
	N_dataIn(N_dataIn)
{
	plot = new QwtPlot();

	plot->setMinimumSize(300, 200);
	
	QPalette pal;
	pal.setColor(QPalette::Window, Qt::white);
	plot->setPalette(pal);
	
    // We don't need the cache here
	// plot->canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, false);
    // plot->canvas()->setPaintAttribute(QwtPlotCanvas::Opaque, false);

	// Axis 
    plot->setAxisTitle(QwtPlot::xBottom, "Time [s]");
    plot->setAxisScale(QwtPlot::xBottom, 0, 100);
    plot->setAxisScale(QwtPlot::yLeft, -5.0, 5.0);

	alignScales();

	// Initialize data
    for (int i = 0; i < N_PLOT_DATA; i++)
    {
        data_x[i] = (20./(N_PLOT_DATA-1.)) * i;     // time axis
		
		for(int j = 0; j < N_PLOT_CHANNELS; j++)
			data_y[j][i] = 0.0+0.0*j;
	}
	
	// Initialize plot curves
	for (int j = 0; j < N_PLOT_CHANNELS; j++)
	{
		curves[j] = new QwtPlotCurve("Trace " + QString::number(j+1));
		switch (j)
		{
			case 0:
				curves[j]->setPen(QPen(Qt::red));
				break;
			case 1:
				curves[j]->setPen(QPen(Qt::blue));
				break;
			case 2:
				curves[j]->setPen(QPen(Qt::green));
				break;
			case 3:
				curves[j]->setPen(QPen(Qt::magenta));
				break;
			case 4:
				curves[j]->setPen(QPen(Qt::cyan));
				break;
			default:
				curves[j]->setPen(QPen(Qt::black));
				curves[j]->setStyle(QwtPlotCurve::Dots);
		}
		curves[j]->attach(plot);

		// Attach (don't copy) data.  All curves use the same x array.
		curves[j]->setRawSamples(data_x, &(data_y[j][0]), N_PLOT_DATA);
		
		curves[j]->hide();
	}

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(plot);

	// Make the mode combobox
	QHBoxLayout *hLayout = new QHBoxLayout();
	mainLayout->addLayout(hLayout);

	hLayout->addWidget(new QLabel(tr("Mode:")));
	
	modeComboBox = new QComboBox();
	modeComboBox->addItem(tr("Off"));
	modeComboBox->addItem(tr("Rolling"));
	modeComboBox->addItem(tr("Triggered"));
	QObject::connect(modeComboBox, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(on_modeComboBox_currentIndexChanged(int)));

	hLayout->addWidget(modeComboBox);
	hLayout->addStretch();
	
	// Trigger channel select combo box
	triggerChannelSelectLabel = new QLabel(tr("Chan:"));
	hLayout->addWidget(triggerChannelSelectLabel);

	triggerChannelSelectComboBox = new QComboBox();
	triggerChannelSelectComboBox->addItem(tr("Off"));
	for (int i = 0; i < channelNames->length(); i ++)
	{
		triggerChannelSelectComboBox->addItem(channelNames->value(i));
	}
	hLayout->addWidget(triggerChannelSelectComboBox);

	// Trigger level spin box
	triggerLevelLabel = new QLabel(tr("Level:"));
	hLayout->addWidget(triggerLevelLabel);
	
	triggerLevelSpinBox = new VariableDoubleSpinBox();
	triggerLevelSpinBox->setSuffix(tr(" V"));
	triggerLevelSpinBox->setRange(-100, 100);
	triggerLevelSpinBox->setDecimals(2);
	triggerLevelSpinBox->setSingleStep(0.1);
	triggerLevelSpinBox->setValue(0);
	hLayout->addWidget(triggerLevelSpinBox);

	on_modeComboBox_currentIndexChanged(modeComboBox->currentIndex());
	
	// Make the trace selection comboboxes
	for (int j = 0; j < N_PLOT_CHANNELS; j++)
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		mainLayout->addLayout(hLayout);

		// Pictorial sample of scope trace
		QPicture picture;
        QPainter painter;
        painter.begin(&picture);           // paint in picture
		painter.setPen(curves[j]->pen());
		painter.drawLine(0, 0, 20, 0);
        painter.end();                     // painting done

		QLabel *label = new QLabel();
		label->setPicture(picture);
		hLayout->addWidget(label);
		
		// Channel select combo box
		channelSelectComboBoxes[j] = new QComboBox();
		channelSelectComboBoxes[j]->addItem(tr("Off"));
		for (int i = 0; i < channelNames->length(); i ++)
		{
			channelSelectComboBoxes[j]->addItem(channelNames->value(i));
		}
		QObject::connect(channelSelectComboBoxes[j], SIGNAL(currentIndexChanged(int)), this, SLOT(on_channelSelectComboBox_currentIndexChanged(int)));
		channelSelectComboBoxes[j]->setCurrentIndex(j + 1);
		
		hLayout->addWidget(channelSelectComboBoxes[j]);

		// Scale spin box
		hLayout->addWidget(new QLabel(tr("Scale:")));
		channelScaleSpinBoxes[j] = new VariableDoubleSpinBox();
		channelScaleSpinBoxes[j]->setSuffix(tr(" V/div"));
		channelScaleSpinBoxes[j]->setRange(0.001, 100);
		channelScaleSpinBoxes[j]->setDecimals(3);
		channelScaleSpinBoxes[j]->setSingleStep(0.1);
		channelScaleSpinBoxes[j]->setValue(1);
		hLayout->addWidget(channelScaleSpinBoxes[j]);

		// Offset spin box
		hLayout->addWidget(new QLabel(tr("Offset:")));
		channelOffsetSpinBoxes[j] = new VariableDoubleSpinBox();
		channelOffsetSpinBoxes[j]->setSuffix(tr(" V"));
		channelOffsetSpinBoxes[j]->setRange(-100, 100);
		channelOffsetSpinBoxes[j]->setDecimals(3);
		channelOffsetSpinBoxes[j]->setSingleStep(0.1);
		channelOffsetSpinBoxes[j]->setValue(0);
		hLayout->addWidget(channelOffsetSpinBoxes[j]);

		// Connect on_displayParametersChanged
		QObject::connect(channelSelectComboBoxes[j], SIGNAL(currentIndexChanged(int)), this, SLOT(on_displayParametersChanged()));
		QObject::connect(channelScaleSpinBoxes[j], SIGNAL(valueChanged(double)), this, SLOT(on_displayParametersChanged()));
		QObject::connect(channelOffsetSpinBoxes[j], SIGNAL(valueChanged(double)), this, SLOT(on_displayParametersChanged()));

		hLayout->addStretch();
	}
}

void ScopePlot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    // plot->canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    // plot->canvas()->setLineWidth(1);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)plot->axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)plot->axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}

void ScopePlot::on_modeComboBox_currentIndexChanged(int index)
{
	double timeScale;

	// Show or hide the trigger options
	if (index == 2)
	{
		triggerChannelSelectLabel->show();
		triggerChannelSelectComboBox->show();
		triggerLevelLabel->show();
		triggerLevelSpinBox->show();

		timeScale = 0.05;
	}
	else
	{
		triggerChannelSelectLabel->hide();
		triggerChannelSelectComboBox->hide();
		triggerLevelLabel->hide();
		triggerLevelSpinBox->hide();

		timeScale = 0.1*T_MONITOR_TIMER;
	}

	// Set the time axis
	for (int i = 0; i < N_PLOT_DATA; i++)
	{
		data_x[i] = (timeScale/(N_PLOT_DATA-1.)) * i;
	}
	
	plot->setAxisScale(QwtPlot::xBottom, 0, timeScale);
}

void ScopePlot::on_channelSelectComboBox_currentIndexChanged(int index)
{
	// Not pretty, but this figures out which combobox changed
	QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
	for (int j = 0; j < N_PLOT_CHANNELS; j++)
	{
		if (comboBox == channelSelectComboBoxes[j])
		{
			if (index == 0)
			{
				curves[j]->hide();
			}
			else
			{
//				for(int i = 0; i < N_PLOT_DATA; i++)
//				{
//					data_y[j][i] = 0.0+0.0*j;
//				}
				
				curves[j]->show();
			}
			plot->replot();
		}
	}
}

void ScopePlot::on_displayParametersChanged()
{
	if (SETTING_WIDGETS_FROM_CONFIG)
		return;

	for (int j = 0; j < N_PLOT_CHANNELS; j++)
	{
		parent->set_config(configOffset + 64 + 5*j, 15, 0, channelSelectComboBoxes[j]->currentIndex());
		parent->set_config(configOffset + 65 + 5*j, 31, 0, doubleToConfig(channelScaleSpinBoxes[j]->value()));
		parent->set_config(configOffset + 67 + 5*j, 31, 0, doubleToConfig(channelOffsetSpinBoxes[j]->value()));
	}
}

void ScopePlot::set_widgets_from_config()
{
	SETTING_WIDGETS_FROM_CONFIG = true;
	
	for (int j = 0; j < N_PLOT_CHANNELS; j++)
	{
		channelSelectComboBoxes[j]->setCurrentIndex(parent->config(configOffset + 64 + 5*j, 15, 0));
		channelScaleSpinBoxes[j]->setValue(configToDouble(parent->config(configOffset + 65 + 5*j, 31, 0, 1)));
		channelOffsetSpinBoxes[j]->setValue(configToDouble(parent->config(configOffset + 67 + 5*j, 31, 0, 1)));
	}

	SETTING_WIDGETS_FROM_CONFIG = false;
}

void ScopePlot::updatePlot()
{
	if (modeComboBox->currentIndex() == 1) // "Rolling" mode
	{
		for (int j = 0; j < N_PLOT_CHANNELS; j++)
		{
			int index = channelSelectComboBoxes[j]->currentIndex();
			if (index > 0)
			{
				// Shift data backwards ... this is very inefficient
				for (int i = 0; i < N_PLOT_DATA - 1; i++)
				{
					data_y[j][i] = data_y[j][i+1];
				}
		
				data_y[j][N_PLOT_DATA - 1] = ( *(p_dataIn + N_dataIn*(index - 1)) - channelOffsetSpinBoxes[j]->value() ) / channelScaleSpinBoxes[j]->value();
			}
		}
		plot->replot();
	}
	else if (modeComboBox->currentIndex() == 2) // "Triggered" mode
	{
		// First see if there are any trigger events
		int index = triggerChannelSelectComboBox->currentIndex();
		double triggerLevel = triggerLevelSpinBox->value();
		
		int iTrigger = -1;
		int iTriggerPosition = (N_PLOT_DATA >> 1);
		
		if (index > 0)
		{
			for (int i = iTriggerPosition; i < N_dataIn - (N_PLOT_DATA - iTriggerPosition) - 1; i++)
			{
				if (iTrigger < 0) // only find the first trigger event
				{
					double y1 = *(p_dataIn + N_dataIn*(index - 1) + i);
					double y2 = *(p_dataIn + N_dataIn*(index - 1) + i + 1);
					if ((y1 < triggerLevel) && (triggerLevel < y2))
						iTrigger = i;
				}
			}
		}

		int iStart;
		if (iTrigger < 0)
			iStart = 3;
		else
			iStart = iTrigger - iTriggerPosition;
		
		// Then plot the data, starting at iStart
		for (int j = 0; j < N_PLOT_CHANNELS; j++)
		{
			index = channelSelectComboBoxes[j]->currentIndex();
			if (index > 0)
			{
				for (int i = 0; i < N_PLOT_DATA; i++)
				{
					data_y[j][i] = ( *(p_dataIn + N_dataIn*(index - 1) + iStart + i) - channelOffsetSpinBoxes[j]->value() ) / channelScaleSpinBoxes[j]->value();
				}
			}
		}
		plot->replot();
	}
}