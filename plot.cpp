#include "plot.h"
#include <qapplication.h>
#include <qwt_color_map.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_picker.h>

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>


#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_marker.h>

#include "qwtThreadManager.h"
#include "qwt_plot_waterfall.h"

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::darkBlue, Qt::darkRed )
    {
        addColorStop( 0.2, Qt::blue );
        addColorStop( 0.4, Qt::cyan );
        addColorStop( 0.6, Qt::yellow );
        addColorStop( 0.8, Qt::red );
    }
};

Plot::Plot(QwtPlot *plo, QWidget *parent ):
    QwtPlot( parent )
{
	setAutoReplot( false );
	QwtPlotCanvas *canvas = new QwtPlotCanvas();
	canvas->setBorderRadius( 10 );
	canvas->setPaintAttribute( QwtPlotCanvas::BackingStore, false );

	setCanvas( canvas );
//	resize(300, 200);
	setCanvasBackground( QColor( "MidnightBlue" ) );

//    this->canvas()->installEventFilter( this );

	QwtPlotPicker *picker = new QwtPlotPicker( canvas);
	picker->setTrackerMode( QwtPlotPicker::AlwaysOn );

	m_content = new QwtPlotWaterfall(canvas);
//	m_content->addLayer(1000, 1000, 100, 100, 150, 150, 0, 100,  xBottom, yLeft, QImage::Format_ARGB32, QColor(0x66, 0x66, 0xAF, 10), 1.0 );
	m_content->addLayer(1000, 1000, 10, 10, 150, 150, 0, 100, QImage::Format_ARGB32, QColor(0x66, 0xF6, 0xAF, 50), 1.0 );
	m_content->addLayer(1000, 1000, 100, 100, 150, 150, 0, 100, QImage::Format_ARGB32, QColor(0xAF, 0x66, 0x96, 20), 0.6 );
	m_content->addLayer(1000, 1000, 110, 110, 200, 200, 0, 100,  QImage::Format_ARGB32, QColor(0xA6, 0x66, 0xAF, 45), 1.0 );


	m_content->layers[0]->noscaleX = true;
//	m_content->layers[0]->noscaleY = true;
	for(int i=0; i < m_content->layers.count(); i++){
		m_content->setColorMap(i, new ColorMap());
		}
	m_content->attach(this);

	if(!plo) {
	m_content->layers[0]->attachAxis(yLeft, this);
	m_content->layers[1]->attachAxis(xBottom, this);
	m_content->layers[1]->attachAxis(yLeft, this);
	m_content->layers[2]->attachAxis(xBottom, this);
	m_content->layers[2]->attachAxis(yLeft, this);
	} else {
	m_content->layers[0]->attachAxis(yLeft, plo);
	m_content->layers[1]->attachAxis(xBottom, plo);
	m_content->layers[1]->attachAxis(yLeft, plo);
	m_content->layers[2]->attachAxis(xBottom, plo);
	m_content->layers[2]->attachAxis(yLeft, plo);
	}



//	Testing subplot on the canvas directly
	{
	insertLegend( new QwtLegend() );

	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->attach( this );

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setTitle( "Some Points" );
	curve->setPen( Qt::red, 4 ),
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

	QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
	QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
	curve->setSymbol( symbol );

	QPolygonF points;
	points << QPointF( 0.0, 0.0 ) << QPointF( 11.0, 59.0 )
		<< QPointF( 20.0, 45 ) << QPointF( 30, 68 )
		<< QPointF( 40, 79 ) << QPointF( 150, 171 );
	curve->setSamples( points );
	curve->attach( this );
	}


	const QwtInterval zInterval(0, 100);
	// A color bar on the right axis
	QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
	rightAxis->setColorBarEnabled( true );
	rightAxis->setColorBarWidth( 40 );
	rightAxis->setColorMap( zInterval, new ColorMap() );

	setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
	enableAxis( QwtPlot::yRight );

	plotLayout()->setAlignCanvasToScales( true );

	setAxisScale( QwtPlot::xBottom, 0.0, 200.0 );
	setAxisMaxMinor( QwtPlot::xBottom, 0 );
	enableAxis( QwtPlot::xBottom );
	setAxisScale( QwtPlot::yLeft, 0.0, 200.0 );
	setAxisMaxMinor( QwtPlot::yLeft, 0 );
	enableAxis( QwtPlot::yLeft );

	QwtPlotZoomer* zoomer = new QwtPlotZoomer(canvas );
	zoomer->setRubberBandPen( QColor( Qt::black ) );
	zoomer->setTrackerPen( QColor( Qt::black ) );
	zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
		Qt::RightButton, Qt::ControlModifier );
	zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
		Qt::RightButton );

	QwtPlotPanner *panner = new QwtPlotPanner( canvas );
	panner->setMouseButton( Qt::MidButton );
	panner->setAxisEnabled( QwtPlot::yRight, false );

	QwtPlotMagnifier *magnifier = new QwtPlotMagnifier( canvas );
	magnifier->setAxisEnabled( QwtPlot::yRight, false );


//	d_clock.start();
	d_timerId = startTimer( 10 );


	// single thread, 3 layers
	// set l to -1 to enable 3 layers in 1 thread
	m_loadThread[0].stopAndClear();
	m_loadThread[0].setWf(m_content, -1);
	m_loadThread[0].start();
/*	
	// multithreaded: works (tested)
	m_loadThread[0].stopAndClear();
	m_loadThread[0].setWf(m_content, 0);
	m_loadThread[0].start();

	m_loadThread[1].stopAndClear();
	m_loadThread[1].setWf(m_content, 1);
	m_loadThread[1].start();
	
	m_loadThread[2].stopAndClear();
	m_loadThread[2].setWf(m_content, 2);
	m_loadThread[2].start();
*/	

}

void Plot::timerEvent( QTimerEvent *event )
{   
    if ( event->timerId() == d_timerId )
    {
	m_content->Update();
	return;
	}
    
QwtPlot::timerEvent( event );
}


void Plot::exportPlot()
{
	QwtPlotRenderer renderer;
	renderer.exportTo( this, "rasterview.pdf" );
}

/*
bool Plot::eventFilter( QObject *object, QEvent *e )
{
//    return QwtPlot::eventFilter( object, e );

    if ( e->type() == QEvent::Resize )
    {
        const QSize size = static_cast<QResizeEvent *>( e )->size();
//	QwtInterval iv = axisInterval(QwtPlot::yLeft);
//	printf("IV: %f %f", iv.maxValue(), iv.minValue());
	
        if ( object == canvas() )
        {
	printf("Filter Canvas\n");
//	m_content->setFixedWidth(canvas()->width());
//	m_content->setFixedHeight(canvas()->height());
//	m_content->replot();
//	m_content->resize(canvas()->width(), canvas()->height());
//	QCoreApplication::sendEvent(m_content, e);
//            const int w = 16;
//            const int h = 50;
//            const int margin = 2;

//            const QRect cr = canvas()->contentsRect();
//            d_wheel->setGeometry(
//                cr.right() - margin - w, cr.center().y() - h / 2, w, h );
        }
    }
     else
        if ( e->type() == QEvent::ContentsRectChange ) {
        printf("QEvent::ContentsRectChange\n");
        }

//   	QwtInterval iv = axisInterval(QwtPlot::yLeft);
//	printf("IV: %f %f", iv.maxValue(), iv.minValue());

    return QwtPlot::eventFilter( object, e );
}
*/
