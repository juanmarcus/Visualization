#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtGui/QLabel>

class ImageWidget: public QLabel
{
Q_OBJECT

public:
	ImageWidget(QWidget *parent = 0);
	~ImageWidget();

};

#endif // IMAGEWIDGET_H
