TEMPLATE = app
CONFIG += console
TARGET = waterfall


QT += core gui widgets

CONFIG += c++11

include(public.pri)
include ( /opt/Qt/5.12.0/gcc_64/features/qwt.prf )

INCLUDEPATH += .

HEADERS = \
	qwtThreadManager.h \
	qwt_plot_waterfall.h \
	plot.h

SOURCES = \
	qwtThreadManager.cpp \ 
	qwt_plot_waterfall.cpp \
	plot.cpp \
	main.cpp

