#include "Viewer.h"

#include "ibi_geometry/Transform.h"
#include "ibi_geometry/Matrix4.h"
#include "ibi_gl/Texture.h"
#include "ibi_geometry/Intersection.h"
#include "ibi_interpolation/Interpolation.h"

using namespace qglviewer;

Viewer::Viewer(QWidget *parent) :
	QGLViewer(parent)
{
}

Viewer::~Viewer()
{

}

void Viewer::init()
{
	setManipulatedFrame(new ManipulatedFrame());

	setSceneRadius(2.0);
	showEntireScene();

	box.setExtents(-1, -1, -1, 1, 1, 1);
	//	restoreStateFromFile();

	nin = nrrdNew();
	if (nrrdLoad(nin, "data/A-spgr-deface.nhdr", NULL))
	{
		char* err = biffGetDone(NRRD);
		std::cerr << err << std::endl;
		free(err);
		nrrdNuke(nin);
	}

	sampler.setNrrd(nin);
	sampler.update();

	textureManager = TextureManager::getInstance();
	textureManager->loadPlugin("../ibi/build/lib/libtexture_loader_nrrd.so");
	loader = textureManager->getLoader("nrrd");
}

void Viewer::draw()
{
	const GLdouble* mod = manipulatedFrame()->matrix();
	Matrix4 m(mod[0], mod[4], mod[8], mod[12], mod[1], mod[5], mod[9], mod[13],
			mod[2], mod[6], mod[10], mod[14], mod[3], mod[7], mod[11], mod[15]);

	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0, 1.0, 1.0);

	// Transform ray
	Ray tray = Transform::TransformRay(m, ray);
	// Draw ray
	drawer.drawRay(tray, 4, 0.005);

	// Draw point at the origin
	drawer.drawPoint(Vector3::ZERO);

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
		std::vector<Vector3> ipoints = Interpolation::interpolate(intpoint1,
				intpoint2, 100);
		drawer.drawPoints(ipoints);

		// Calculate size
		int size = ipoints.size();

		// reserve memory space
		double* data = new double[size];

		// sample points and save to memory
		for (int i = 0; i < size; ++i)
		{
			// Get point
			Vector3 point = ipoints[i];

			// Calculate position on dataset
			double ix, iy, iz;

			ix = ((point.x + 1.0) / 2.0) * 500.0;
			iy = ((point.y + 1.0) / 2.0) * 500.0;
			iz = ((point.z + 1.0) / 2.0) * 240.0;

			// Sample
			double value = sampler.sample(ix, iy, iz);

			// Set sampled value in memory area
			data[i] = value;
		}
		Nrrd* wr = nrrdNew();
		if (nrrdWrap_va(wr, data, nrrdTypeDouble, 1, size))
		{
			char * err = biffGetDone(NRRD);
			throw Exception(err);
		}

		// Determine value range
		NrrdRange* range = nrrdRangeNewSet(wr, 0);

		// Make historgram
		Nrrd* dhist = nrrdNew();
		if (nrrdHistoDraw(dhist, wr, 50, 1, range->max))
		{
			char * err = biffGetDone(NRRD);
			throw Exception(err);
		}

		// Destroy range
		nrrdRangeNix(range);

		// Create a texture from the histogram
		TextureLoadingInfo info;
		info.target = GL_TEXTURE_2D;
		info.options["nrrd"] = dhist;
		Texture* t = loader->load(info);

		// Show texture
		mode2d.enable();

		t->enable();

		glColor3d(1.0, 1.0, 1.0);

		glBegin(GL_QUADS);

		glTexCoord2d(0.0, 0.0);
		glVertex2d(0.8, 0.8);

		glTexCoord2d(1.0, 0.0);
		glVertex2d(1.0, 0.8);

		glTexCoord2d(1.0, 1.0);
		glVertex2d(1.0, 1.0);

		glTexCoord2d(0.0, 1.0);
		glVertex2d(0.8, 1.0);

		glEnd();

		mode2d.disable();

		// Free data
		nrrdNuke(dhist);
		nrrdNix(wr);
		delete[] data;
		delete t;

	}

}

