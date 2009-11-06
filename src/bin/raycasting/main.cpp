/*
 * main.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: Juan Ibiapina
 */

#include "ibi_gl/ibi_gl.h"
#include <QtGui/QApplication>
#include "RaycastingViewer.h"

int main(int argc, char **argv)
{
	// Create an application
	QApplication app(argc, argv);
	// Create a ImageWidget
	RaycastingViewer w;
	// Show the widget
	w.showMaximized();
	// Run the application loop
	return app.exec();
}
