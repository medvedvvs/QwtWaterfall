#include <qapplication.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include "plot.h"


class MainWindow: public QMainWindow
{
public:
    MainWindow( QWidget * = NULL );
};

MainWindow::MainWindow( QWidget *parent ):
	QMainWindow( parent )
{
QVBoxLayout *l = new QVBoxLayout();
QWidget *win = new QWidget();

QGridLayout *l1 = new QGridLayout();
QWidget *win1 = new QWidget();
QLabel *win2 = new QLabel("<font color=Red><b>Some label</b></font>");
win1->setLayout(l1);
l->addWidget(win1);

	win2->resize(50, 50);
	l1->addWidget(win2, 0, 1);

	l->setMargin( 5 );
	l->setSpacing( 5 );

	win->setLayout(l);
	setCentralWidget( win );

	Plot *plot = new Plot(NULL, NULL);
	plot->resize(200, 200);
//	l->addWidget(plot);
	plot->m_content->id = 1;
	l1->addWidget(plot, 0, 0);

	

	// make plot1 listen to changes of scales in plot
	Plot *plot1 = new Plot(plot, NULL );
	plot1->resize(300, 300);
	plot1->m_content->id = 2;
	l->addWidget(plot1);
    
/*
    QToolBar *toolBar = new QToolBar( this );

    QComboBox *rasterBox = new QComboBox( toolBar );
    rasterBox->addItem( "Wikipedia" );

    toolBar->addWidget( new QLabel( "Data ", toolBar ) );
    toolBar->addWidget( rasterBox );
    toolBar->addSeparator();

    QComboBox *modeBox = new QComboBox( toolBar );
    modeBox->addItem( "Nearest Neighbour" );
    modeBox->addItem( "Bilinear Interpolation" );

    toolBar->addWidget( new QLabel( "Resampling ", toolBar ) );
    toolBar->addWidget( modeBox );

    toolBar->addSeparator();

    QToolButton *btnExport = new QToolButton( toolBar );
    btnExport->setText( "Export" );
    btnExport->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnExport );

    addToolBar( toolBar );

    connect( modeBox, SIGNAL( activated( int ) ), plot, SLOT( setResampleMode( int ) ) );
    connect( btnExport, SIGNAL( clicked() ), plot, SLOT( exportPlot() ) );
*/
}

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    MainWindow mainWindow;
    mainWindow.resize( 800, 800 );
    mainWindow.show();


    return a.exec();
}
