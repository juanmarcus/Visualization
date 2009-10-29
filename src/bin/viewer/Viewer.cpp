#include "Viewer.h"

#include "ibi_geometry/Transform.h"
#include "ibi_geometry/Matrix4.h"
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
}

void Viewer::draw()
{
	const GLdouble* mod = manipulatedFrame()->matrix();
	Matrix4 m(mod[0], mod[4], mod[8], mod[12], mod[1], mod[5], mod[9], mod[13],
			mod[2], mod[6], mod[10], mod[14], mod[3], mod[7], mod[11], mod[15]);

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
	}

	// Test sample
	double value = sampler.sample(150, 100, 125);
	std::cout << "Sampling: " << value << std::endl;
	std::cout << "Real Value: " << nrrdUILookup[nin->type](nin->data, 150
			+ (100 * nin->axis[0].size) + (125 * nin->axis[0].size
			* nin->axis[1].size)) << std::endl;

}

