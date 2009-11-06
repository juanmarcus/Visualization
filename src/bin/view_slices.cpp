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

#include "ibi_qt/ibiQGLViewer.h"
#include "ibi_cmdline/CommandLineParser.h"
#include "ibi_error/Exception.h"
//#include "ibi_gl/GLMode2D.h"
#include "ibi_gl/Texture.h"
#include "ibi_texturemanager/TextureManager.h"

#include <teem/nrrd.h>

using namespace std;
using namespace ibi;

class Viewer: public ibiQGLViewer
{
public:
	Viewer() :
		ibiQGLViewer()
	{
		currentSlice = 0;
	}
	~Viewer()
	{

	}

	void init()
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		textures.resize(nin->axis[axis].size, NULL);
		range = nrrdRangeNewSet(nin, 0);

		textureManager = TextureManager::getInstance();
		textureManager->loadPlugin("../ibi/build/lib/libtexture_loader_nrrd.so");
		nrrdTextureLoader = textureManager->getLoader("nrrd");

		setDesiredAspectRatio(1.0);
	}

	void draw()
	{
		glClear(GL_COLOR_BUFFER_BIT);

		makeSlice(currentSlice);

		start2DMode();

		drawFullScreenQuad();

		stop2DMode();
	}

	void makeSlice(unsigned int n)
	{
		if (textures[n] != NULL)
		{
			textures[n]->enable();
		}
		else
		{
			Nrrd* slice = nrrdNew();
			nrrdSlice(slice, nin, axis, n);
			Nrrd* nout = nrrdNew();
			nrrdQuantize(nout,slice,range,8);

			TextureLoadingInfo info;
			info.texture_type = "nrrd";
			info.target = GL_TEXTURE_2D;
			info.options["nrrd"] = nout;

			Texture* t = nrrdTextureLoader->load(info);

			textures[n] = t;

			t->enable();
			nrrdNuke(slice);
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
	TextureManager* textureManager;
	TextureLoader* nrrdTextureLoader;
	//	GLMode2D mode2d;
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
	Viewer w;
	// Set parameters
	w.nin = nin;
	w.axis = axis;
	// Show the widget
	w.showMaximized();
	// Run the application loop
	return app.exec();
}

