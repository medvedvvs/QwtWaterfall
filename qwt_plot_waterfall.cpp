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
/*
#include <QEvent>
#include <QMetaEnum>
#include <QDebug>   
*/

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

bool QwtPlotWaterfall::addLayer(qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, qreal minval, qreal maxval,  int Xa, int Ya, QImage::Format fm, QColor fil, qreal opacity )
{
	QwtWfLayer_t *la = new QwtWfLayer_t;
	la->w = width_;
	la->h = height_;
//	la->image = QImage(width_, height_, QImage::Format_ARGB32);
	la->image = QImage(width_, height_, fm);
	if(Xa >= QwtPlot::axisCnt || Xa < 0) Xa = -1;
	if(Ya >= QwtPlot::axisCnt || Ya < 0) Ya = -1;
	la->Xaxis = Xa;
	la->Yaxis = Ya;
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
	
	if(Xa > 0)
		axis_used[Xa] = true;
	if(Ya > 0)
		axis_used[Ya] = true;
	layers.append(la);
return true;
}

bool QwtPlotWaterfall::addLayer(qint32 width_, qint32 height_, qreal minx, qreal miny, qreal maxx, qreal maxy, QwtInterval range_,  int Xa, int Ya, QImage::Format fm, QColor fil, qreal opacity )
{
	QwtWfLayer_t *la = new QwtWfLayer_t;
	la->w = width_;
	la->h = height_;
//	la->image = QImage(width_, height_, QImage::Format_ARGB32);
	la->image = QImage(width_, height_, fm);
	if(Xa >= QwtPlot::axisCnt || Xa < 0) Xa = -1;
	if(Ya >= QwtPlot::axisCnt || Ya < 0) Ya = -1;
	la->Xaxis = Xa;
	la->Yaxis = Ya;
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
	
	if(Xa > 0)
		axis_used[Xa] = true;
	if(Ya > 0)
		axis_used[Ya] = true;
	layers.append(la);
return true;
}

void QwtPlotWaterfall::deleteLayer( qint32 l ) {

rw_lock->lockForWrite();
//printf("Not implemented yet\n"); return;
	if(l >= 0 && l < layers.count())
		delete layers.takeAt(l);
rw_lock->unlock();
		
}



void QwtPlotWaterfall::attach( QwtPlot *plot_ ) {

if(plot_) {
	plott = plot_;
	plott->canvas()->installEventFilter( this );
	for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ ) {
		if(axis_used[axis])
			connect( plott->axisWidget( axis ), SIGNAL( scaleDivChanged() ), SLOT( plotscaleDivChanged() ) );
		}
	}

}

/*
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
*/
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

//if(e->type() != QEvent::Paint) qDebug() << "handling an event" << e;
return QWidget::eventFilter( object, e );
}


void QwtPlotWaterfall::plotscaleDivChanged()
{
QwtWfLayer_t *la;
qint32	w, h, x , y;
qreal 	scx, scy;
QwtScaleMap mx, my;

if(orig_set) {

	scx = (qreal)width() / (qreal)orig_w;
	scy = (qreal)height() / (qreal)orig_h;

rw_lock->lockForRead();
//for(int i=0; i < layers.count(); i++){
for ( QList<QwtWfLayer_t*>::iterator it = layers.begin();
	it != layers.end(); ++it)
	{
//	la = layers[i];
 	la = *it;

	
	if(la->Xaxis < 0) {
		mx.setPaintInterval(la->minx, la->minx + la->w);
		mx.setScaleInterval(la->minx, la->maxx);
		} else
		mx = plott->canvasMap(la->Xaxis);
		
	 if(la->Yaxis < 0 ){
	 	my.setPaintInterval(la->miny, la->miny + la->h);
		my.setScaleInterval(la->miny, la->maxy);
	 	} else
	 	my = plott->canvasMap(la->Yaxis);
	
	QRectF ff(la->minx, la->maxx,  fabs(la->maxx - la->minx), fabs((la->maxy - la->miny)));
//	printf("FF: %f %f %f %f | %f %f %f %f \n", mx.p1(), mx.p2(), mx.s1(), mx.s2(), my.p1(), my.p2(), my.s1(), my.s2());
	QRectF f = QwtScaleMap::transform (mx, my, ff);

	if(la->Xaxis < 0) {
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

	if(la->Yaxis < 0) {
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
	}

	QwtPlotCanvas *canva = reinterpret_cast<QwtPlotCanvas *>( plott->canvas());
	QRect rr = canva->rect();
	rr.adjust(canva->frameWidth(), canva->frameWidth(), -canva->frameWidth(), -canva->frameWidth());
	borderClip = canva->borderPath( rr);

//	update();
plott->canvas()->update();
}


void QwtPlotWaterfall::resizeEvent(QResizeEvent *event)
{
QwtWfLayer_t *la;
qint32	w, h, x , y;
qreal 	scx, scy;
QwtScaleMap mx, my;
//printf("Resize called\n");
	if(!orig_set) {
		orig_set = true;
		orig_w = width();
		orig_h = height();
                }
	scx = (qreal)width() / (qreal)orig_w;
	scy = (qreal)height() / (qreal)orig_h;

rw_lock->lockForRead();
//for(int i=0; i < layers.count(); i++){
//	la = layers[i];
for ( QList<QwtWfLayer_t*>::iterator it = layers.begin();
	it != layers.end(); ++it)
	{
	la = *it;
	
	if(la->Xaxis < 0) {
		mx.setPaintInterval(la->minx, la->minx + la->w);
		mx.setScaleInterval(la->minx, la->maxx);
		} else
		mx = plott->canvasMap(la->Xaxis);
		
	 if(la->Yaxis < 0 ){
	 	my.setPaintInterval(la->miny, la->miny + la->h);
		my.setScaleInterval(la->miny, la->maxy);
	 	} else
	 	my = plott->canvasMap(la->Yaxis);
	
	QRectF ff(la->minx, la->maxx,  fabs(la->maxx - la->minx), fabs((la->maxy - la->miny)));
//	printf("FF: %f %f %f %f | %f %f %f %f \n", mx.p1(), mx.p2(), mx.s1(), mx.s2(), my.p1(), my.p2(), my.s1(), my.s2());
	QRectF f = QwtScaleMap::transform (mx, my, ff);

	if(la->Xaxis < 0) {
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

	if(la->Yaxis < 0) {
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
static int cnt = 0;
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
printf("Replot called\n");
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
	if(la->w >= w && la->h >= h) {
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
	if(la->w >= w && la->h >= h) {
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
	if(la->w >= w && la->h >= h) {
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
	if(la->w >= w && la->h >= h) {
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
	if(la->w >= w && la->h >= h) {
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
	if(la->w >= w && la->h >= h) {
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
	if(la->w >= w && la->h >= h) {
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
	if(la->w >= w && la->h >= h) {
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

