/*
 * view_nrrd_2d.cpp
 *
 *  Created on: Oct 8, 2009
 *      Author: Juan Ibiapina
 */

#include <QtGui/QApplication>

#include <string>
#include <iostream>

#include "qtopengl/ImageWidget.h"
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
	// Create a ImageWidget
	ImageWidget imgw;
	// Show the widget
	imgw.show();
	// Run the application loop
	return app.exec();
}

