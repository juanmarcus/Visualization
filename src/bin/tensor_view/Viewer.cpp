#include "Viewer.h"

#include "ibi_geometry/Transform.h"
#include "ibi_geometry/Matrix4.h"
#include "ibi_gl/Texture.h"
#include "ibi_gl/TextureLoadingInfo.h"
#include "ibi_geometry/Intersection.h"
#include "ibi_interpolation/Interpolation.h"
#include "TextureConfigurator_Nrrd.h"

using namespace qglviewer;

Viewer::Viewer(QWidget *parent) :
	RaycastingViewer(parent)
{
}

Viewer::~Viewer()
{

}

void Viewer::init()
{
	RaycastingViewer::init();
}

void Viewer::draw()
{
	RaycastingViewer::draw();
}

