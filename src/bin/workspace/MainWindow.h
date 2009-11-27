#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include "NrrdViewer.h"
#include "TransferFunctionEditor.h"

using namespace ibi;

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void applyTransferFunctionSlot();
	void updateMenus();

private:
	void createActions();
	void createMenus();

private:
	// MDI area
	QMdiArea* mdiArea;

	// Volume viewer
	NrrdViewer* nrrdViewer;

	// Transfer function editor
	TransferFunctionEditor* transferFunctionEditor;

	// Actions
	QAction* exitAct;
	QAction* applyTransferFunctionAct;

	// Menus
	QMenu* fileMenu;
	QMenu* updateMenu;
	QMenu* actionsMenu;
	QMenu* windowMenu;

};

#endif // MAINWINDOW_H
