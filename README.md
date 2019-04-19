# *"Dirty"* implementation of Waterfall view widget for Qwt and Qt

![WF QwtPlot screen](/WF.png)

It is the alpha version, but works nice. *Please check it.*

Basic idea is to keep qwt's ideology and portability while making all things
faster (compared to existing implementations over QwtSpectr....etc) for intensive data streams.

Based on pure Qt's Qimage. WF canvas is placed over the QwtPlot->canvas().

Implemented features
* multiple layers support; each holding Qimage as a canvas
* adding data from top, bottom, left and right.
* control widget and colors/colormap opacity (alpha values)
* adding data of WxH size 2D blocks, not equal to image width and height
* fix WF layer in any position
* fix layer vertical or horizontal scale (noscaleX, noscaleY in the layer structure)
* attach (or not) layer to any set of axis
* only RGB colorspace implemented for now

*Obeys (+/-)* Qt/Qwt styling of background widgets (rounded corners, for
example).

To compile, just edit project.pro and check the path to qwt config file *qwt.prf*,
then 
 
 **qmake project.pro**
 
 and 
 
 **make**.



Qt/qwt versions tested: 5.12.* vs 6.1.4
