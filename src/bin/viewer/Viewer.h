#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>

class Viewer: public QGLViewer
{
Q_OBJECT

public:
	Viewer(QWidget *parent = 0);
	~Viewer();

};

#endif // VIEWER_H
