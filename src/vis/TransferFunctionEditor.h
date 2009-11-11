#ifndef TRANSFERFUNCTIONEDITOR_H
#define TRANSFERFUNCTIONEDITOR_H

#include "ibi_qt/ibiQGLViewer.h"
#include <list>
#include "ibi_geometry/Vector3.h"
#include "ibi_gl/GeometryDrawer2D.h"
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

protected slots:
	void addPointSlot();

protected:
	void createActions();

private:
	// GeometryDrawer
	GeometryDrawer2D geometryDrawer2d;

	// Actions
	QAction* addPointAct;

	// Temporary
	QPoint lastMouseClick;

	std::vector<Vector3> controlPoints;
	int selectedPoint;

};

#endif // TRANSFERFUNCTIONEDITOR_H
