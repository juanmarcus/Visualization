#ifndef VIEWER_H
#define VIEWER_H

#include "ibi_gl/ibi_gl.h"
#include "ibi_gl/GLMode2D.h"
#include "ibi_gl/GeometryDrawer.h"
#include "ibi_geometry/Ray.h"
#include "ibi_geometry/AxisAlignedBox.h"
#include "ibi_qt/ibiQGLViewer.h"
#include "Sampler.h"

using namespace ibi;

class Viewer: public ibiQGLViewer
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
