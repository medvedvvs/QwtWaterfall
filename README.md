# *"Dirty"* implementation of Waterfall view widget for Qwt

![WF QwtPlot screen](/WF.png)

It is the alpha version, but works nice. *Please check it.*
Basic idea is to keep qwt's ideology and portability while making the things
faster (compared to existing implementations over QwtSpectr....etc).

Based on pure Qt's Qimage. WF canvas is placed over the QwtPlot->canvas().

Options
* multiple layers support; each holding Qimage as a canvas
* adding data from top, bottom, left and right.
* control widget and colors/colormap opacity (alpha values)
* adding data of WxH size 2D blocks, not equal to image width and height
* fix WF layer in any position
* fix layer vertical or horizontal scale (noscaleX, noscaleY in the layer structure)
* attach (or not) layer to any set of axis

*Obeys (+/-)* Qt/Qwt styling of background widgets (round corners, for
example).

To compile, just edit project.pro and check the way to qwt config file *qwt.prf*,
then **qmake project.pro** and **make**.



Qt/qwt versions tested: 5.12.* vs 6.1.4
