#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QMdiArea>
#include "NrrdVolumeViewer.h"
#include "TransferFunctionEditor.h"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	// MDI area
	QMdiArea* mdiArea;

	// Volume viewer
	NrrdVolumeViewer* volumeViewer;

	// Transfer function editor
	TransferFunctionEditor* transferFunctionEditor;

};

#endif // MAINWINDOW_H
