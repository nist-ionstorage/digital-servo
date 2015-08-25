TEMPLATE = app
TARGET = SuperLaserLand
include(local.pro)

SIMPLEPI {
message("target = SuperLaserLand_SIMPLEPI")
DEFINES += CONFIG_SIMPLEPI
}

QT     += core
QT     += widgets

srcRS232 = thirdparty/qextserialport

win32 {
	DEFINES += WIN32

	INCLUDEPATH += \
		$$srcRS232 \
		thirdparty/qwt-6.1.2/src \
		thirdparty/qwt-6.1.2/lib \
		
	HEADERS += \
		$$srcRS232/qextserialport.h \
		$$srcRS232/win_qextserialport.h \
		$$srcRS232/qextserialbase.h \
		$$srcRS232/qextserialenumerator.h

		SOURCES += \
		$$srcRS232/qextserialport.cpp \
		$$srcRS232/win_qextserialport.cpp \
		$$srcRS232/qextserialbase.cpp \
		$$srcRS232/qextserialenumerator.cpp

   debug {
   CONFIG += console
   LIBS += qwtd.lib
   }

   release {
   LIBS += qwt.lib
   }
}

unix {
   DEFINES += _TTY_POSIX_
   DEFINES += __linux__
   INCLUDEPATH += /usr/include/qwt
   INCLUDEPATH += /usr/local/include/qextserialport

   LIBS += -lqextserialport -lqwt
}

HEADERS += \
	SuperLaserLand.pro \
	local.pro \
	common.h \
	MainWindow.h \
	ionizer_utils.h \
	HexSpinBox.h \
	VariableDoubleSpinBox.h \
	VariableLongLongSpinBox.h \
	SendCommandDialog.h \
	RecordDataDialog.h \
	MeasureTransferFunctionDialog.h \
	OptimizeTimingDialog.h \
	XEM6010.h \
	LockBox.h \
	IIRfilter1stOrder.h \
	IIRfilter2ndOrder.h \
	LockBoxSimplePI.h \
	SimplePIgroupBox.h \
	SimplePIinput.h \
	SimplePIlockin.h \
	SimplePIphasedetector.h \
	SimplePIoutput.h \
	PhaseDetector.h \
	ScopePlot.h \
	okFrontPanelDLL.h \
	RS232device.h \
	NP_adjuster.h \
	NPlocker.h \
	MonitorDataLogger.h
SOURCES += \
	main.cpp\
	common.cpp \
	MainWindow.cpp \
	ionizer_utils.cpp \
	HexSpinBox.cpp \
	VariableDoubleSpinBox.cpp \
	VariableLongLongSpinBox.cpp \
	SendCommandDialog.cpp \
	RecordDataDialog.cpp \
	MeasureTransferFunctionDialog.cpp \
	OptimizeTimingDialog.cpp \
	XEM6010.cpp \
	LockBox.cpp \
	IIRfilter1stOrder.cpp \
	IIRfilter2ndOrder.cpp \
	LockBoxSimplePI.cpp \
	SimplePIgroupBox.cpp \
	SimplePIinput.cpp \
	SimplePIlockin.cpp \
	SimplePIphasedetector.cpp \
	SimplePIoutput.cpp \
	PhaseDetector.cpp \
	ScopePlot.cpp \
	okFrontPanelDLL.cpp \
	RS232device.cpp \
	NP_adjuster.cpp \
	NPlocker.cpp \
	MonitorDataLogger.cpp

RESOURCES = SuperLaserLand.qrc

DEFINES += \
    QWT_DLL
