#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtOpenGL/QGLWidget>

class ImageWidget: public QGLWidget
{
Q_OBJECT

public:
	ImageWidget(QWidget *parent = 0);
	~ImageWidget();

};

#endif // IMAGEWIDGET_H
