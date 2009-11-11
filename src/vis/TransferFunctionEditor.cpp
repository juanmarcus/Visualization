#include "TransferFunctionEditor.h"

#include <algorithm>
#include "QtGui/QKeyEvent"

TransferFunctionEditor::TransferFunctionEditor(QWidget *parent) :
	ibiQGLViewer(parent), selectedPoint(-1)
{
	createActions();
}

TransferFunctionEditor::~TransferFunctionEditor()
{

}

void TransferFunctionEditor::createActions()
{
	addPointAct = new QAction(tr("Add point"), this);
	connect(addPointAct, SIGNAL(triggered()), this, SLOT(addPointSlot()));
}

void TransferFunctionEditor::init()
{
	setDesiredAspectRatio(4.0);
}

void TransferFunctionEditor::draw()
{
	start2DMode();

	glColor3f(1.0, 1.0, 1.0);
	drawFullScreenQuad();

	glColor3f(0.3, 0.3, 0.3);
	for (int i = 0; i < controlPoints.size(); ++i)
	{
		Vector3 vec = controlPoints[i];
		Vector3 point = absoluteViewportCoordinates(vec);
		if (i == selectedPoint)
		{
			glColor3f(0.0, 0.0, 0.0);
			geometryDrawer2d.drawCircle(point, 5);
			glColor3f(0.3, 0.3, 0.3);
		}
		else
		{
			geometryDrawer2d.drawCircle(point, 5);
		}
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
	bool handled = false;

	lastMouseClick = e->pos();

	if ((e->button() == Qt::RightButton) && (e->modifiers() == Qt::NoButton))
	{
		// Create context menu
		QMenu contextMenu(this);
		contextMenu.addAction(addPointAct);
		contextMenu.addSeparator();
		QMap<QAction*, int> pointMap;

		for (int i = 0; i < controlPoints.size(); ++i)
		{
			Vector3 vec = controlPoints[i];
			QString text("Point: ");
			text += QString::number(vec.x);
			text += " ";
			text += QString::number(vec.y);
			QAction* action = contextMenu.addAction(text);
			action->setCheckable(true);
			if (i == selectedPoint)
			{
				action->setChecked(true);
			}
			pointMap[action] = i;
		}

		QAction* action = contextMenu.exec(e->globalPos());
		if (pointMap.count(action) != 0)
		{
			selectedPoint = pointMap[action];
		}
		handled = true;
	}
	else if (e->button() == Qt::LeftButton)
	{
		handled = true;
	}

	if (!handled)
	{
		QGLViewer::mousePressEvent(e);
	}
}

bool comp(Vector3 vec1, Vector3 vec2)
{
	return (vec1.x <= vec2.x);
}

void TransferFunctionEditor::addPointSlot()
{
	Vector3 point = normalizedViewportCoordinates(lastMouseClick.x(),
			lastMouseClick.y());
	controlPoints.push_back(point);
	std::sort(controlPoints.begin(), controlPoints.end(), comp);
	selectedPoint = -1;
	updateGL();
}
