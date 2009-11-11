#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	// Configure MDI area
	mdiArea = new QMdiArea();
	setCentralWidget(mdiArea);

	volumeViewer = new NrrdVolumeViewer();
	mdiArea->addSubWindow(volumeViewer);

	transferFunctionEditor = new TransferFunctionEditor();
	mdiArea->addSubWindow(transferFunctionEditor);

}

MainWindow::~MainWindow()
{

}
