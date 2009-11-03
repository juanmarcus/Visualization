#ifndef VIEWER_H
#define VIEWER_H

#include "ibi_qt/ibiQGLViewer.h"

using namespace ibi;

class Viewer: public ibiQGLViewer
{
Q_OBJECT

public:
	Viewer(QWidget *parent = 0);
	~Viewer();

	void init();
	void draw();
};

#endif // VIEWER_H
