#include "magnifier.h"
#include <qwt_math.h>
#include <qevent.h>
#include <qwidget.h>
#include <qwt_plot.h>
#include <qwt_scale_div.h>

class WFMagnifier::PrivateData
{
public:
    PrivateData():
        isEnabled( false ),
        wheelFactor( 0.9 ),
        wheelModifiers( Qt::NoModifier ),
        mouseFactor( 0.95 ),
        mouseButton( Qt::RightButton ),
        mouseButtonModifiers( Qt::NoModifier ),
        keyFactor( 0.9 ),
        zoomInKey( Qt::Key_Plus ),
        zoomInKeyModifiers( Qt::NoModifier ),
        zoomOutKey( Qt::Key_Minus ),
        zoomOutKeyModifiers( Qt::NoModifier ),
        mousePressed( false ),
        mouseCentered(false)
    {
            for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
            isAxisEnabled[axis] = true;
    }

    bool isEnabled;
    
    double wheelFactor;
    Qt::KeyboardModifiers wheelModifiers;

    double mouseFactor;

    Qt::MouseButton mouseButton;
    Qt::KeyboardModifiers mouseButtonModifiers;

    double keyFactor;

    int zoomInKey;
    Qt::KeyboardModifiers zoomInKeyModifiers;

    int zoomOutKey;
    Qt::KeyboardModifiers  zoomOutKeyModifiers;

    bool mousePressed;
    bool hasMouseTracking;
    QPoint mousePos;
    
    bool isAxisEnabled[QwtPlot::axisCnt];

    bool mouseCentered;    
    
};

/*!
   Constructor
   \param parent Widget to be magnified
*/
WFMagnifier::WFMagnifier( QWidget *parent ):
    QObject( parent )
{
    d_data = new PrivateData();
    setEnabled( true );
}

//! Destructor
WFMagnifier::~WFMagnifier()
{
    delete d_data;
}


/*!
  \brief En/disable the magnifier

  When enabled is true an event filter is installed for
  the observed widget, otherwise the event filter is removed.

  \param on true or false
  \sa isEnabled(), eventFilter()
*/
void WFMagnifier::setEnabled( bool on )
{
    if ( d_data->isEnabled != on )
    {
        d_data->isEnabled = on;

        QObject *o = parent();
        if ( o )
        {
            if ( d_data->isEnabled )
                o->installEventFilter( this );
            else
                o->removeEventFilter( this );
        }
    }
}

/*!
  \return true when enabled, false otherwise
  \sa setEnabled(), eventFilter()
*/
bool WFMagnifier::isEnabled() const
{
    return d_data->isEnabled;
}

/*!
   \brief Change the wheel factor

   The wheel factor defines the ratio between the current range
   on the parent widget and the zoomed range for each step of the wheel.

   Use values > 1 for magnification (i.e. 2.0) and values < 1 for
   scaling down (i.e. 1/2.0 = 0.5). You can use this feature for
   inverting the direction of the wheel.

   The default value is 0.9.

   \param factor Wheel factor
   \sa wheelFactor(), setWheelButtonState(),
       setMouseFactor(), setKeyFactor()
*/
void WFMagnifier::setWheelFactor( double factor )
{
    d_data->wheelFactor = factor;
}

/*!
   \return Wheel factor
   \sa setWheelFactor()
*/
double WFMagnifier::wheelFactor() const
{
    return d_data->wheelFactor;
}

/*!
   Assign keyboard modifiers for zooming in/out using the wheel.
   The default modifiers are Qt::NoModifiers.

   \param modifiers Keyboard modifiers
   \sa wheelModifiers()
*/
void WFMagnifier::setWheelModifiers( Qt::KeyboardModifiers modifiers )
{
    d_data->wheelModifiers = modifiers;
}

/*!
   \return Wheel modifiers
   \sa setWheelModifiers()
*/
Qt::KeyboardModifiers WFMagnifier::wheelModifiers() const
{
    return d_data->wheelModifiers;
}

/*!
   \brief Change the mouse factor

   The mouse factor defines the ratio between the current range
   on the parent widget and the zoomed range for each vertical mouse movement.
   The default value is 0.95.

   \param factor Wheel factor
   \sa mouseFactor(), setMouseButton(), setWheelFactor(), setKeyFactor()
*/
void WFMagnifier::setMouseFactor( double factor )
{
    d_data->mouseFactor = factor;
}

/*!
   \return Mouse factor
   \sa setMouseFactor()
*/
double WFMagnifier::mouseFactor() const
{
    return d_data->mouseFactor;
}

/*!
   Assign the mouse button, that is used for zooming in/out.
   The default value is Qt::RightButton.

   \param button Button
   \param modifiers Keyboard modifiers

   \sa getMouseButton()
*/
void WFMagnifier::setMouseButton(
    Qt::MouseButton button, Qt::KeyboardModifiers modifiers )
{
    d_data->mouseButton = button;
    d_data->mouseButtonModifiers = modifiers;
}

//! \sa setMouseButton()
void WFMagnifier::getMouseButton(
    Qt::MouseButton &button, Qt::KeyboardModifiers &modifiers ) const
{
    button = d_data->mouseButton;
    modifiers = d_data->mouseButtonModifiers;
}

/*!
   \brief Change the key factor

   The key factor defines the ratio between the current range
   on the parent widget and the zoomed range for each key press of
   the zoom in/out keys. The default value is 0.9.

   \param factor Key factor
   \sa keyFactor(), setZoomInKey(), setZoomOutKey(),
       setWheelFactor, setMouseFactor()
*/
void WFMagnifier::setKeyFactor( double factor )
{
    d_data->keyFactor = factor;
}

/*!
   \return Key factor
   \sa setKeyFactor()
*/
double WFMagnifier::keyFactor() const
{
    return d_data->keyFactor;
}

/*!
   Assign the key, that is used for zooming in.
   The default combination is Qt::Key_Plus + Qt::NoModifier.

   \param key
   \param modifiers
   \sa getZoomInKey(), setZoomOutKey()
*/
void WFMagnifier::setZoomInKey( int key,
    Qt::KeyboardModifiers modifiers )
{
    d_data->zoomInKey = key;
    d_data->zoomInKeyModifiers = modifiers;
}

/*!
   \brief Retrieve the settings of the zoom in key

   \param key Key code, see Qt::Key
   \param modifiers Keyboard modifiers

   \sa setZoomInKey()
*/
void WFMagnifier::getZoomInKey( int &key,
    Qt::KeyboardModifiers &modifiers ) const
{
    key = d_data->zoomInKey;
    modifiers = d_data->zoomInKeyModifiers;
}

/*!
   Assign the key, that is used for zooming out.
   The default combination is Qt::Key_Minus + Qt::NoModifier.

   \param key
   \param modifiers
   \sa getZoomOutKey(), setZoomOutKey()
*/
void WFMagnifier::setZoomOutKey( int key,
    Qt::KeyboardModifiers modifiers )
{
    d_data->zoomOutKey = key;
    d_data->zoomOutKeyModifiers = modifiers;
}

/*!
   \brief Retrieve the settings of the zoom out key

   \param key Key code, see Qt::Key
   \param modifiers Keyboard modifiers

   \sa setZoomOutKey()
*/
void WFMagnifier::getZoomOutKey( int &key,
    Qt::KeyboardModifiers &modifiers ) const
{
    key = d_data->zoomOutKey;
    modifiers = d_data->zoomOutKeyModifiers;
}

/*!
  \brief Event filter

  When isEnabled() is true, the mouse events of the
  observed widget are filtered.

  \param object Object to be filtered
  \param event Event

  \return Forwarded to QObject::eventFilter()

  \sa widgetMousePressEvent(), widgetMouseReleaseEvent(),
      widgetMouseMoveEvent(), widgetWheelEvent(), widgetKeyPressEvent()
      widgetKeyReleaseEvent()
*/
bool WFMagnifier::eventFilter( QObject *object, QEvent *event )
{
    if ( object && object == parent() )
    {
        switch ( event->type() )
        {
            case QEvent::MouseButtonPress:
            {
                widgetMousePressEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::MouseMove:
            {
                widgetMouseMoveEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::MouseButtonRelease:
            {
                widgetMouseReleaseEvent( static_cast<QMouseEvent *>( event ) );
                break;
            }
            case QEvent::Wheel:
            {
                widgetWheelEvent( static_cast<QWheelEvent *>( event ) );
                break;
            }
            case QEvent::KeyPress:
            {
                widgetKeyPressEvent( static_cast<QKeyEvent *>( event ) );
                break;
            }
            case QEvent::KeyRelease:
            {
                widgetKeyReleaseEvent( static_cast<QKeyEvent *>( event ) );
                break;
            }
            default:;
        }
    }
    return QObject::eventFilter( object, event );
}

/*!
  Handle a mouse press event for the observed widget.

  \param mouseEvent Mouse event
  \sa eventFilter(), widgetMouseReleaseEvent(), widgetMouseMoveEvent()
*/
void WFMagnifier::widgetMousePressEvent( QMouseEvent *mouseEvent )
{
    if ( parentWidget() == NULL )
        return;

    if ( ( mouseEvent->button() != d_data->mouseButton ) ||
        ( mouseEvent->modifiers() != d_data->mouseButtonModifiers ) )
    {
        return;
    }

    d_data->hasMouseTracking = parentWidget()->hasMouseTracking();

    parentWidget()->setMouseTracking( true );
    d_data->mousePos = mouseEvent->pos();
    d_data->mousePressed = true;
}

/*!
  Handle a mouse release event for the observed widget.

  \param mouseEvent Mouse event

  \sa eventFilter(), widgetMousePressEvent(), widgetMouseMoveEvent(),
*/
void WFMagnifier::widgetMouseReleaseEvent( QMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );

    if ( d_data->mousePressed && parentWidget() )
    {
        d_data->mousePressed = false;
        parentWidget()->setMouseTracking( d_data->hasMouseTracking );
    }
}

/*!
  Handle a mouse move event for the observed widget.

  \param mouseEvent Mouse event
  \sa eventFilter(), widgetMousePressEvent(), widgetMouseReleaseEvent(),
*/
void WFMagnifier::widgetMouseMoveEvent( QMouseEvent *mouseEvent )
{
    if ( !d_data->mousePressed )
        return;

    d_data->mousePos = mouseEvent->pos();

    const int dy = mouseEvent->pos().y() - d_data->mousePos.y();
    if ( dy != 0 )
    {
        double f = d_data->mouseFactor;
        if ( dy < 0 )
            f = 1 / f;

        rescale( f );
    }


}

/*!
  Handle a wheel event for the observed widget.

  \param wheelEvent Wheel event
  \sa eventFilter()
*/
void WFMagnifier::widgetWheelEvent( QWheelEvent *wheelEvent )
{
    if ( wheelEvent->modifiers() != d_data->wheelModifiers )
    {
        return;
    }

    d_data->mousePos = wheelEvent->pos();

    if ( d_data->wheelFactor != 0.0 )
    {
        /*
            A positive delta indicates that the wheel was
            rotated forwards away from the user; a negative
            value indicates that the wheel was rotated
            backwards toward the user.
            Most mouse types work in steps of 15 degrees,
            in which case the delta value is a multiple
            of 120 (== 15 * 8).
         */
        double f = qPow( d_data->wheelFactor,
            qAbs( wheelEvent->delta() / 120.0 ) );

        if ( wheelEvent->delta() > 0 )
            f = 1 / f;

        rescale( f );
    }
}

/*!
  Handle a key press event for the observed widget.

  \param keyEvent Key event
  \sa eventFilter(), widgetKeyReleaseEvent()
*/
void WFMagnifier::widgetKeyPressEvent( QKeyEvent *keyEvent )
{
    if ( keyEvent->key() == d_data->zoomInKey &&
        keyEvent->modifiers() == d_data->zoomInKeyModifiers )
    {
        rescale( d_data->keyFactor );
    }
    else if ( keyEvent->key() == d_data->zoomOutKey &&
        keyEvent->modifiers() == d_data->zoomOutKeyModifiers )
    {
        rescale( 1.0 / d_data->keyFactor );
    }
}

/*!
  Handle a key release event for the observed widget.

  \param keyEvent Key event
  \sa eventFilter(), widgetKeyReleaseEvent()
*/
void WFMagnifier::widgetKeyReleaseEvent( QKeyEvent *keyEvent )
{
    Q_UNUSED( keyEvent );
}

//! \return Parent widget, where the rescaling happens
QWidget *WFMagnifier::parentWidget()
{
    return qobject_cast<QWidget *>( parent() );
}

//! \return Parent widget, where the rescaling happens
const QWidget *WFMagnifier::parentWidget() const
{
    return qobject_cast<const QWidget *>( parent() );
}


void WFMagnifier::setAxisEnabled( int axis, bool on )
{
    if ( axis >= 0 && axis < QwtPlot::axisCnt )
        d_data->isAxisEnabled[axis] = on;
}

bool WFMagnifier::isAxisEnabled( int axis ) const
{
    if ( axis >= 0 && axis < QwtPlot::axisCnt )
        return d_data->isAxisEnabled[axis];

    return true;
}

QWidget *WFMagnifier::canvas()
{
    return parentWidget();
}

const QWidget *WFMagnifier::canvas() const
{
    return parentWidget();
}

QwtPlot *WFMagnifier::plot()
{
    QWidget *w = canvas();
    if ( w )
        w = w->parentWidget();

    return qobject_cast<QwtPlot *>( w );
}

const QwtPlot *WFMagnifier::plot() const
{
    const QWidget *w = canvas();
    if ( w )
        w = w->parentWidget();

    return qobject_cast<const QwtPlot *>( w );
}

void WFMagnifier::rescale( double factor )
{

    QwtPlot* plt = plot();
    if ( plt == NULL )
        return;

    factor = qAbs( factor );
    if ( factor == 1.0 || factor == 0.0 )
        return;

    bool doReplot = false;

    const bool autoReplot = plt->autoReplot();
    plt->setAutoReplot( false );

    for ( int axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
    {
        if ( isAxisEnabled( axisId ) )
        {
            const QwtScaleMap scaleMap = plt->canvasMap( axisId );

            double v1 = scaleMap.s1();
            double v2 = scaleMap.s2();

            if ( scaleMap.transformation() )
            {
                // the coordinate system of the paint device is always linear

                v1 = scaleMap.transform( v1 ); // scaleMap.p1()
                v2 = scaleMap.transform( v2 ); // scaleMap.p2()
            }

            double center, width_2, width_3;
            if(!d_data->mouseCentered) {
            	center = 0.5 * ( v1 + v2 );
            	width_2 = 0.5 * ( v2 - v1 ) * factor;
            	width_3 = width_2;
	    } else {
            	double poz = 0, kz = 1, dst = 0;
            	if(axisId == QwtPlot::xBottom || axisId == QwtPlot::xTop) {
            		poz = d_data->mousePos.x();
			dst = scaleMap.pDist();
		} else
		if(axisId == QwtPlot::yLeft || axisId == QwtPlot::yRight) {
	   		poz = canvas()->height() - d_data->mousePos.y();
			dst = scaleMap.pDist();
			}
		kz = poz / dst;
		center = v1 + kz * qAbs( v2 - v1 );
		width_2 = kz * qAbs( v2 - v1 ) * factor;
		width_3 = (1.0 - kz) * qAbs( v2 - v1 ) * factor;
		
	    }

            v1 = center - width_2;
            v2 = center + width_3;

            if ( scaleMap.transformation() )
            {
                v1 = scaleMap.invTransform( v1 );
                v2 = scaleMap.invTransform( v2 );
            }

            plt->setAxisScale( axisId, v1, v2 );
            doReplot = true;
        }
    }

    plt->setAutoReplot( autoReplot );

    if ( doReplot )
        plt->replot();
}

bool WFMagnifier::mouseCentered() {
return d_data->mouseCentered;
}

void WFMagnifier::setMouseCentered(bool tf) {
d_data->mouseCentered = tf;
}
