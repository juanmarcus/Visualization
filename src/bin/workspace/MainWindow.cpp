#include "MainWindow.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMenuBar>
#include <QtGui/QMdiSubWindow>
#include <teem/nrrd.h>
#include "ibi_error/Exception.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent)
{
	// Configure MDI area
	mdiArea = new QMdiArea();
	setCentralWidget(mdiArea);

	raycastingViewer = new RaycastingViewer();
	mdiArea->addSubWindow(raycastingViewer);

	transferFunctionEditor = new TransferFunctionEditor();
	mdiArea->addSubWindow(transferFunctionEditor);

	// Initialize plugins
	raycastingViewer->loadPlugin("../ibi/build/lib/libtexture_loader_nrrd3D.so");
	raycastingViewer->loadPlugin(
			"../ibi/build/lib/libtexture_loader_transfer_func.so");

	createActions();
	createMenus();

	connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this,
			SLOT(updateMenus()));

}

MainWindow::~MainWindow()
{

}

void MainWindow::applyTransferFunctionSlot()
{
	QImage img = transferFunctionEditor->getTransferFunctionAsQImage();

	if (img.isNull())
	{
		return;
	}

	raycastingViewer->setTransferFunction(img);

}

void MainWindow::createActions()
{
	exitAct = new QAction(tr("Exit"), this);
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	applyTransferFunctionAct = new QAction(tr("Apply Transfer Function"), this);
	connect(applyTransferFunctionAct, SIGNAL(triggered()), this,
			SLOT(applyTransferFunctionSlot()));

}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("File"));
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	updateMenu = menuBar()->addMenu(tr("Update"));
	updateMenu->addAction(applyTransferFunctionAct);

	actionsMenu = menuBar()->addMenu(tr("Actions"));

	windowMenu = menuBar()->addMenu(tr("Window"));
}

void MainWindow::updateMenus()
{
	actionsMenu->clear();
	QMdiSubWindow* window = mdiArea->activeSubWindow();
	if (window == 0)
	{
		actionsMenu->addAction("No window selected");
	}
	else
	{
		QList<QAction*> actions = window->widget()->actions();
		if (actions.empty())
		{
			actionsMenu->addAction("No actions");
		}
		else
		{
			actionsMenu->addActions(actions);
		}
	}
}
