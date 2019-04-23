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
	colorMap = new QwtLinearColorMap();

	plott = NULL;
	orig_set = false;
	resize(parent->width(), parent->height());
	rw_lock = new QReadWriteLock(QReadWriteLock::Recursive);
}

bool QwtPlotWaterfall::addLayer(qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, qreal minval, qreal maxval,  QImage::Format fm, QColor fil, qreal opacity )
{
	QwtWfLayer_t *la = new QwtWfLayer_t;
//	la->image = QImage(width_, height_, QImage::Format_ARGB32);
	la->image = QImage(width_, height_, fm);
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
	la->rect.setRect(0, 0, width(), height() );	// initial size
	la->noscaleX = false;
	la->noscaleY = false;
	la->attach(this);
	
	layers.append(la);
return true;
}

bool QwtPlotWaterfall::addLayer(qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, QwtInterval range_,  QImage::Format fm, QColor fil, qreal opacity )
{
	QwtWfLayer_t *la = new QwtWfLayer_t;
//	la->image = QImage(width_, height_, QImage::Format_ARGB32);
	la->image = QImage(width_, height_, fm);
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

void QwtPlotWaterfall::attachAxis(qint32 l,  qint32 axid, QwtPlot *p){
if(l < layers.count())
	layers[l]->attachAxis(axid, p);

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
			}
		if(Yscale != NULL) {
			connect( p->axisWidget( axid ), SIGNAL( scaleDivChanged() ), SLOT( yplotscaleDivChanged() ) );
			}
		}
	}

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
	plotscaleDivChanged();
}

void QwtWfLayer_t::yplotscaleDivChanged() {
	plotscaleDivChanged();
}


void QwtPlotWaterfall::lockForRead(){
	rw_lock->lockForRead();
	}
	
void QwtPlotWaterfall::unlockForRead(){
	rw_lock->unlock();
	}

void QwtWfLayer_t::plotscaleDivChanged()
{
qint32	w, h, x , y, wi, he;
qreal 	scx, scy;
QwtScaleMap mx, my;
QwtPlotWaterfall *p = reinterpret_cast<QwtPlotWaterfall *>(myparent);

if(p && p->is_orig_set()) {
	p->lockForRead();
	wi = p->width();
	he = p->height();
	scx = (qreal)wi / (qreal)p->get_orig_w();
	scy = (qreal)he / (qreal)p->get_orig_h();

	if(Xscale == NULL) {
		mx.setPaintInterval(minx, minx + image.width());
		mx.setScaleInterval(minx, maxx);
		} else {
		mx = plot_x->canvasMap(x_id);
		}
		
	 if(Yscale == NULL ){
	 	my.setPaintInterval(miny, miny + image.height());
		my.setScaleInterval(miny, maxy);
	 	} else {
	 	my = plot_y->canvasMap(y_id);
	 	}

	QRectF ff(minx, maxx,  fabs(maxx - minx), fabs((maxy - miny)));
//	printf("FF: %f %f %f %f | %f %f %f %f \n", mx.p1(), mx.p2(), mx.s1(), mx.s2(), my.p1(), my.p2(), my.s1(), my.s2());
	QRectF f = QwtScaleMap::transform (mx, my, ff);

	if(Xscale == NULL) {
		w = fabs(maxx - minx);
		x = minx;
		if(!noscaleX) {
			w *= scx;
			x *= scx;
			}
			
		} else {
		w = qRound(f.width());
		x = qRound(f.left());
		}

	if(Yscale == NULL) {
		if(!noscaleY) {
			h = fabs(maxy - miny) * scy;
			y = he - miny * scy -  h;
			} else {
			h = fabs(maxy - miny);
			y = he - miny -  h;
			}
		} else {
		h = qRound(f.height());
		y = qRound(f.bottom());
		}
	rect.setRect(x, y, w, h);
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

	#ifdef WF_DEBUG_EVENTS
	if(e->type() != QEvent::Paint) qDebug() << "WF event" << e;
	#endif

return QWidget::eventFilter( object, e );
}



void QwtPlotWaterfall::Update(){
	plott->canvas()->update();
}


void QwtPlotWaterfall::resizeEvent(QResizeEvent *event)
{
QwtWfLayer_t *la;
qint32	w, h, x , y;
qreal 	scx, scy;
QwtScaleMap mx, my;

	if(!orig_set) {
		orig_w = width();
		orig_h = height();
		orig_set = true;
                }
	scx = (qreal)width() / (qreal)orig_w;
	scy = (qreal)height() / (qreal)orig_h;

rw_lock->lockForRead();

for ( QList<QwtWfLayer_t*>::iterator it = layers.begin(); it != layers.end(); ++it)
	{
	la = *it;
	
	if(la->Xscale == NULL) {
		mx.setPaintInterval(la->minx, la->minx + la->image.width());
		mx.setScaleInterval(la->minx, la->maxx);
		} else
		mx = plott->canvasMap(la->get_x_id());
		
	 if(la->Yscale == NULL ){
	 	my.setPaintInterval(la->miny, la->miny + la->image.height());
		my.setScaleInterval(la->miny, la->maxy);
	 	} else
	 	my = plott->canvasMap(la->get_y_id());
	
	QRectF ff(la->minx, la->maxx,  fabs(la->maxx - la->minx), fabs((la->maxy - la->miny)));
//	printf("FF: %f %f %f %f | %f %f %f %f \n", mx.p1(), mx.p2(), mx.s1(), mx.s2(), my.p1(), my.p2(), my.s1(), my.s2());
	QRectF f = QwtScaleMap::transform (mx, my, ff);

	if(la->Xscale == NULL) {
		w = fabs(la->maxx - la->minx);
		x = la->minx;
		if(!la->noscaleX) {
			w *= scx;
			x *= scx;
			}
			
		} else {
		w = qRound(f.width());
		x = qRound(f.left());
		}

	if(la->Yscale == NULL) {
		if(!la->noscaleX) {
			h = fabs(la->maxy - la->miny) * scy;
			y = height() - la->miny * scy -  h;
			} else {
			h = fabs(la->maxy - la->miny);
			y = height() - la->miny -  h;
			}
		} else {
		h = qRound(f.height());
		y = qRound(f.bottom());
		}

	la->rect.setRect(x, y, w, h);
	}
	
	rw_lock->unlock();

	QwtPlotCanvas *canva = reinterpret_cast<QwtPlotCanvas *>( plott->canvas());
	QRect rr = canva->rect();
	rr.adjust(canva->frameWidth(), canva->frameWidth(), -canva->frameWidth(), -canva->frameWidth());
	borderClip = canva->borderPath( rr);


	update();
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
delete colorMap;

rw_lock->lockForWrite();
while (!layers.isEmpty())
	delete layers.takeFirst();
rw_lock->unlock();

delete rw_lock;

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
plott->canvas()->update();
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
plott->canvas()->update();
}

void QwtPlotWaterfall::appendT(qint32 l, double *data, int w, int h)
{
QwtWfLayer_t *la;
int	y = 0;

rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
//	if(la->w >= w && la->h >= h) {
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
plott->canvas()->update();
}

void QwtPlotWaterfall::appendT(qint32 l, QRgb *data, int w, int h)
{
QwtWfLayer_t *la;
int	y = 0;

rw_lock->lockForRead();
if(l < layers.count())  {
	la = layers[l];
//	if(la->w >= w && la->h >= h) {
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
plott->canvas()->update();
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
plott->canvas()->update();
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
plott->canvas()->update();
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
plott->canvas()->update();
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
plott->canvas()->update();
}

