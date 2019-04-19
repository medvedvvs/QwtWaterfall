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
  \param Xaxis Attached Qwt X scale index (type of QwtPlot::Axis)
  \param Yaxis Attached Qwt Y scale index (type of QwtPlot::Axis)
  \param noscaleX Switch on/off the scaling of the layer on X
  \param noscaleY Switch on/off the scaling of the layer on Y
  \param opacity Layer (painter) opacity (0..1)
  \param range For internal use
  \param colorMap Color map used for the layer data
  \param rect Rect for image drawing, for internal use

  \sa QwtPlotWaterfall
 */
typedef struct QwtWfLayer_t {
	QImage		image;
	qint32		w, h;
	qreal		minx, miny, maxx, maxy;
	int		Xaxis, Yaxis;
	bool		noscaleX, noscaleY;
	qreal		opacity;
	QwtInterval	range;
	QwtColorMap	*colorMap;
	QRect		rect;
} QwtWfLayer_t;


class QwtPlotWaterfall : public QWidget
{
	Q_OBJECT

public:
	QwtPlotWaterfall(QWidget* parent = 0);
	~QwtPlotWaterfall();

public:

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
  \param width Widget height (as Qt widget)
  \param minx Minimum x value for the layer
  \param maxx Maximum x value for the layer
  \param miny Minimum y value for the layer
  \param maxy Maximum y value for the layer
  \param minval Minimum (expected) value of data
  \param maxval Maximum (expected) value of data
  \param Xa Attached Qwt X scale index (type of QwtPlot::Axis or -1 if not attached)
  \param Ya Attached Qwt Y scale index (type of QwtPlot::Axis or -1 if not attached)
  \param fm Of type QImage::Format. Supported QImage::Format_ARGB32 and QImage::Format_RGB32.
  \param fil Fill color for the layer (QColor).
  \param opacity Layer (painter) opacity [0..1.0]
 */
	bool addLayer(qint32 width, qint32 height, qreal minx, qreal miny, qreal maxx, qreal maxy, qreal minval, qreal maxval, int Xa, int Ya, QImage::Format fm, QColor fil, qreal opacity );

/*!
   \brief Delete a layer
   
   \param l Layer index
*/
	void deleteLayer(qint32 l);

/*!
  \brief Adds a new layer
  
  \param range Is of type QwtInterval

  \sa addLayer
 */
	bool addLayer(qint32 width, qint32 height, qreal minx, qreal miny, qreal maxx, qreal maxy, QwtInterval range, int Xa, int Ya, QImage::Format fm, QColor fil, qreal opacity );

/*!
  \brief Data color map
  
  It is avalaible as public member.
  Color map is used to map values inside the 2D region to color value.
  
  \sa setColorMap
 */
	QwtColorMap *colorMap;

/*!
  \brief QList of layers
  
  It is avalaible as public member.
  It is possible to use variables inside QwtWfLayer_t - noscaleX, noscaleY - to set static sizes of the layer.
  Default - false.

 */
	QList<QwtWfLayer_t *> layers;
	
/*!
  \brief Append data from bottom
  
  Data 'data' is a linear array (of doubles) of size w*h.

  \param l Layer index (max: layers->count() - 1);
  \param data Array of double values. Size: w*h.
  \param w Width of the data block.
  \param h Height of the data block.
 */
	void	appendB(qint32 l, double *data, int w, int h);
/*!
  \brief Append data from bottom
  
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
  
  \param plot QwtPlot widget where to place layers
 */
	void attach( QwtPlot *plot );
	
	virtual bool eventFilter( QObject *, QEvent * );
		
public Q_SLOTS:
	void replot();
    
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	QwtPlot *plott;

protected Q_SLOTS:
	void plotscaleDivChanged();

private:
	bool		axis_used[QwtPlot:: axisCnt];
	QPainter	painter;
	QPainterPath	borderClip;
	bool		orig_set;
	qint32		orig_w, orig_h;
	QReadWriteLock	*rw_lock;
};


#endif // QWT_WATERFALL_H
