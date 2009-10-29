#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include "ibi_geometry/Ray.h"
#include "ibi_gl2d/GLMode2D.h"
#include "ibi_gl/GeometryDrawer.h"
#include "ibi_geometry/AxisAlignedBox.h"
#include "ibi_qt/TextureLoader.h"
#include "Sampler.h"

class Viewer: public QGLViewer
{
Q_OBJECT

public:
	Viewer(QWidget *parent = 0);
	~Viewer();

	void init();
	void draw();
private:
	// drawing
	GeometryDrawer drawer;
	TextureLoader loader;
	GLMode2D mode2d;

	// Objects
	Ray ray;
	AxisAlignedBox box;

	// Sampler to query values from a nrrd
	Sampler sampler;

	// Dataset
	Nrrd* nin;
};

#endif // VIEWER_H
