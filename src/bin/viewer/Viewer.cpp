#include "Viewer.h"

using namespace qglviewer;

Viewer::Viewer(QWidget *parent) :
	QGLViewer(parent)
{

}

Viewer::~Viewer()
{

}

void Viewer::init()
{
	setManipulatedFrame(new ManipulatedFrame());

	setSceneRadius(2.0);

//	restoreStateFromFile();
}

void Viewer::draw()
{
	// Change to frame coordinate system
	glPushMatrix();
	glMultMatrixd(manipulatedFrame()->matrix());
	// Draw ray
	drawer.draw(ray, 2, 0.005);
	// Change back to world coordinate system
	glPopMatrix();

}

