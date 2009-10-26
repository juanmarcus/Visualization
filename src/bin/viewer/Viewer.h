#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include "ibi_geometry/Ray.h"
#include "ibi_gl/GeometryDrawer.h"

class Viewer: public QGLViewer
{
Q_OBJECT

public:
	Viewer(QWidget *parent = 0);
	~Viewer();

	void init();
	void draw();
private:
	GeometryDrawer drawer;
	Ray ray;
};

#endif // VIEWER_H
