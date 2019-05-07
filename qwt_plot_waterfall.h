/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * Copyright (C) 2019   Vadim Sinolits, Alex Erchenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/
 
#ifndef QWT_WATERFALL_H
#define QWT_WATERFALL_H

#include <QtCore>
#include <QtWidgets>

#include <qwt_color_map.h>
#include <qwt_plot.h>

class QwtColorMap;

/*!
  \brief Structure for layer definitions

  Layers, that are QImage in nature, are used to render some raster data.
  
  \param image Image widget itself
  \param w Width of the layer
  \param h Height of the layer
  \param minx Minimum x value for the layer
  \param maxx Maximum x value for the layer
  \param miny Minimum y value for the layer
  \param maxy Maximum y value for the layer
  \param Xscale Attached Qwt X scale
  \param Yscale Attached Qwt Y scale
  \param noscaleX Switch on/off the scaling of the layer on X
  \param noscaleY Switch on/off the scaling of the layer on Y
  \param opacity Layer (painter) opacity (0..1)
  \param range For internal use
  \param colorMap Color map used for the layer data
  \param rect Rect for image drawing, for internal use
  \param x_id X axis id from QwtPlot::Axis
  \param y_id Y axis id from QwtPlot::Axis
  \param plot_x QwtPlot widget reference (for X axis to listen to)
  \param plot_y QwtPlot widget reference (for Y axis to listen to)
  \sa QwtPlotWaterfall
 */
class QwtWfLayer_t: public QObject  {
	Q_OBJECT
public:
	QwtWfLayer_t () {
		colorMap = NULL;
		Xscale = NULL;
		Yscale = NULL;
		plot_x = NULL;
		plot_y = NULL;
		x_id = -1;
		y_id = -1;
		noscaleX = false;
		noscaleY = false;
	};

	void	attachAxis(qint32 axid, QwtPlot *p);
	void	reattachAxis();
	void	detatchAxis(bool x, bool y);
	QImage		image;
	qreal		minx, miny, maxx, maxy;
	QwtScaleWidget	*Xscale, *Yscale;
	bool		noscaleX, noscaleY;
	qreal		opacity;
	QwtInterval	range;
	QwtColorMap	*colorMap;
	QRectF		rect;
	void		attach(QWidget *wd);
	
//	qint32		get_x_id() {return x_id;}
//	qint32		get_y_id() {return y_id;}

	qint32		x_id, y_id;	

	QwtPlot		*plot_x, *plot_y;
	QwtScaleMap	orig_foreign_mx, orig_foreign_my;
	qreal	corrector_x, corrector_y;
	void	plotscaleDivChanged();
	qint32		id;
	
protected Q_SLOTS:
	void	xplotscaleDivChanged();
	void	yplotscaleDivChanged();

protected:
	QWidget		*myparent;	// *QwtPlotWaterfall

	
};
// QwtWfLayer_t;

/*!
  \brief Qwt waterfall (datafall) widget

  \note It is possible to use variables inside QwtWfLayer_t - noscaleX, noscaleY - to set static sizes of the separate layer.
  Functions append*() are used to append some data to a layer. Colormap is used to map values inside the 2D region to color value.

*/

class QwtPlotWaterfall : public QWidget
{
	Q_OBJECT
public:
	QwtPlotWaterfall(QWidget* parent = 0);
	~QwtPlotWaterfall();

public:
	int id;
	int current_layer;
/*!
  \brief Set the color map for the layer

  Color map is used to map values inside the 2D region to color value.
  
  \param l Layer index. Must be in range [0..layers.count()-1]
  \param m QwtColorMap

 */
	void setColorMap(qint32 l, QwtColorMap *m);
/*!
  \brief Adds a new layer
  
  New layer is an image (2D) with colored data points
  Color map is used to map values inside the 2D region to color value.
  
  \note It is normal to use opacity level. Basic styling of underlying QwtPlot::canvas() is supported.
  
  \param width Widget width (as Qt widget)
  \param height Widget height (as Qt widget)
  \param minx Minimum x value for the layer
  \param maxx Maximum x value for the layer
  \param miny Minimum y value for the layer
  \param maxy Maximum y value for the layer
  \param minval Minimum (expected) value of data
  \param maxval Maximum (expected) value of data
  \param fm Of type QImage::Format. Supported QImage::Format_ARGB32 and QImage::Format_RGB32.
  \param fil Fill color for the layer (QColor).
  \param opacity Layer (painter) opacity [0 .. 1.0]
 */
	bool addLayer(qint32 width, qint32 height, qreal minx, qreal miny, qreal maxx, qreal maxy, qreal minval, qreal maxval, QImage::Format fm, QColor fil, qreal opacity );

/*!
  \brief Adds a new layer
  
  \param range Is of type QwtInterval

  \sa addLayer
 */
	bool addLayer(qint32 width, qint32 height, qreal minx, qreal miny, qreal maxx, qreal maxy, QwtInterval range, QImage::Format fm, QColor fil, qreal opacity );



/*!
   \brief Delete a layer
   
   \param l Layer index
*/
	void deleteLayer(qint32 l);

/*!
   \brief Detatch layer from an axis
   
   \param l Layer index
   \param X If true, stops X attachement
   \param Y If true, stops Y attachement
*/
	void detatchAxis(qint32 l,  bool X,  bool Y);

/*!
  \brief Data color map
  
  It is avalaible as a public member.
  Color map is used to map values inside the 2D region to color value.
  
  \sa setColorMap
 */
	QwtColorMap *colorMap;

/*!
  \brief (Q)List of layers
  
  It is avalaible as a public member.

  Default - false.

 */
	QList<QwtWfLayer_t *> layers;
	
/*!
  \brief Append data to bottom
  
  Data 'data' is a linear array (of doubles) of size w*h.

  \param l Layer index (max: layers->count() - 1);
  \param data Array of double values. Size: w*h.
  \param w Width of the data block.
  \param h Height of the data block.
 */
	void	appendB(qint32 l, double *data, int w, int h);
/*!
  \brief Append data to bottom
  
  \param data Of type QRgb.
  
  \sa appendB
 */
	void	appendB(qint32 l, QRgb *data, int w, int h);
/*!
  \brief Append data from top
  
  Data 'data' is a linear array (of doubles) of size w*h.

  \param l Layer index (max: layers->count() - 1);
  \param data Array of double values. Size: w*h.
  \param w Width of the data block.
  \param h Height of the data block.
 */
	void	appendT(qint32 l, double *data, int w, int h);
/*!
  \brief Append data from top
  
  \param data Of type QRgb.

  \sa appendT
 */
	void	appendT(qint32 l, QRgb *data, int w, int h);

/*!
  \brief Append data from left
  
  Data 'data' is a linear array (of doubles) of size w*h.

  \param l Layer index (max: layers->count() - 1);
  \param data Array of double values. Size: w*h.
  \param w Width of the data block.
  \param h Height of the data block.
 */
	void	appendL(qint32 l, double *data, int w, int h);

/*!
  \brief Append data from left
  
  \param data Of type QRgb.
  
  \sa appendL 
  */
	void	appendL(qint32 l, QRgb *data, int w, int h);

/*!
  \brief Append data from right
  
  Data 'data' is a linear array (of doubles) of size w*h.

  \param l Layer index (max: layers->count() - 1);
  \param data Array of double values. Size: w*h.
  \param w Width of the data block.
  \param h Height of the data block.
 */
	void	appendR(qint32 l, double *data, int w, int h);

/*!
  \brief Append data from right
  
  \param data Of type QRgb.
  
  \sa appendR
 */	

	void	appendR(qint32 l, QRgb *data, int w, int h);

/*!
  \brief Attach QwtPlot widget parent
  
  \note This operation attaches a plot widget for WF layers drawing.
  
  \param plot QwtPlot widget where to place layers
 */
	void attach( QwtPlot *plot );

/*!
  \brief Attach axis
  
  \param l Layer index
  \param axid Qwt::Axis id
  \param p QwtPlot widget with axes to listen for changes
 */
	void attachAxis(qint32 l,  qint32 axid, QwtPlot *p);
	
	void reattachAxis(bool lock);

	virtual bool eventFilter( QObject *, QEvent * );

/*!
  \brief Update graph

  \note Use it in a timer of the parent QwtPlot
 */
	void	Update();

	bool	is_orig_set(){ return orig_set;}
	qreal	get_orig_w(){ return orig_w;}
	qreal	get_orig_h(){ return orig_h;}

	qreal		orig_w, orig_h;
	qreal		orig_xrange, orig_yrange;	// original ranges in scale measure
	qint32		native_xaxid, native_yaxid;	// native plot x and y axis id

	void lockForRead();
	void unlockForRead();

	QwtPlot *plott;
	
public Q_SLOTS:
	void replot();
    
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);


protected Q_SLOTS:
	void plotscaleDivChanged();

private:
//	bool		axis_used[QwtPlot:: axisCnt];
	QPainter	painter;
	QPainterPath	borderClip;
	bool		orig_set;

	QReadWriteLock	*rw_lock;
	
};


#endif // QWT_WATERFALL_H
