#ifndef WF_MAGNIFIER_H
#define WF_MAGNIFIER_H 1

#include <qwt_global.h>
#include <qobject.h>

class QWidget;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

class QwtPlot;

class QWT_EXPORT WFMagnifier: public QObject
{
    Q_OBJECT

public:
    explicit WFMagnifier( QWidget * );
    virtual ~WFMagnifier();

    QWidget *parentWidget();
    const QWidget *parentWidget() const;
    
    void setAxisEnabled( int axis, bool on );
    bool isAxisEnabled( int axis ) const;

    QWidget *canvas();
    const QWidget *canvas() const;

    QwtPlot *plot();
    const QwtPlot *plot() const;


    void setEnabled( bool );
    bool isEnabled() const;

    // mouse
    void setMouseFactor( double );
    double mouseFactor() const;

    void setMouseButton( Qt::MouseButton, Qt::KeyboardModifiers = Qt::NoModifier );
    void getMouseButton( Qt::MouseButton &, Qt::KeyboardModifiers & ) const;

    // mouse wheel
    void setWheelFactor( double );
    double wheelFactor() const;

    void setWheelModifiers( Qt::KeyboardModifiers );
    Qt::KeyboardModifiers wheelModifiers() const;

    // keyboard
    void setKeyFactor( double );
    double keyFactor() const;

    void setZoomInKey( int key, Qt::KeyboardModifiers = Qt::NoModifier );
    void getZoomInKey( int &key, Qt::KeyboardModifiers & ) const;

    void setZoomOutKey( int key, Qt::KeyboardModifiers = Qt::NoModifier );
    void getZoomOutKey( int &key, Qt::KeyboardModifiers & ) const;

    virtual bool eventFilter( QObject *, QEvent * );
    
    // rescale center
    bool mouseCentered();
    void setMouseCentered(bool tf);
    
protected:
    /*!
       Rescale the parent widget
       \param factor Scale factor
     */
    void rescale( double factor);

    virtual void widgetMousePressEvent( QMouseEvent * );
    virtual void widgetMouseReleaseEvent( QMouseEvent * );
    virtual void widgetMouseMoveEvent( QMouseEvent * );
    virtual void widgetWheelEvent( QWheelEvent * );
    virtual void widgetKeyPressEvent( QKeyEvent * );
    virtual void widgetKeyReleaseEvent( QKeyEvent * );

private:
    class PrivateData;
    PrivateData *d_data;
};

#endif
