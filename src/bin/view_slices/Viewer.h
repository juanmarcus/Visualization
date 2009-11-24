#ifndef VIEWER_H
#define VIEWER_H

#include <QtGui/QKeyEvent>
#include <teem/nrrd.h>

#include <vector>

#include "ibi_qt/ibiQGLViewer.h"
#include "TextureConfigurator_Nrrd.h"

using namespace std;
using namespace ibi;

class Viewer: public ibiQGLViewer
{
Q_OBJECT

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

		setDesiredAspectRatio(1.0);
	}

	void draw()
	{
		glClear( GL_COLOR_BUFFER_BIT);

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
			nrrdQuantize(nout, slice, range, 8);

			TextureLoadingInfo info = TextureConfigurator_Nrrd::fromNrrd2D(nout);
			Texture* t = loadTexture(info);

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
	NrrdRange* range;
public:
	Nrrd* nin;
	unsigned int axis;

};

#endif // VIEWER_H
