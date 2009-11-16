#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include "ibi_qt/ibiQRaycastingViewer.h"
#include "TransferFunctionEditor.h"

using namespace ibi;

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void openVolumeSlot();
	void applyTransferFunctionSlot();

private:
	void createActions();
	void createMenus();

private:
	// MDI area
	QMdiArea* mdiArea;

	// Volume viewer
	ibiQRaycastingViewer* raycastingViewer;

	// Transfer function editor
	TransferFunctionEditor* transferFunctionEditor;

	// Actions
	QAction* exitAct;
	QAction* openVolumeAct;
	QAction* applyTransferFunctionAct;

};

#endif // MAINWINDOW_H
