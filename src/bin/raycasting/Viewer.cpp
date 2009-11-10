#include "Viewer.h"

#include "ibi_error/Exception.h"
#include <teem/nrrd.h>

Viewer::Viewer(QWidget *parent) :
	ibiQRaycastingViewer(parent)
{

}

Viewer::~Viewer()
{

}

void Viewer::initRaycasting()
{
	volumeTexture = loadVolume("data/A-spgr-deface_quant.nhdr");
	volumeTexture->disable();

	setVolume(volumeTexture);

	// Transfer function
	textureManager->loadPlugin(
			"../ibi/build/lib/libtexture_loader_transfer_func.so");
	TextureLoadingInfo txf_info;
	txf_info.target = GL_TEXTURE_1D;
	txf_info.texture_type = "transfer_func";
	txf_info.options["filename"] = String("data/txf_func.png");

	transferFunction = textureManager->load(txf_info);
	transferFunction->disable();

	setTransferFunction(transferFunction);
}

// create a test volume texture, here you could load your own volume
Texture* Viewer::loadVolume(String filename)
{
	textureManager = TextureManager::getInstance();
	textureManager->loadPlugin("../ibi/build/lib/libtexture_loader_nrrd3D.so");

	Nrrd* nin = nrrdNew();
	if (nrrdLoad(nin, filename.c_str(), NULL))
	{
		char* err = biffGetDone(NRRD);
		throw Exception("Viewer.cpp", "Problem loading nrrd.", err);
	}

	TextureLoadingInfo info;
	info.target = GL_TEXTURE_3D;
	info.texture_type = "nrrd3D";
	info.options["nrrd"] = nin;

	Texture* t = textureManager->load(info);

	nrrdNuke(nin);

	return t;

}
