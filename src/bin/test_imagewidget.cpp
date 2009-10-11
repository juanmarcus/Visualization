/*
 * test_imagewidget.cpp
 *
 *  Created on: Oct 11, 2009
 *      Author: Juan Ibiapina
 */

#include <QtGui/QApplication>

#include "qtopengl/ImageWidget.h"

int main(int argc, char **argv)
{
	// Create an application
	QApplication app(argc, argv);
	// Create a ImageWidget
	ImageWidget imgw;
	// Show the widget
	imgw.show();
	// Run the application loop
	return app.exec();
}

