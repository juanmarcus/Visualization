/*
 * main.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: Juan Ibiapina
 */

#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char **argv)
{
	// Create an application
	QApplication app(argc, argv);
	// Create a window
	MainWindow w;
	// Show the window
	w.showMaximized();
	// Run the application loop
	return app.exec();
}
