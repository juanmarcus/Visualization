#ifndef TRANSFERFUNCTIONEDITOR_H
#define TRANSFERFUNCTIONEDITOR_H

#include "ibi_qt/ibiQGLViewer.h"
#include <list>
#include "ibi_gl/Texture.h"
#include "ibi_texturemanager/TextureManager.h"
#include "ibi_geometry/Vector3.h"
#include "ibi_gl/GeometryDrawer2D.h"
#include "QtGui/QMenu"

using namespace ibi;

struct ControlPoint
{
	ControlPoint(const Vector3& a_point, const Vector3& a_color = Vector3::ZERO)
	{
		point = a_point;
		color = a_color;
	}
	Vector3 point;
	Vector3 color;
};

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

	Texture* getTransferFunctionAsTexture();
	QImage getTransferFunctionAsQImage();
public slots:
	void saveTextureDescription();
	void loadTextureDescription();
	void saveTexture();
	void addPointSlot();

protected:
	void createActions();
	void createMenus();

private:
	// GeometryDrawer
	GeometryDrawer2D geometryDrawer2d;

	// Actions
	QAction* addPointAct;

	// Menu
	QMenu* actionsMenu;

	// Temporary
	QPoint lastMouseClick;
	int selectedPoint;
	bool viewOpacity;

	// Transfer function data
	std::vector<ControlPoint> controlPoints;

};

#endif // TRANSFERFUNCTIONEDITOR_H
