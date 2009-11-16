#include "TransferFunctionEditor.h"

#include <algorithm>
#include "QtGui/QColorDialog"
#include "QtGui/QFileDialog"
#include "QtGui/QKeyEvent"
#include "ibi_framebuffer/Framebuffer.h"
#include <fstream>

using namespace std;

TransferFunctionEditor::TransferFunctionEditor(QWidget *parent) :
	ibiQGLViewer(QGLFormat(QGL::AlphaChannel), parent), selectedPoint(-1)
{
	viewOpacity = false;

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

void TransferFunctionEditor::ibi_init()
{
	// Init glew
	//	glewInit();
	loadPlugin("../ibi/build/lib/libtexture_loader_empty.so");

	glDisable(GL_BLEND);

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
	vector<Vector3> points;
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glBegin(GL_QUADS);
	int max = points.size() - 1;
	for (int i = 0; i < max; ++i)
	{
		Vector3 p1 = points[i];
		Vector3 p2 = points[i + 1];
		Vector3 c1 = controlPoints[i].color;
		Vector3 c2 = controlPoints[i + 1].color;

		float p1a;
		float p2a;

		if (viewOpacity)
		{
			p1a = controlPoints[i].point.y;
			p2a = controlPoints[i + 1].point.y;
		}
		else
		{
			p1a = 1.0;
			p2a = 1.0;
		}

		glColor4f(c1.x, c1.y, c1.z, p1a);
		glVertex2f(p1.x, 0);

		glColor4f(c2.x, c2.y, c2.z, p2a);
		glVertex2f(p2.x, 0);

		glColor4f(c2.x, c2.y, c2.z, p2a);
		glVertex2f(p2.x, viewportMiddle);

		glColor4f(c1.x, c1.y, c1.z, p1a);
		glVertex2f(p1.x, viewportMiddle);

	}
	glEnd();

	glDisable(GL_BLEND);

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
			Vector3 col = controlPoints[selectedPoint].color;
			QColor orig = QColor::fromRgbF(col.x, col.y, col.z, 1.0);

			QColor color = QColorDialog::getColor(orig, this);
			if (color.isValid())
			{
				col.x = color.redF();
				col.y = color.greenF();
				col.z = color.blueF();
				controlPoints[selectedPoint].color = col;
			}
			handled = true;
		}
	}
	else if ((e->key() == Qt::Key_Delete) && (modifiers == Qt::NoButton))
	{
		if (selectedPoint != -1)
		{
			vector<ControlPoint>::iterator it = controlPoints.begin();
			vector<ControlPoint>::iterator itEnd = controlPoints.end();
			for (int i = 0; it != itEnd; ++it, ++i)
			{
				if (i == selectedPoint)
				{
					controlPoints.erase(it);
					break;
				}
			}
			selectedPoint = -1;
			handled = true;
		}
	}
	else if ((e->key() == Qt::Key_Up) && (modifiers == Qt::ShiftModifier))
	{
		if (selectedPoint != -1)
		{
			Vector3 point = controlPoints[selectedPoint].point;
			point.y += 0.01;
			if (point.y > 1.0)
			{
				point.y = 1.0;
			}
			controlPoints[selectedPoint].point = point;
			handled = true;
		}
	}
	else if ((e->key() == Qt::Key_Down) && (modifiers == Qt::ShiftModifier))
	{
		if (selectedPoint != -1)
		{
			Vector3 point = controlPoints[selectedPoint].point;
			point.y -= 0.01;
			if (point.y < 0.0)
			{
				point.y = 0.0;
			}
			controlPoints[selectedPoint].point = point;
			handled = true;
		}
	}
	else if ((e->key() == Qt::Key_Left) && (modifiers == Qt::NoButton))
	{
		selectedPoint -= 1;
		if (selectedPoint < 0)
		{
			selectedPoint = 0;
		}
		handled = true;
	}
	else if ((e->key() == Qt::Key_Right) && (modifiers == Qt::NoButton))
	{
		selectedPoint += 1;
		if (selectedPoint >= controlPoints.size())
		{
			selectedPoint = controlPoints.size() - 1;
		}
		handled = true;
	}
	else if ((e->key() == Qt::Key_L) && (modifiers == Qt::ControlModifier))
	{
		controlPoints.clear();
		handled = true;
	}
	else if ((e->key() == Qt::Key_S) && (modifiers == Qt::ControlModifier))
	{
		saveTextureDescription();
		handled = true;
	}
	else if ((e->key() == Qt::Key_T) && (modifiers == Qt::ControlModifier))
	{
		saveTexture();
		handled = true;
	}
	else if ((e->key() == Qt::Key_O) && (modifiers == Qt::ControlModifier))
	{
		loadTextureDescription();
		handled = true;
	}
	else if ((e->key() == Qt::Key_O) && (modifiers == Qt::NoModifier))
	{
		viewOpacity = !viewOpacity;
		handled = true;
	}

	if (handled)
	{
		updateGL();
	}
	else
	{
		QGLViewer::keyPressEvent(e);
	}
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

void TransferFunctionEditor::loadTextureDescription()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open", ".", "*.txt");
	if (!filename.isEmpty())
	{
		ifstream in(filename.toStdString().c_str());

		if (in.is_open())
		{
			controlPoints.clear();

			int size;
			in >> size;
			for (int i = 0; i < size; ++i)
			{
				Vector3 point;
				Vector3 color;
				in >> point.x;
				in >> point.y;
				in >> point.z;
				in >> color.x;
				in >> color.y;
				in >> color.z;
				controlPoints.push_back(ControlPoint(point, color));
			}
			in.close();
		}
	}
}

void TransferFunctionEditor::saveTextureDescription()
{
	QString filename = QFileDialog::getSaveFileName(this, "Save", ".", "*.txt");
	if (!filename.isEmpty())
	{
		ofstream out(filename.toStdString().c_str());

		if (out.is_open())
		{
			out << controlPoints.size() << "\n";
			for (int i = 0; i < controlPoints.size(); ++i)
			{
				Vector3 point = controlPoints[i].point;
				Vector3 color = controlPoints[i].color;
				out << point.x << " " << point.y << " " << point.z << "\n";
				out << color.x << " " << color.y << " " << color.z << "\n";
			}
			out.close();
		}
	}
}

Texture* TransferFunctionEditor::renderToTexture()
{
	int textureWidth = 64;
	int textureHeight = 16;

	// May be needed
	makeCurrent();

	// 2D mode
	GLMode2D mode2d;
	mode2d.setScreenDimensions(textureWidth, textureHeight);

	// Create target texture
	TextureLoadingInfo info;
	info.target = GL_TEXTURE_2D;
	info.texture_type = "empty";
	info.options["width"] = textureWidth;
	info.options["height"] = textureHeight;
	info.options["internalformat"] = GL_RGBA;
	info.options["format"] = GL_RGBA;
	info.options["type"] = GL_FLOAT;
	Texture* renderTarget = loadTexture(info);

	// Create the framebuffer and start rendering
	Framebuffer framebuffer;
	framebuffer.init();
	framebuffer.setTarget(renderTarget);
	//		QGLFramebufferObject framebuffer(textureWidth, textureHeight,
	//				GL_TEXTURE_2D);
	framebuffer.bind();
	mode2d.enable();
	glViewport(0, 0, textureWidth, textureHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	mode2d.drawFullScreenQuad();

	Vector3 pi = controlPoints[0].point;
	Vector3 pf = controlPoints[controlPoints.size() - 1].point;

	float dx = pf.x - pi.x;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	glBegin(GL_QUADS);
	for (int i = 0; i < controlPoints.size() - 1; ++i)
	{
		Vector3 p1 = controlPoints[i].point;
		Vector3 c1 = controlPoints[i].color;
		Vector3 p2 = controlPoints[i + 1].point;
		Vector3 c2 = controlPoints[i + 1].color;

		float x1 = ((p1.x - pi.x) / dx) * textureWidth;
		float x2 = ((p2.x - pi.x) / dx) * textureWidth;

		glColor4f(c1.x, c1.y, c1.z, p1.y);
		glVertex2f(x1, 0);

		glColor4f(c2.x, c2.y, c2.z, p2.y);
		glVertex2f(x2, 0);

		glColor4f(c2.x, c2.y, c2.z, p2.y);
		glVertex2f(x2, textureHeight);

		glColor4f(c1.x, c1.y, c1.z, p1.y);
		glVertex2f(x1, textureHeight);

	}
	glEnd();

	glDisable(GL_BLEND);

	// Stop rendering in 2D
	mode2d.disable();

	framebuffer.release();

	return renderTarget;
}

void TransferFunctionEditor::saveTexture()
{
	if (controlPoints.size() < 2)
	{
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, "Save", ".", "*.png");
	if (!filename.isEmpty())
	{

		Texture* result = renderToTexture();

		// Create the framebuffer and start rendering
		Framebuffer framebuffer;
		framebuffer.init();
		framebuffer.setTarget(result);
		//		QGLFramebufferObject framebuffer(textureWidth, textureHeight,
		//				GL_TEXTURE_2D);
		framebuffer.bind();

		// Read the framebuffer to an image
		int textureWidth = result->getWidth();
		int textureHeight = result->getHeight();
		QImage img(textureWidth, textureHeight, QImage::Format_ARGB32);
		glReadPixels(0, 0, textureWidth, textureHeight, GL_RGBA,
				GL_UNSIGNED_BYTE, img.bits());
		// OpenGL gives ABGR (i.e. RGBA backwards); Qt wants ARGB
		for (int y = 0; y < textureHeight; y++)
		{
			uint *q = (uint*) img.scanLine(y);
			for (int x = 0; x < textureWidth; ++x)
			{
				const uint pixel = *q;
				*q = ((pixel << 16) & 0xff0000) | ((pixel >> 16) & 0xff)
						| (pixel & 0xff00ff00);
				q++;
			}
		}

		framebuffer.release();

		img.save(filename);

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
	sort(controlPoints.begin(), controlPoints.end(), comp);
	selectedPoint = -1;
	updateGL();
}
