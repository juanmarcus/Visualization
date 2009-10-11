/*
 * test_imagewidget.cpp
 *
 *  Created on: Oct 11, 2009
 *      Author: Juan Ibiapina
 */

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

int main(int argc, char **argv)
{
	// Create an application
	QApplication app(argc, argv);
	// Create a ImageWidget
	QMainWindow w;
	// Show the widget
	w.show();
	// Run the application loop
	return app.exec();
}

