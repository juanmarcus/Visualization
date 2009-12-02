#include "NrrdViewer.h"

#include <QtGui/QAction>
#include "ibi_geometry/Matrix4.h"
#include "ibi_geometry/Transform.h"
#include "ibi_geometry/Intersection.h"
#include "ibi_interpolation/Interpolation.h"
#include "TextureConfigurator_Nrrd.h"
#include <teem/ten.h>
#include <QtGui/QFileDialog>

NrrdViewer::NrrdViewer(QWidget *parent) :
	RaycastingViewer(parent)
{
	doRayQuery = false;
	queryResult = 0;
	viewMode = 0;
	createActions();
}

NrrdViewer::~NrrdViewer()
{

}

void NrrdViewer::toggleRayQuerySlot()
{
	doRayQuery = !doRayQuery;
	updateGL();
}

void NrrdViewer::toggleShowRayImageSlot()
{
	switch (viewMode)
	{
	case 0:
		viewMode = 1;
		break;
	case 1:
		viewMode = 0;
		break;
	case 2:
		viewMode = 1;
		break;
	default:
		viewMode = 0;
		break;
	}
	updateGL();
}

void NrrdViewer::toggleShowRayHistogramSlot()
{
	switch (viewMode)
	{
	case 0:
		viewMode = 2;
		break;
	case 1:
		viewMode = 2;
		break;
	case 2:
		viewMode = 0;
		break;
	default:
		viewMode = 0;
		break;
	}
	updateGL();
}

void NrrdViewer::openTensorVolumeSlot()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open tensor volume",
			".", "*.nhdr");
	if (!filename.isEmpty())
	{
		Nrrd* tensor_nin = nrrdNew();
		if (nrrdLoad(tensor_nin, filename.toStdString().c_str(), 0))
		{
			char* err = biffGetDone(NRRD);
			throw Exception("Viewer.cpp", "Problem loading tensor volume.", err);
		}

		Nrrd* nout = nrrdNew();
		if (tenAnisoVolume(nout, tensor_nin, tenAniso_Tr, 0.5))
		{
			char* err = biffGetDone(TEN);
			throw Exception("Viewer.cpp", "Problem calculating anisotropy.",
					err);
		}
		nrrdNuke(tensor_nin);
		setVolume(nout);
	}
}

void NrrdViewer::init()
{
	RaycastingViewer::init();

	setManipulatedFrame(new qglviewer::ManipulatedFrame());

	box.setExtents(-0.5, -0.5, -0.5, 0.5, 0.5, 0.5);
}

void NrrdViewer::draw()
{
	RaycastingViewer::draw();

	if (doRayQuery)
	{
		// Read the frame matrix
		const GLdouble* mod = manipulatedFrame()->matrix();
		Matrix4 m(mod[0], mod[4], mod[8], mod[12], mod[1], mod[5], mod[9],
				mod[13], mod[2], mod[6], mod[10], mod[14], mod[3], mod[7],
				mod[11], mod[15]);

		// Set color to white
		glColor3f(1.0, 1.0, 1.0);

		// Transform ray
		Ray tray = Transform::TransformRay(m, ray);
		// Draw ray
		drawer.drawRay(tray, 4, 0.005);
		// Draw a box
		drawer.drawAxisAlignedBox(box);

		// Calculate ray box intersections and interpolate
		std::pair<bool, RealPair> result = Intersection::intersects(tray, box);
		if (result.first)
		{
			Real d1 = result.second.first;
			Real d2 = result.second.second;
			Vector3 intpoint1 = tray * d1;
			Vector3 intpoint2 = tray * d2;
			glColor3f(1.0, 0.0, 0.0);
			drawer.drawPoint(intpoint1);
			drawer.drawPoint(intpoint2);

			// Draw interpolation points
			glColor3f(0.0, 0.0, 1.0);
			std::vector<Vector3> ipoints = Interpolation::interpolate(
					intpoint1, intpoint2, 100);
			drawer.drawPoints(ipoints);

			// Calculate size
			int size = ipoints.size();

			if (volume)
			{
				// Prepare sampler
				sampler.setNrrd(volume);
				sampler.update();

				// Reserve memory space
				double* data = new double[size];
				//			// Sample points and save to memory
				for (int i = 0; i < size; ++i)
				{
					// Get point
					Vector3 point = ipoints[i];

					// Calculate position on dataset
					double ix, iy, iz;

					ix = (point.x + 0.5) * 500.0; // TODO fix this numbers
					iy = (point.y + 0.5) * 500.0;
					iz = (point.z + 0.5) * 240.0;

					// Sample
					double value = sampler.sample(ix, iy, iz);

					// Set sampled value in memory area
					data[i] = value;
				}

				// Wrap value into an nrrd
				Nrrd* wr = nrrdNew();
				if (nrrdWrap_va(wr, data, nrrdTypeDouble, 1, size))
				{
					char * err = biffGetDone(NRRD);
					throw Exception("Viewer.cpp", "Problem wrapping values.",
							err);
				}

				// Make ray image
				Nrrd* rayimage = nrrdNew();
				if (nrrdHistoDraw(rayimage, wr, 256, 1, volume_range->max))
				{
					char * err = biffGetDone(NRRD);
					throw Exception("Viewer.cpp", "Problem drawing ray image.",
							err);
				}

				// Make a histogram for the ray
				Nrrd* ray_histogram = nrrdNew();
				if (nrrdHisto(ray_histogram, wr, volume_range, 0, 64,
						nrrdTypeDouble))
				{
					char * err = biffGetDone(NRRD);
					throw Exception("Viewer.cpp", "Problem making histogram.",
							err);
				}

				NrrdRange* rayrange = nrrdRangeNewSet(wr, 0);

				// Draw the histogram
				Nrrd* ray_histogram_draw = nrrdNew();
				if (nrrdHistoDraw(ray_histogram_draw, ray_histogram, 64, 1, 0))
				{
					char * err = biffGetDone(NRRD);
					throw Exception("Viewer.cpp", "Problem drawing histogram.",
							err);
				}

				// Create a texture from the ray image
				TextureLoadingInfo info = TextureConfigurator_Nrrd::fromNrrd2D(
						rayimage);
				if (queryResult)
				{
					delete queryResult;
					queryResult = 0;
				}
				queryResult = loadTexture(info);

				// Create a texture from the histogram
				TextureLoadingInfo
						hinfo = TextureConfigurator_Nrrd::fromNrrd2D(
								ray_histogram_draw);
				Texture* thist = loadTexture(hinfo);

				start2DMode();
				switch (viewMode)
				{
				case 0:
					queryResult->enable();

					glBegin(GL_QUADS);

					glTexCoord2d(0.0, 0.0);
					glVertex2d(0.0, 0.0);

					glTexCoord2d(1.0, 0.0);
					glVertex2d(200.0, 0.0);

					glTexCoord2d(1.0, 1.0);
					glVertex2d(200.0, 100.0);

					glTexCoord2d(0.0, 1.0);
					glVertex2d(0.0, 100.0);

					glEnd();

					queryResult->disable();

					thist->enable();

					glBegin(GL_QUADS);

					glTexCoord2d(0.0, 0.0);
					glVertex2d(300, 0);

					glTexCoord2d(1.0, 0.0);
					glVertex2d(500.0, 0);

					glTexCoord2d(1.0, 1.0);
					glVertex2d(500.0, 100.0);

					glTexCoord2d(0.0, 1.0);
					glVertex2d(300.0, 100.0);

					glEnd();

					thist->disable();

					break;
				case 1:
					queryResult->enable();

					drawFullScreenQuad();

					queryResult->disable();
					break;
				case 2:
					thist->enable();

					drawFullScreenQuad();

					thist->disable();
				default:
					break;
				}
				stop2DMode();

				// Free memory
				delete[] data;
				nrrdNuke(rayimage);
				nrrdNuke(ray_histogram);
				nrrdNuke(ray_histogram_draw);
				delete thist;
				nrrdNix(wr);
			}
		}
	}
}

void NrrdViewer::createActions()
{
	toggleRayQueryAct = new QAction(tr("Toggle ray query"), this);
	toggleRayQueryAct->setShortcut(tr("q"));
	connect(toggleRayQueryAct, SIGNAL(triggered()), this,
			SLOT(toggleRayQuerySlot()));
	addAction(toggleRayQueryAct);

	toggleShowRayImageAct = new QAction(tr("Toggle show ray image"), this);
	toggleShowRayImageAct->setShortcut(tr("r"));
	connect(toggleShowRayImageAct, SIGNAL(triggered()), this,
			SLOT(toggleShowRayImageSlot()));
	addAction(toggleShowRayImageAct);

	toggleShowRayHistogramAct = new QAction(tr("Toggle show ray histgram"),
			this);
	toggleShowRayHistogramAct->setShortcut(tr("h"));
	connect(toggleShowRayHistogramAct, SIGNAL(triggered()), this,
			SLOT(toggleShowRayHistogramSlot()));
	addAction(toggleShowRayHistogramAct);

	openTensorVolumeAct = new QAction(tr("Open tensor volume"), this);
	openTensorVolumeAct->setShortcut(tr("Ctrl+Shift+O"));
	connect(openTensorVolumeAct, SIGNAL(triggered()), this,
			SLOT(openTensorVolumeSlot()));
	addAction(openTensorVolumeAct);
}
