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

void MainWindow::openVolumeSlot()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open volume",
			"data", "*.nhdr");
	if (!filename.isEmpty())
	{
		Nrrd* nin = nrrdNew();
		if (nrrdLoad(nin, filename.toStdString().c_str(), NULL))
		{
			char* err = biffGetDone(NRRD);
			throw Exception("MainWindow.cpp", "Problem loading nrrd", err);
		}

		// Prepare loading info for nrrd volume
		TextureLoadingInfo info;
		info.target = GL_TEXTURE_3D;
		info.texture_type = "nrrd3D";
		info.options["nrrd"] = nin;

		// Load volume texture
		Texture* volumeTexture = raycastingViewer->loadTexture(info);

		// Set the texture
		raycastingViewer->setVolume(volumeTexture);
	}
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

	openVolumeAct = new QAction(tr("Open volume"), this);
	connect(openVolumeAct, SIGNAL(triggered()), this, SLOT(openVolumeSlot()));

	applyTransferFunctionAct = new QAction(tr("Apply Transfer Function"), this);
	connect(applyTransferFunctionAct, SIGNAL(triggered()), this,
			SLOT(applyTransferFunctionSlot()));

}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("File"));
	fileMenu->addAction(openVolumeAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	updateMenu = menuBar()->addMenu(tr("Update"));
	updateMenu->addAction(applyTransferFunctionAct);

	actionsMenu = menuBar()->addMenu(tr("Actions"));
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
