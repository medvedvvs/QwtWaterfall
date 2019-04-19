#include <qwt_plot.h>
#include "qwtThreadManager.h"
#include "qwt_plot_waterfall.h"

class Plot: public QwtPlot
{
	Q_OBJECT

public:
	Plot( QWidget * = NULL );
	QwtPlotWaterfall* m_content;
//	virtual bool eventFilter( QObject *, QEvent * );

public Q_SLOTS:
	void exportPlot();

private:
	qwtThreadManager m_loadThread[3];
};
