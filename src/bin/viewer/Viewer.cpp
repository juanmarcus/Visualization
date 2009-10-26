#include "Viewer.h"

Viewer::Viewer(QWidget *parent) :
	QGLViewer(parent)
{

}

Viewer::~Viewer()
{

}

void Viewer::draw()
{
	drawer.draw(ray, 1);
}

