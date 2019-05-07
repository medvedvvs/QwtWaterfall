#include <qwt_plot.h>
#include <qwt_system_clock.h>
#include "qwtThreadManager.h"
#include "qwt_plot_waterfall.h"

class Plot: public QwtPlot
{
	Q_OBJECT

public:
	Plot(QwtPlot *p,  QWidget * = NULL );
	QwtPlotWaterfall* m_content;
	virtual bool eventFilter( QObject *, QEvent * );

void initAxis( int axis, const QString& title);
public Q_SLOTS:
	void exportPlot();
	void Panned(int dx, int dy);
	void    plotscDivChanged();


private:
	qwtThreadManager m_loadThread[3];
	void incrementInterval();

protected:
	virtual void timerEvent( QTimerEvent * );
	int d_timerId;
	void alignScales();

};
