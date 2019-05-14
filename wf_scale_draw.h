#ifndef _WF_SCALE_DRAW_H_
#define _WF_SCALE_DRAW_H_

#include "qwt_global.h"
#include "qwt_scale_draw.h"
#include "qwt_date.h"

// dummy scale draw
class QWT_EXPORT WFScaleDraw: public QwtScaleDraw
{
public:
    virtual QwtText label( double ) const;
};

#endif
