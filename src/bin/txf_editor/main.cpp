/*
 * main.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: Juan Ibiapina
 */

#include "ibi.h"
#include <QtGui/QApplication>
#include "TransferFunctionEditor.h"

int main(int argc, char **argv)
{
	// Create an application
	QApplication app(argc, argv);

	// Create a Viewer
	TransferFunctionEditor w;

	// Show the widget
	w.showMaximized();

	// Run the application loop
	return app.exec();
}
