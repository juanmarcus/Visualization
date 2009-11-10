#ifndef TRANSFERFUNCTIONEDITOR_H
#define TRANSFERFUNCTIONEDITOR_H

#include "ibi_qt/ibiQGLViewer.h"
#include "QtGui/QMenu"

using namespace ibi;

class TransferFunctionEditor: public ibiQGLViewer
{
Q_OBJECT

public:
	TransferFunctionEditor(QWidget *parent = 0);
	~TransferFunctionEditor();

	void init();
	void draw();

	void keyPressEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent* e);

public slots:
	void addPointSlot();

protected:
	void createContextMenu();
	void createActions();

private:
	// Context menu
	QMenu* contextMenu;

	// Actions
	QAction* addPointAct;

	// Temporary
	QPoint lastMouseClick;

};

#endif // TRANSFERFUNCTIONEDITOR_H
