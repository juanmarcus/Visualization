#include "TransferFunctionEditor.h"

#include "QtGui/QKeyEvent"

TransferFunctionEditor::TransferFunctionEditor(QWidget *parent) :
	ibiQGLViewer(parent)
{
	createActions();
	createContextMenu();
}

TransferFunctionEditor::~TransferFunctionEditor()
{

}

void TransferFunctionEditor::createContextMenu()
{
	contextMenu = new QMenu(this);
	contextMenu->addAction(addPointAct);
}

void TransferFunctionEditor::createActions()
{
	addPointAct = new QAction(tr("Add point"), this);
	connect(addPointAct, SIGNAL(triggered()), this, SLOT(addPointSlot()));
}

void TransferFunctionEditor::init()
{
	setDesiredAspectRatio(2.0);
}

void TransferFunctionEditor::draw()
{
	start2DMode();

	glColor3f(1.0, 1.0, 1.0);
	drawFullScreenQuad();

	std::list<Vector3>::iterator it = controlPoints.begin();
	std::list<Vector3>::iterator itEnd = controlPoints.end();

	glColor3f(0.0, 0.0, 0.0);
	for (; it != itEnd; ++it)
	{
		Vector3 point = absoluteViewportCoordinates(*it);
		std::cout << point << std::endl;
		geometryDrawer2d.drawCircle(point, 10);
	}

	stop2DMode();
}

void TransferFunctionEditor::keyPressEvent(QKeyEvent *e)
{
	// Get event modifiers key
	const Qt::KeyboardModifiers modifiers = e->modifiers();

	bool handled = false;
	//	if ((e->key() == Qt::Key_W) && (modifiers == Qt::NoButton))
	//	{
	//		handled = true;
	//		updateGL();
	//	}

	if (!handled)
		QGLViewer::keyPressEvent(e);
}

void TransferFunctionEditor::mousePressEvent(QMouseEvent* e)
{
	if ((e->button() == Qt::RightButton) && (e->modifiers() == Qt::NoButton))
	{
		lastMouseClick = e->pos();
		QAction* action = contextMenu->exec(e->globalPos());
	}
	else
		QGLViewer::mousePressEvent(e);
}

void TransferFunctionEditor::addPointSlot()
{
	Vector3 point = normalizedViewportCoordinates(lastMouseClick.x(),
			lastMouseClick.y());
	controlPoints.push_back(point);
	updateGL();
}
