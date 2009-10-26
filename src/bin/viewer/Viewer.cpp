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
//	drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(1.0,1.0,1.0), 0.2, 5);
	drawArrow(1, 0.1);
}

