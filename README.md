# Waterfall view widget for Qwt and Qt

![Waterfall QwtPlot screen](/WF.png)

![Real life example](/spectrograph1.png)

It is the beta version, but works fine. *Please check it.*

Basic idea is to keep qwt's ideology and portability while making all things
faster (compared to existing implementations over QwtSpectr..QwtRaster..etc) for intensive data streams.

Based on pure Qt's Qimage (not optimal, but...). Waterfall canvas is placed over the
parent QwtPlot->canvas().
So it is possible to place waterfall widget somewhere inside any QwtPlot.

Implemented features
* multiple layers support; each holding Qimage as a canvas
* data feed from top, bottom, left and right.
* control widget and colors/colormap opacity (alpha values)
* adding data of WxH size 2D blocks [maybe not equal to image W & H]
* fix waterfall layer in any position
* fix layer vertical or horizontal scale (noscaleX, noscaleY in the layer structure)
* attach (or not) layer to axes (scales) from different source QwtPlot plots (as in the example source code)
* only RGB colorspace is implemented for now
* obeys (+/-) Qt/Qwt styling of background widgets (rounded corners, for example).
* separate pan/zoom operations from source plot
* mouse-centered magnifier (for testing)

To compile, just edit project.pro and check the path to qwt config file *qwt.prf*,
then (*NIX):
 
 **~$ qmake project.pro**
 
 and 
 
 **~$ make**

Usage workflow (look at the example source code):
1. create QwtPlot with canvas
2. create waterfall widget
3. add waterfall layer[s] to the waterfall widget
4. attach waterfall widget to the plot canvas, where waterfall can draw layers
5. attach layers to [different] QwtPlot/axes
6. start timer in QwtPlot parent with WF->Update() call

Qt/qwt versions tested: 5.12.* vs 6.1.4
