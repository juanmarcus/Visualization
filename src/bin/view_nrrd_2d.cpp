/*
 * view_nrrd_2d.cpp
 *
 *  Created on: Oct 8, 2009
 *      Author: Juan Ibiapina
 */

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QKeyEvent>

#include <string>
#include <iostream>
#include <vector>

#include "ibi_cmdline/CommandLineParser.h"
#include "ibi_qt/ibiQtFunctorGLWidget.h"
#include "ibi_error/Exception.h"
#include "ibi_gl2d/GLMode2D.h"

#include <teem/nrrd.h>

using namespace std;

class GLWidget: public ibiQtSmartGLWidget
{
public:
	GLWidget() :
		ibiQtSmartGLWidget()
	{
		setViewportAuto(true);
		currentSlice = 0;
	}
	~GLWidget()
	{

	}

	void initializeGL()
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		textures.resize(nin->axis[axis].size, NULL);
		range = nrrdRangeNewSet(nin, 0);
	}

	void paintGL()
	{
		glClear(GL_COLOR_BUFFER_BIT);

		//		mode2d.enable();

		makeSlice(currentSlice);

		glBegin(GL_QUADS);

		glTexCoord2d(0.0, 0.0);
		glVertex2d(-1.0, -1.0);
		glTexCoord2d(1.0, 0.0);
		glVertex2d(+1.0, -1.0);
		glTexCoord2d(1.0, 1.0);
		glVertex2d(+1.0, +1.0);
		glTexCoord2d(0.0, 1.0);
		glVertex2d(-1.0, +1.0);

		glEnd();

		//		mode2d.disable();
	}

	void makeSlice(unsigned int n)
	{
		if (textures[n] != NULL)
		{
			textures[n]->apply();
		}
		else
		{
			Nrrd* slice = nrrdNew();
			nrrdSlice(slice, nin, axis, n);
			Nrrd* qslice = nrrdNew();
			nrrdQuantize(qslice, slice, range, 16);
			Texture* t = new Texture(FF_NRRD);
			t->setData(qslice->data);
			t->setDims(qslice->axis[0].size, qslice->axis[1].size);
			t->setDataFormat(GL_UNSIGNED_SHORT);
			loader.load(*t);
			textures[n] = t;
			t->apply();
			nrrdNuke(slice);
			nrrdNuke(qslice);
		}
	}

	void keyPressEvent(QKeyEvent * event)
	{
		if (event->key() == Qt::Key_Up)
		{
			currentSlice += 1;
			if (currentSlice >= nin->axis[axis].size)
			{
				currentSlice = nin->axis[axis].size - 1;
			}
		}
		else if (event->key() == Qt::Key_Down)
		{
			currentSlice -= 1;
			if (currentSlice < 0)
			{
				currentSlice = 0;
			}
		}
		updateGL();
	}

private:
	vector<Texture*> textures;
	int currentSlice;
	TextureLoader loader;
	GLMode2D mode2d;
	NrrdRange* range;
public:
	Nrrd* nin;
	unsigned int axis;
};

int main(int argc, char **argv)
{
	// Input values
	string inputFileName;
	unsigned int axis;

	// Define command line options
	CommandLineParser p;
	p.addOption("nin,i", value<std::string> (&inputFileName), "input nrrd",
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
		throw Exception("axis index out of bounds");
	}

	// Create an application
	QApplication app(argc, argv);
	// Create a ImageWidget
	GLWidget w;
	// Set parameters
	w.nin = nin;
	w.axis = axis;
	// Show the widget
	w.showMaximized();
	// Run the application loop
	return app.exec();
}

