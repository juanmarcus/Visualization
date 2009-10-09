/*
 * view_nrrd_2d.cpp
 *
 *  Created on: Oct 8, 2009
 *      Author: Juan Ibiapina
 */

#include <QtGui/QApplication>

#include <string>
#include <iostream>

#include "widget/ImageWidget.h"
#include "widget/QPixmapFactory.h"
#include "cmdline/CommandLineParser.h"

#include <teem/nrrd.h>
#include <teem/biff.h>

using namespace std;

int main(int argc, char **argv)
{
	// Input values
	string inputFileName;

	// Define command line options
	CommandLineParser p;
	p.addOption("nin,i", value<std::string> (&inputFileName), "input nrrd",
			true);

	// Parse command line options
	if (!p.parse(argc, argv))
	{
		return 1;
	}

	// Create a nrrd from the input file
	Nrrd* nin = nrrdNew();
	if (nrrdLoad(nin, inputFileName.c_str(), NULL))
	{
		char* err = biffGetDone(NRRD);
		fprintf(stderr, "error loading nrrd\n%s", err);
		free(err);
		return 1;
	}

	// Create an application
	QApplication app(argc, argv);
	// Create an pixmap from the nrrd
	QPixmapFactory pixmapFactory;
	QPixmap* image = pixmapFactory.createPixmap(nin);
	// Create a ImageWidget
	ImageWidget imgw;
	imgw.setPixmap(*image);
	// Show the widget
	imgw.show();
	// Run the application loop
	return app.exec();
}

