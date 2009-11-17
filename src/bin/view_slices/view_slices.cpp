/*
 * view_nrrd_2d.cpp
 *
 *  Created on: Oct 8, 2009
 *      Author: Juan Ibiapina
 */

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <teem/nrrd.h>

#include "ibi_cmdline/CommandLineParser.h"
#include "ibi_error/Exception.h"

#include "Viewer.h"

int main(int argc, char **argv)
{
	// Input values
	String inputFileName;
	unsigned int axis;

	// Define command line options
	CommandLineParser p;
	p.addOption("nin,i", value<String> (&inputFileName), "input nrrd",
			true);
	p.addOption("axis,a", value<unsigned int> (&axis), "axis to slice", true);

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

	if (axis >= nin->dim)
	{
		throw Exception("view_slices.cpp", "Problem setting axis.",
				"axis index out of bounds.");
	}

	// Create an application
	QApplication app(argc, argv);
	// Create a ImageWidget
	Viewer w;
	// Set parameters
	w.nin = nin;
	w.axis = axis;
	// Show the widget
	w.showMaximized();
	// Run the application loop
	return app.exec();
}

