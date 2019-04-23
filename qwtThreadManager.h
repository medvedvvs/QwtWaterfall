#ifndef QWT_WATERFALL_THREAD_H
#define QWT_WATERFALL_THREAD_H

#include <QtCore>
#include "qwt_plot_waterfall.h"

class qwtThreadManager : public QThread
{
	Q_OBJECT
public:
	qwtThreadManager(QObject* object = 0);
	~qwtThreadManager();

public:

	void quit();
	void stopAndClear();
	void setWf(QwtPlotWaterfall *wf_, int l) {wf = wf_;  layer = l;};
	
protected:
	void run();


private:
	bool m_quit;
	QwtPlotWaterfall *wf;
	int layer;
};

#endif // QWT_WATERFALL_THREAD_H
