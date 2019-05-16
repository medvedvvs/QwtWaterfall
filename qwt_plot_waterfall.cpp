/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * Copyright (C) 2019   Vadim Sinolits, Alex Erchenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/
#include <qwt_color_map.h>
#include <qwt_symbol.h>
#include <qwt_plot_picker.h>
#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>

#include <QReadWriteLock>


//#define WF_DEBUG_EVENTS

#ifdef WF_DEBUG_EVENTS
#include <QEvent>
#include <QMetaEnum>
#include <QDebug>   
#endif

#include "qwt_plot_waterfall.h"


QwtPlotWaterfall::QwtPlotWaterfall(QWidget* parent /*= 0*/)
	:QWidget(parent)
{
	colorMap_ = new QwtLinearColorMap();
	colorMap = colorMap_;

	plott = NULL;
	orig_set = false;
	resize(parent->width(), parent->height());
	rw_lock = new QReadWriteLock(QReadWriteLock::Recursive);
	native_xaxid = QwtPlot::xBottom;
	native_yaxid = QwtPlot::yLeft;
	current_layer = 0;
}

bool QwtPlotWaterfall::addLayer(qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, qreal minval, qreal maxval,  QImage::Format fm, QColor fil, qreal opacity )
{
	QwtWfLayer_t *la = new QwtWfLayer_t;
//	la->image = QImage(width_, height_, QImage::Format_ARGB32);
	la->image = QImage(width_, height_, fm);
	la->format = fm;
	la->Xscale = NULL;
	la->Yscale = NULL;
	la->opacity = opacity;
	la->range = QwtInterval(minval, maxval);
	la->colorMap = colorMap;
	la->minx  = minx;
	la->miny  = miny;
	la->maxx  = maxx;
	la->maxy  = maxy;
//	la->image.fill(QColor(0xA6, 0x66, 0xAF, 66));
	la->image.fill(fil);
	la->fill = fil;
	la->rect.setRect(0, 0, width(), height() );	// initial size
	la->noscaleX = false;
	la->noscaleY = false;
	la->attach(this);
	la->id = current_layer;
	current_layer++;

	
	layers.append(la);
return true;
}

bool QwtPlotWaterfall::addLayer(qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, QwtInterval range_,  QImage::Format fm, QColor fil, qreal opacity )
{
	QwtWfLayer_t *la = new QwtWfLayer_t;
//	la->image = QImage(width_, height_, QImage::Format_ARGB32);
	la->image = QImage(width_, height_, fm);
	la->format = fm;
	la->Xscale = NULL;
	la->Yscale = NULL;
	la->opacity = opacity;
	la->range = range_;
	la->colorMap = colorMap;
	la->minx  = minx;
	la->miny  = miny;
	la->maxx  = maxx;
	la->maxy  = maxy;
//	la->image.fill(QColor(0xA6, 0x66, 0xAF, 66));
	la->image.fill(fil);
	la->fill = fil;
	la->rect.setRect(0, 0, width(), height() );	// initial size
	la->noscaleX = false;
	la->noscaleY = false;
	la->attach(this);
	
	layers.append(la);
return true;
}

void QwtPlotWaterfall::deleteLayer( qint32 l ) {
rw_lock->lockForWrite();
	if(l >= 0 && l < layers.count())
		delete layers.takeAt(l);
rw_lock->unlock();
}

bool QwtPlotWaterfall::reconfigLayer(qint32 l, qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, qreal minval, qreal maxval)
{
QwtWfLayer_t *la = NULL;
rw_lock->lockForWrite();
if(l >= 0 && l < layers.count())
		la = layers[l];	
if(!la) {
	rw_lock->unlock();
	return false;
	}

fprintf(stderr, "Reconfig layer called\n");
	la->image = QImage(width_, height_, la->format);
	la->range = QwtInterval(minval, maxval);
	la->minx  = minx;
	la->miny  = miny;
	la->maxx  = maxx;
	la->maxy  = maxy;
	la->image.fill(la->fill);
	la->rect.setRect(0, 0, width(), height() );	// initial size
//	fprintf(stderr, " New image %d x %d\n", la->image.width(), la->image.height());
rw_lock->unlock();
return true;
}

bool QwtPlotWaterfall::reconfigLayer(qint32 l, qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, qreal minval, qreal maxval,  QImage::Format fm, QColor fil, qreal opacity )
{
QwtWfLayer_t *la = NULL;
rw_lock->lockForWrite();
if(l >= 0 && l < layers.count())
                la = layers.takeAt(l);  
if(!la) {
	rw_lock->unlock();
	return false;
	}

	la->image = QImage(width_, height_, fm);
	la->format = fm;
	la->opacity = opacity;
	la->range = QwtInterval(minval, maxval);
	la->minx  = minx;
	la->miny  = miny;
	la->maxx  = maxx;
	la->maxy  = maxy;
	la->image.fill(fil);
	la->fill = fil;
	la->rect.setRect(0, 0, width(), height() );	// initial size
rw_lock->unlock();
return true;
}

bool QwtPlotWaterfall::reconfigLayer(qint32 l, qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, QwtInterval range_,  QImage::Format fm, QColor fil, qreal opacity )
{
QwtWfLayer_t *la = NULL;
rw_lock->lockForWrite();
if(l >= 0 && l < layers.count())
                la = layers.takeAt(l);  
if(!la) {
	rw_lock->unlock();
	return false;
	}
	la->image = QImage(width_, height_, fm);
	la->format = fm;
	la->opacity = opacity;
	la->range = range_;
	la->colorMap = colorMap;
	la->minx  = minx;
	la->miny  = miny;
	la->maxx  = maxx;
	la->maxy  = maxy;
	la->image.fill(fil);
	la->fill = fil;
	la->rect.setRect(0, 0, width(), height() );	// initial size
rw_lock->unlock();
return true;
}




void QwtPlotWaterfall::attachAxis(qint32 l,  qint32 axid, QwtPlot *p){
if(l < layers.count())
	layers[l]->attachAxis(axid, p);

}

void QwtPlotWaterfall::reattachAxis(bool lock){
if(lock) rw_lock->lockForWrite();
for(int i = 0; i < layers.count(); i++)
	layers[i]->reattachAxis();
if(lock) rw_lock->unlock();
}


void QwtPlotWaterfall::plotscaleDivChanged(){
//if(lock) rw_lock->lockForWrite();
//printf("QwtPlotWaterfall::plotscaleDivChanged\n");
for(int i = 0; i < layers.count(); i++)
	layers[i]->plotscaleDivChanged();
//if(lock) rw_lock->unlock();
}

void QwtWfLayer_t::attach(QWidget *wd){
	myparent = wd;

}

void QwtWfLayer_t::attachAxis(qint32 axid, QwtPlot  *p){
	if(axid == QwtPlot::xBottom || axid == QwtPlot::xTop) {
	if(plot_x == NULL) {
		if(p != NULL) {
			Xscale = p->axisWidget( axid );
			x_id = axid;
			plot_x = p;
			orig_foreign_mx = p->canvasMap(axid);
			}
		if(Xscale != NULL) {
			connect( p->axisWidget( axid ), SIGNAL( scaleDivChanged() ), SLOT(xplotscaleDivChanged() ) );
			}
		}
	} else
	if(axid == QwtPlot::yLeft || axid == QwtPlot::yRight) {
	if(plot_y == NULL) {
		if(p != NULL) {
			Yscale = p->axisWidget( axid );
			y_id = axid;
			plot_y = p;
			orig_foreign_my = p->canvasMap(axid);
			}
		if(Yscale != NULL) {
			connect( p->axisWidget( axid ), SIGNAL( scaleDivChanged() ), SLOT( yplotscaleDivChanged() ) );
			}
		}
	}

}

void QwtWfLayer_t::reattachAxis(){
	if(plot_x != NULL)
		orig_foreign_mx = plot_x->canvasMap(x_id);
	if(plot_y != NULL)
		orig_foreign_my = plot_y->canvasMap(y_id);
}

//vvs: check this:
void QwtWfLayer_t::detatchAxis(bool x, bool y){
	if(x && Xscale != NULL) {
		//vvs: check this:
		disconnect( Xscale, SIGNAL( scaleDivChanged() ), this, SLOT( xplotscaleDivChanged() ) );
		Xscale = NULL;
		x_id = -1;
		}
	if(y && Yscale != NULL) {
		//vvs: check this:
		disconnect( Yscale, SIGNAL( scaleDivChanged() ), this, SLOT( yplotscaleDivChanged() ) );
		Yscale = NULL;
		y_id = -1;
		}

}

void QwtWfLayer_t::xplotscaleDivChanged() {
//	printf("--------------------- xplotscaleDivChanged called\n");
	plotscaleDivChanged();
}

void QwtWfLayer_t::yplotscaleDivChanged() {
//	printf("--------------------- yplotscaleDivChanged called\n");
	plotscaleDivChanged();
}


void QwtPlotWaterfall::lockForRead(){
	rw_lock->lockForRead();
	}
	
void QwtPlotWaterfall::unlockForRead(){
	rw_lock->unlock();
	}

void QwtPlotWaterfall::lockForWrite(){
	rw_lock->lockForWrite();
	}
	
void QwtPlotWaterfall::unlockForWrite(){
	rw_lock->unlock();
	}

void QwtWfLayer_t::plotscaleDivChanged()
{
qreal	wi, he;
qreal 	w, h, x, y, scx, scy, local_scx, local_scy, foreign_scx, foreign_scy, shift_x, shift_y;
QwtScaleMap mx, my, mxx, myy;
QwtPlotWaterfall *p = reinterpret_cast<QwtPlotWaterfall *>(myparent);

//printf("QwtWfLayer_t::plotscaleDivChanged image WH: %d %d\n", image.width(), image.height());
if(p && p->is_orig_set()) {
	p->lockForRead();

	foreign_scx = 1;
	foreign_scy = 1;
	shift_x = 0;
	shift_y = 0;

	mxx = p->plott->canvasMap(p->native_xaxid);
	myy = p->plott->canvasMap(p->native_yaxid);


//	wi = p->width();
//	he = p->height();
//	scx = wi / (qreal)p->get_orig_w();
//	scy = he / (qreal)p->get_orig_h();
//	scx = 1;
//	scy = 1;
	scx = mxx.sDist() / p->orig_xrange;
	scy = myy.sDist() / p->orig_yrange;
	
//	printf("%d --- newforx %f %f %f %f   %f %f\n", p->id, mxx.p1(), mxx.p2(), mxx.s1(), mxx.s2(), mxx.pDist(), mxx.sDist());
//	printf("%d --- newfory %f %f %f %f   %f %f\n", p->id, myy.p1(), myy.p2(), myy.s1(), myy.s2(), myy.pDist(), myy.sDist());

	if(plot_x != NULL) {
		mx = plot_x->canvasMap(x_id);
		if(p->plott != plot_x) {	 // avoid self-loop
		foreign_scx = (mx.sDist() / orig_foreign_mx.sDist());
		local_scx = (mxx.sDist() / p->orig_xrange);
		foreign_scx = local_scx / foreign_scx;
		shift_x =  foreign_scx * (orig_foreign_mx.s1() - mx.s1()) * p->orig_xrange / orig_foreign_mx.sDist();
//		shift_x =  foreign_scx * (orig_foreign_mx.s1() - mx.s1());
		}
//		else
//		scx = mxx.sDist() / p->orig_xrange;
		}

	if(plot_y != NULL) {
		my = plot_y->canvasMap(y_id);
		if(p->plott != plot_y) {	 // avoid self-loop
		foreign_scy = (my.sDist() / orig_foreign_my.sDist());
		local_scy = myy.sDist() / p->orig_yrange;
		foreign_scy = local_scy / foreign_scy;
		shift_y =  foreign_scy * (orig_foreign_my.s1() - my.s1()) * p->orig_yrange / orig_foreign_my.sDist();
//		shift_y =  foreign_scy * (orig_foreign_my.s1() - my.s1());
		}
//		else
//		scy = myy.sDist() / p->orig_yrange;
		}

	
	w = qAbs(maxx - minx) * foreign_scx;
	x = minx * foreign_scx + shift_x;

	h = qAbs(maxy - miny) * foreign_scy;
	y = miny * foreign_scy + shift_y;
/*	
	printf("\nplotscaleDivChanged\n");
	printf("%d --- orig ranges %f %f %f %f\n", p->id, p->orig_xrange, p->orig_yrange, mxx.sDist(), myy.sDist());
	printf("%d --- newforx %f %f %f %f   %f %f\n", p->id, mx.p1(), mx.p2(), mx.s1(), mx.s2(), mx.pDist(), mx.sDist());
	printf("%d --- newfory %f %f %f %f   %f %f\n", p->id, my.p1(), my.p2(), my.s1(), my.s2(), my.pDist(), my.sDist());
	if(plot_x != NULL) printf("%d --- origforx %f %f %f %f   %f %f\n", p->id, orig_foreign_mx.p1(), orig_foreign_mx.p2(), orig_foreign_mx.s1(), orig_foreign_mx.s2(), orig_foreign_mx.pDist(), orig_foreign_mx.sDist());
	if(plot_y != NULL) printf("%d --- origfory %f %f %f %f   %f %f\n", p->id, orig_foreign_my.p1(), orig_foreign_my.p2(), orig_foreign_my.s1(), orig_foreign_my.s2(), orig_foreign_my.pDist(), orig_foreign_my.sDist());
	printf("%d -------- %.3f %.3f %.3f %.3f | scales: %f %f %f %f shift: %f %f\n", p->id, x, y, w, h, scx, scy, foreign_scx, foreign_scy, shift_x, shift_y);
*/

	if(!noscaleX) {
	w /= scx;
	x /= scx;
	}

	if(!noscaleY) {
	h /= scy;
	y /= scy;
	}


	
//	rect.setRect(x, y, w, h);
	QRectF ff(x, y, w, h);
//	mx = p->plott->canvasMap(p->native_xaxid);
//	my = p->plott->canvasMap(p->native_yaxid);
	QRectF f = QwtScaleMap::transform (mxx, myy, ff);

//	printf("----------------\n");
//	printf("		%d plotscaleDivChanged %.4f %.4f    MINMAX: %.4f %.4f %.4f %.4f FSC: %.4f %.4f\n", id, scx, scy, minx, miny, maxx, maxy, foreign_scx, foreign_scy);
//	printf("		WxH: %.4f %.4f XY: %.4f %.4f  Rect: %.4f %.4f   %.4f x %.4f\n", w, h, x, y, f.left(), f.top(), f.width(), f.height());

//	la->rect.setRect(x, y, w, h);
	rect.setRect(f.left(), f.top(), f.width(), f.height());
	p->unlockForRead();
	}


//	QwtPlotCanvas *canva = reinterpret_cast<QwtPlotCanvas *>( plott->canvas());
//	QRect rr = canva->rect();
//	rr.adjust(canva->frameWidth(), canva->frameWidth(), -canva->frameWidth(), -canva->frameWidth());
//	borderClip = canva->borderPath( rr);

//	update();
p->Update();
}



void QwtPlotWaterfall::detatchAxis(qint32 l,  bool X,  bool Y){
if(l < layers.count())
	layers[l]->detatchAxis(X, Y);
}


void QwtPlotWaterfall::attach( QwtPlot *plot_ ) {

if(plot_) {
	plott = plot_;
	plott->canvas()->installEventFilter( this );
	connect( plott->axisWidget(native_xaxid), SIGNAL( scaleDivChanged() ), SLOT(plotscaleDivChanged() ) );
	connect( plott->axisWidget(native_yaxid), SIGNAL( scaleDivChanged() ), SLOT(plotscaleDivChanged() ) );
	}

}

#ifdef WF_DEBUG_EVENTS
/// Gives human-readable event type information.
QDebug operator<<(QDebug str, const QEvent * ev) {
   static int eventEnumIndex = QEvent::staticMetaObject
         .indexOfEnumerator("Type");
   str << "QEvent";
   if (ev) {
      QString name = QEvent::staticMetaObject
            .enumerator(eventEnumIndex).valueToKey(ev->type());
      if (!name.isEmpty()) str << name; else str << ev->type();
   } else {
      str << (void*)ev;
   }
   return str.maybeSpace();
}
#endif


bool QwtPlotWaterfall::eventFilter( QObject *object, QEvent *e )
{

	if ( e->type() == QEvent::Resize )
	{
		if ( object == plott->canvas() )
		{
		// emit resizeEvent() via resize()
		resize(plott->canvas()->width(), plott->canvas()->height());
		}
		

		
		
	}

	if ( e->type() == QEvent::LayoutRequest )
	{
		static int cnt = 0;
		if ( object == plott->canvas() )
		{
		// emit resizeEvent() via resize()
		resize(plott->canvas()->width(), plott->canvas()->height());
//		printf("------------------ GOT LayoutRequest %d\n", cnt);
		cnt++;
		}
		

		
		
	}	

//		if( object == this)
//		printf(" Event THIS %p\n", object);

	#ifdef WF_DEBUG_EVENTS
	if(e->type() != QEvent::Paint) qDebug() << "WF event" << e << object;
	#endif

return QWidget::eventFilter( object, e );
}

void QwtPlotWaterfall::reconfig()
{
//	fprintf(stderr, "resize from %d %d to %d %d\n", width(), height(), plott->canvas()->width(), plott->canvas()->height());
	resize(plott->canvas()->width(), plott->canvas()->height());
	QwtScaleMap mx, my, mxx, myy;

	reattachAxis(false);
	mx = plott->canvasMap(native_xaxid);
	my = plott->canvasMap(native_yaxid);
	orig_w = mx.pDist();
	orig_h = my.pDist();
	orig_xrange = mx.sDist();
	orig_yrange = my.sDist();
}


void QwtPlotWaterfall::Update(){
//	plott->canvas()->update();
	update();
}


void QwtPlotWaterfall::resizeEvent(QResizeEvent *event)
{
QwtWfLayer_t *la;
qreal	scx, scy;
qreal 	w, h, x, y, shift_x, shift_y, local_scx, local_scy, foreign_scx, foreign_scy; // , foreign_scx = 1, foreign_scy = 1;
QwtScaleMap mx, my, mxx, myy;

//printf("Resize event\n");
rw_lock->lockForRead();

	if(!orig_set) {
		reattachAxis(false);
		mx = plott->canvasMap(native_xaxid);
		my = plott->canvasMap(native_yaxid);
//		printf("OrigX --- %f %f %f %f   %f %f\n", mx.p1(), mx.p2(), mx.s1(), mx.s2(), mx.pDist(), mx.sDist());
//		printf("OrigY --- %f %f %f %f   %f %f\n", my.p1(), my.p2(), my.s1(), my.s2(), my.pDist(), my.sDist());
		orig_w = mx.pDist();
		orig_h = my.pDist();
		orig_xrange = mx.sDist();
		orig_yrange = my.sDist();
		orig_set = true;
		}

	mxx = plott->canvasMap(native_xaxid);
	myy = plott->canvasMap(native_yaxid);

//	scx = mxx.pDist() / (qreal)orig_w;
//	scy = myy.pDist() / (qreal)orig_h;
	scx = mxx.sDist() / orig_xrange;
	scy = myy.sDist() / orig_yrange;
//	printf("Scale to %f %f\n", scx, scy);
//	scx = 1;
//	scy = 1;
//printf("%f %f     %f %f %d %d\n", scx, scy, wi, he, orig_w, orig_h);

for ( QList<QwtWfLayer_t*>::iterator it = layers.begin(); it != layers.end(); ++it)
	{
	la = *it;

	// calculate foreign scaling and offset (physical (scale) valyes, not screen)
	foreign_scx = 1;
	foreign_scy = 1;
	shift_x = 0;
	shift_y = 0;
	
	if(la->plot_x != NULL) {
		mx = la->plot_x->canvasMap(la->x_id);
		if(plott != la->plot_x) {	 // avoid self-loop
		foreign_scx =  (mx.sDist() / la->orig_foreign_mx.sDist());
		local_scx = (mxx.sDist() / orig_xrange);
		foreign_scx = local_scx / foreign_scx;
//		shift_x =  foreign_scx * (la->orig_foreign_mx.s1() - mx.s1());
		shift_x =  foreign_scx * (la->orig_foreign_mx.s1() - mx.s1()) * orig_xrange / la->orig_foreign_mx.sDist();
		}
		}

	if(la->plot_y != NULL) {
		my = la->plot_y->canvasMap(la->y_id);
		if(plott != la->plot_y) {	 // avoid self-loop
		foreign_scy = (my.sDist() / la->orig_foreign_my.sDist());
		local_scy = (myy.sDist() / orig_yrange);
		foreign_scy = local_scy / foreign_scy;
//		shift_y =  foreign_scy * (la->orig_foreign_my.s1() - my.s1());
		shift_y =  foreign_scy * (la->orig_foreign_my.s1() - my.s1()) * orig_yrange / la->orig_foreign_my.sDist();
		}
		}


	w = qAbs(la->maxx - la->minx) * foreign_scx;
	h = qAbs(la->maxy - la->miny) * foreign_scy;
	
	x = la->minx * foreign_scx + shift_x;
	y = la->miny * foreign_scy + shift_y;

	if(!la->noscaleX) { // scale here to keep original size and position
	w /= scx;
	x /= scx;
	}

	if(!la->noscaleY) { // scale here to keep original size and position
	h /= scy;
	y /= scy;
	}

	
//	rect.setRect(x, y, w, h);
	QRectF ff(x, y, w, h);
//	mx = plott->canvasMap(native_xaxid);
//	my = plott->canvasMap(native_yaxid);
	QRectF f = QwtScaleMap::transform (mxx, myy, ff);

//	printf("		%d plotscaleDivChanged %f %f    MINMAX: %f %f %f %f FSC: %f %f\n", id, scx, scy, la->minx, la->miny, la->maxx, la->maxy, foreign_scx, foreign_scy);
//	printf("		%d Resize              %.4f %.4f    MINMAX: %.4f %.4f %.4f %.4f FSC: %.4f %.4f\n", la->id, scx, scy, la->minx, la->miny, la->maxx, la->maxy, foreign_scx, foreign_scy);
//	printf("		WxH: %.4f %.4f XY: %.4f %.4f  Rect: %.4f %.4f   %.4f x %.4f\n", w, h, x, y, f.left(), f.top(), f.width(), f.height());

//	la->rect.setRect(x, y, w, h);	

	la->rect.setRect(f.left(), f.top(), f.width(), f.height());
	}
	
	rw_lock->unlock();

	QwtPlotCanvas *canva = reinterpret_cast<QwtPlotCanvas *>( plott->canvas());
	QRect rr = canva->rect();
	rr.adjust(canva->frameWidth(), canva->frameWidth(), -canva->frameWidth(), -canva->frameWidth());
	borderClip = canva->borderPath( rr);


//	update();
	Update();
//	event->ignore();
	event->accept();
}


void QwtPlotWaterfall::paintEvent(QPaintEvent *event)
{
QwtWfLayer_t *la;

//static int cnt = 0;
//	QPainter painter(this);
//	printf("Paint %d\n", cnt); cnt++;
	painter.begin(this);
	painter.setClipPath(borderClip, Qt::IntersectClip );

//	for(int i=0; i < layers.count(); i++){
//		la = layers[i];

rw_lock->lockForRead();
	for ( QList<QwtWfLayer_t*>::iterator it = layers.begin();
	it != layers.end(); ++it)
	{
	la = *it;
//		la->rect.setRect(0, 200, 50, 50);
		painter.setOpacity(la->opacity);
		painter.drawImage(la->rect, la->image);
	}
rw_lock->unlock();
	painter.end();
//	event->ignore();
	event->accept();
}

void QwtPlotWaterfall::replot(){
//printf("Replot called\n");
//	update();
	repaint();
}


QwtPlotWaterfall::~QwtPlotWaterfall()
{
rw_lock->lockForWrite();
while (!layers.isEmpty())
	delete layers.takeFirst();
rw_lock->unlock();
delete rw_lock;

delete colorMap_;
}

void QwtPlotWaterfall::setColorMap( int l, QwtColorMap *colorMap )
{
rw_lock->lockForRead();
if(l < layers.count()) {
	if ( layers[l]->colorMap != colorMap )
		layers[l]->colorMap = colorMap;
	}
rw_lock->unlock();
}


void QwtPlotWaterfall::appendB(qint32 l, double *data, int w, int h)
{
QwtWfLayer_t *la;
int	y = h;

rw_lock->lockForRead();
if(l < layers.count()) {
	la = layers[l];
	if(la->image.width() >= w && la->image.height() >= h) {
	uchar *data_ = la->image.bits();
	memmove(data_, data_ + la->image.bytesPerLine() * h, la->image.sizeInBytes() - la->image.bytesPerLine() * h);
	while(y >= 1) {
	QRgb *line = reinterpret_cast<QRgb *>( la->image.scanLine( la->image.height() - y) );
	for ( int x = 0; x < w; x++ )
		*line++ = la->colorMap->rgb( la->range, data[x] );
	y--;
	}
	}
	}
rw_lock->unlock();

//	update();
//plott->canvas()->update();
}

void QwtPlotWaterfall::appendB(qint32 l, QRgb *data, int w, int h)
{
QwtWfLayer_t *la;
int	y = h;

rw_lock->lockForRead();
if(l < layers.count()) {
	la = layers[l];
//	if(la->w >= w && la->h >= h) {
	if(la->image.width() >= w && la->image.height() >= h) {
	uchar *data_ = la->image.bits();
	memmove(data_, data_ + la->image.bytesPerLine() * h, la->image.sizeInBytes() - la->image.bytesPerLine() * h);
	while(y >= 1) {
	QRgb *line = reinterpret_cast<QRgb *>( la->image.scanLine( la->image.height() - y) );
	memcpy(line, data, w * sizeof(QRgb));
//	for ( int x = 0; x < w; x++ )
//		*line++ = data[x];
	y--;
	}
	}
	}

rw_lock->unlock();
//	update();
//plott->canvas()->update();
}

void QwtPlotWaterfall::appendT(qint32 l, double *data, int w, int h)
{
QwtWfLayer_t *la;
int	y = 0;
//fprintf(stderr, "\n----------> QwtPlotWaterfall::appendT called %d %d\n", l, layers.count());
rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
//	if(la->w >= w && la->h >= h) {
//	fprintf(stderr, "QwtPlotWaterfall::appendT: %d %d vs %d %d\n", la->image.width(), la->image.height(), w, h);
	if(la->image.width() >= w && la->image.height() >= h) {
	uchar *data_ = la->image.bits();
	memmove(data_ + la->image.bytesPerLine() * h, data_, la->image.sizeInBytes() - la->image.bytesPerLine() * h);
	while(y < h) {
	QRgb *line = reinterpret_cast<QRgb *>( la->image.scanLine( y ) );
	for ( int x = 0; x < w; x++ )
		*line++ = la->colorMap->rgb( la->range, data[x] );
	y++;
	}
	}
	}
rw_lock->unlock();
//	update();
//plott->canvas()->update();
}

void QwtPlotWaterfall::appendT(qint32 l, QRgb *data, int w, int h)
{
QwtWfLayer_t *la;
int	y = 0;
//fprintf(stderr, "\n----------> QwtPlotWaterfall::appendT called\n");
rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
//	if(la->w >= w && la->h >= h) {
//	fprintf(stderr, "QwtPlotWaterfall::appendT: %d %d vs %d %d\n", la->image.width(), la->image.height(), w, h);
	if(la->image.width() >= w && la->image.height() >= h) {
	uchar *data_ = la->image.bits();
	memmove(data_ + la->image.bytesPerLine() * h, data_, la->image.sizeInBytes() - la->image.bytesPerLine() * h);
	while(y < h) {
	QRgb *line = reinterpret_cast<QRgb *>( la->image.scanLine( y ) );
	memcpy(line, data, w * sizeof(QRgb));
//	for ( int x = 0; x < w; x++ )
//		*line++ = la->colorMap->rgb( la->range, data[x] );
	y++;
	}
	}
	}
rw_lock->unlock();
//	update();
//plott->canvas()->update();
}


void QwtPlotWaterfall::appendL(qint32 l, double *data, int w, int h)
{
QwtWfLayer_t *la;
qint32	offset = 0;
qint32	bpp;

rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
	bpp = la->image.depth() / 8;
//	if(la->w >= w && la->h >= h) {
	if(la->image.width() >= w && la->image.height() >= h) {
	for(int i=0; i < h; i++) {
		uchar *line = la->image.scanLine(i);
		memmove(line + w * bpp, line, la->image.bytesPerLine() - w*bpp);
		QRgb *lina = reinterpret_cast<QRgb *>(line);
		for(int x = offset; x <= offset + w; x++ )
			*lina++ = la->colorMap->rgb( la->range, data[x] );
		offset += w;
	}
	}
	}
rw_lock->unlock();
//update();
//plott->canvas()->update();
}

void QwtPlotWaterfall::appendL(qint32 l, QRgb *data, int w, int h)
{
QwtWfLayer_t *la;
qint32	offset = 0;
qint32	bpp;

rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
	bpp = la->image.depth() / 8;
//	if(la->w >= w && la->h >= h) {
	if(la->image.width() >= w && la->image.height() >= h) {
	for(int i=0; i < h; i++) {
		uchar *line = la->image.scanLine(i);
		memmove(line + w * bpp, line, la->image.bytesPerLine() - w*bpp);
		QRgb *lina = reinterpret_cast<QRgb *>(line);
		for(int x = offset; x <= offset + w; x++ )
			*lina++ = data[x];
		offset += w;
	}
	}
	}
rw_lock->unlock();
//update();
//plott->canvas()->update();
}

void QwtPlotWaterfall::appendR(qint32 l, double *data, int w, int h)
{
QwtWfLayer_t *la;
qint32	offset = 0;
qint32	bpp;

rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
	bpp = la->image.depth() / 8;
//	if(la->w >= w && la->h >= h) {
	if(la->image.width() >= w && la->image.height() >= h) {
	for(int i=0; i < h; i++) {
		uchar *line = la->image.scanLine(i);
		memmove(line, line + w*bpp, la->image.bytesPerLine() - w*bpp);
		QRgb *lina = reinterpret_cast<QRgb *>(line + la->image.bytesPerLine() - w*bpp);
		for(int x = offset; x <= offset + w; x++ )
			*lina++ = la->colorMap->rgb( la->range, data[x] );
		offset += w;
	}
	}
	}
rw_lock->unlock();
// update();
//plott->canvas()->update();
}

void QwtPlotWaterfall::appendR(qint32 l, QRgb *data, int w, int h)
{
QwtWfLayer_t *la;
qint32	offset = 0;
qint32	bpp;

rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
	bpp = la->image.depth() / 8;
//	if(la->w >= w && la->h >= h) {
	if(la->image.width() >= w && la->image.height() >= h) {
	for(int i=0; i < h; i++) {
		uchar *line = la->image.scanLine(i);
		memmove(line, line + w*bpp, la->image.bytesPerLine() - w*bpp);
		QRgb *lina = reinterpret_cast<QRgb *>(line + la->image.bytesPerLine() - w*bpp);
		for(int x = offset; x <= offset + w; x++ )
			*lina++ = data[x];
		offset += w;
	}
	}
	}
rw_lock->unlock();
// update();
//plott->canvas()->update();
}

