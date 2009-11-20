/*
 * main.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: Juan Ibiapina
 */

#include "ibi.h"
#include "ibi_cmdline/CommandLineParser.h"
#include <QtGui/QApplication>
#include <teem/nrrd.h>
#include "RaycastingViewer.h"

int main(int argc, char **argv)
{
	// Input values
	String volume_filename;
	String txf_func_filename;

	// Define command line options
	CommandLineParser p;
	p.addOption("nin,i", value<std::string> (&volume_filename), "input nrrd",
			true);
	p.addOption("txf,t", value<std::string> (&txf_func_filename),
			"transfer function", true);

	// Parse command line options
	if (!p.parse(argc, argv))
	{
		return 1;
	}

	// Create a nrrd from the input file
	Nrrd* nin = nrrdNew();
	if (nrrdLoad(nin, volume_filename.c_str(), NULL))
	{
		char* err = biffGetDone(NRRD);
		fprintf(stderr, "error loading nrrd\n%s", err);
		free(err);
		return 1;
	}

	// Create an application
	QApplication app(argc, argv);

	// Create a Viewer
	RaycastingViewer w;

	// Show the widget
	w.showMaximized();

	// Run the application loop
	return app.exec();
}
