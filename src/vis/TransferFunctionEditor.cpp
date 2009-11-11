#include "TransferFunctionEditor.h"

#include <algorithm>
#include "QtGui/QColorDialog"
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

	// Read viewport size
	int vPort[4];
	glGetIntegerv(GL_VIEWPORT, vPort);
	int viewportMiddle = vPort[3] / 2;
	int viewportTop = vPort[3];
	int viewportWidth = vPort[2];

	// Draw a white working area
	glColor3f(1.0, 1.0, 1.0);
	drawFullScreenQuad();

	// Draw a division
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(0, viewportMiddle);
	glVertex2f(viewportWidth, viewportMiddle);
	glEnd();

	// Convert points to screen
	std::vector<Vector3> points;
	for (int i = 0; i < controlPoints.size(); ++i)
	{
		Vector3 vec = controlPoints[i].point;
		vec.y = (vec.y / 2) + 0.5;
		Vector3 point = absoluteViewportCoordinates(vec);
		points.push_back(point);
	}

	// Draw opacity lines
	glColor3f(0.0, 0.0, 0.0);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < points.size(); ++i)
	{
		Vector3 point = points[i];
		//			geometryDrawer2d.drawCircle(point, 5);
		glVertex2f(point.x, point.y);
	}
	glEnd();

	// Draw points
	glColor3f(0.0, 0.0, 0.0);
	for (int i = 0; i < points.size(); ++i)
	{
		Vector3 point = points[i];
		if (i == selectedPoint)
		{
			glColor3f(1.0, 0.0, 1.0);
			geometryDrawer2d.drawCircle(point, 5);
			glColor3f(0.0, 0.0, 0.0);
		}
		else
		{
			geometryDrawer2d.drawCircle(point, 5);
		}
	}

	// Draw colors
	glBegin(GL_QUADS);
	int max = points.size() - 1;
	for (int i = 0; i < max; ++i)
	{
		Vector3 p1 = points[i];
		Vector3 p2 = points[i + 1];
		Vector3 c1;
		c1.x = controlPoints[i].color.redF();
		c1.y = controlPoints[i].color.greenF();
		c1.z = controlPoints[i].color.blueF();

		Vector3 c2;
		c2.x = controlPoints[i + 1].color.redF();
		c2.y = controlPoints[i + 1].color.greenF();
		c2.z = controlPoints[i + 1].color.blueF();

		glColor3f(c1.x, c1.y, c1.z);
		glVertex2f(p1.x, 0);

		glColor3f(c2.x, c2.y, c2.z);
		glVertex2f(p2.x, 0);

		glColor3f(c2.x, c2.y, c2.z);
		glVertex2f(p2.x, viewportMiddle);

		glColor3f(c1.x, c1.y, c1.z);
		glVertex2f(p1.x, viewportMiddle);

		// for each line between two points
		// interpolate color and draw
	}
	glEnd();

	stop2DMode();
}

void TransferFunctionEditor::keyPressEvent(QKeyEvent *e)
{
	// Get event modifiers key
	const Qt::KeyboardModifiers modifiers = e->modifiers();

	bool handled = false;
	if ((e->key() == Qt::Key_Space) && (modifiers == Qt::NoButton))
	{
		if (selectedPoint != -1)
		{
			QColor color = QColorDialog::getColor(Qt::black, this);
			if (color.isValid())
			{
				controlPoints[selectedPoint].color = color;
				updateGL();
			}
			handled = true;
		}
	}

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
			Vector3 vec = controlPoints[i].point;
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

bool comp(ControlPoint p1, ControlPoint p2)
{
	return (p1.point.x <= p2.point.x);
}

void TransferFunctionEditor::addPointSlot()
{
	Vector3 point = normalizedViewportCoordinates(lastMouseClick.x(),
			lastMouseClick.y());
	point.y = (point.y - 0.5) * 2;

	if (point.x < 0.0 || point.x > 1.0 || point.y < 0.0 || point.y > 1.0)
	{
		return;
	}

	controlPoints.push_back(ControlPoint(point));
	std::sort(controlPoints.begin(), controlPoints.end(), comp);
	selectedPoint = -1;
	updateGL();
}
