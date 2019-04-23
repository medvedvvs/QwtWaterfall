# Waterfall view widget for Qwt and Qt

![WF QwtPlot screen](/WF.png)

It is the alpha version, but works nice. *Please check it.*

Basic idea is to keep qwt's ideology and portability while making all things
faster (compared to existing implementations over QwtSpectr..QwtRaster..etc) for intensive data streams.

Based on pure Qt's Qimage (not optimal, but...). WF canvas is placed over the
parent QwtPlot->canvas().
So it is possible to place WF widget somewhere inside any QwtPlot.

Implemented features
* multiple layers support; each holding Qimage as a canvas
* data feed from top, bottom, left and right.
* control widget and colors/colormap opacity (alpha values)
* adding data of WxH size 2D blocks [not equal to image W & H]
* fix WF layer in any position
* fix layer vertical or horizontal scale (noscaleX, noscaleY in the layer structure)
* attach (or not) layer to axes (scales) from different QwtPlot widgets
* only RGB colorspace is implemented for now
* Obeys (+/-) Qt/Qwt styling of background widgets (rounded corners, for example).


To compile, just edit project.pro and check the path to qwt config file *qwt.prf*,
then (*NIX):
 
 **~$ qmake project.pro**
 
 and 
 
 **~$ make**

Usage workflow:
* create QwtPlot with canvas
* create WF widget
* add WF layer[s] to the WF widget
* attach WF widget to the plot canvas, where WF can draw layers
* attach layers to [different] QwtPlot/axes


Qt/qwt versions tested: 5.12.* vs 6.1.4
