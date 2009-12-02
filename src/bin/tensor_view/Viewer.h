#ifndef VIEWER_H
#define VIEWER_H

#include "ibi_gl/ibi_gl.h"
#include "ibi_gl/GeometryDrawer.h"
#include "ibi_geometry/Ray.h"
#include "ibi_geometry/AxisAlignedBox.h"
#include "ibi_qt/ibiQGLViewer.h"
#include "RaycastingViewer.h"

using namespace ibi;

class Viewer: public RaycastingViewer
{
Q_OBJECT

public:
	Viewer(QWidget *parent = 0);
	~Viewer();

	void init();
	void draw();
private:
	Nrrd* tensor_nin;
};

#endif // VIEWER_H
