#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	// Configure MDI area
	mdiArea = new QMdiArea();
	setCentralWidget(mdiArea);

	volumeViewer = new NrrdVolumeViewer();
	mdiArea->addSubWindow(volumeViewer);
}

MainWindow::~MainWindow()
{

}
