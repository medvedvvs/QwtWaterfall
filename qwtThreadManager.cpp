#include "qwtThreadManager.h"

qwtThreadManager::qwtThreadManager(QObject* object /*= 0*/)
	: QThread(object)
	, m_quit(false)
{

}

void qwtThreadManager::run()
{
int nn = 0;
double *data = (double*)malloc(10000 * sizeof(double));
int hei = 0;
bool over = false;
	m_quit = false;
	while (!m_quit && nn < 10000)
	{
//		printf("P: %d\n", nn);
//			const QString ttip = QStringLiteral("Image number %1.").arg(nn);
			nn++;

//			if (!ttip.isEmpty())
//			{
			for(int i = 0; i < 10000; i++)
				data[i] = hei;
			hei++;
			if(hei > 100) hei = 0;
			if(wf) {
			if(layer == -1) {
				// layer 0
				if(nn < 500)
				wf->appendT(0, data, 1000, 10);
				else
				wf->appendB(0, data, 1000, 10);

				// layer 1
				if(nn < 800)
				wf->appendB(1, data, 1000, 10);
				else
				wf->appendT(1, data, 1000, 10);

				// layer 2
				if(nn < 500)
				wf->appendR(2, data, 10, 500);
				else {
				if(!over) {
					printf("Delete layer #2.\n");
					wf->deleteLayer(2);
					over = true;
					}
				
				}
			} else {
				if(nn < 500)
					wf->appendT(layer, data, 1000, 10);
					else
					wf->appendB(layer, data, 1000, 10);
				}
			}
//			}
		//msleep(10);
		usleep(15000);
//		sleep(1);
	}
//	for(;;) {
	printf("------------------------------------------------------------------- Thread finished\n");
//	usleep(500000);
//	}
}

void qwtThreadManager::quit()
{
	m_quit = true;
	QThread::quit();
}

void qwtThreadManager::stopAndClear()
{	
	quit();
	wait();
}

qwtThreadManager::~qwtThreadManager()
{
	stopAndClear();
}

