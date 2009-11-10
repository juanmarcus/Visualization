#include "NrrdVolumeViewer.h"

#include "ibi_error/Exception.h"
#include <teem/nrrd.h>

NrrdVolumeViewer::NrrdVolumeViewer(QWidget *parent) :
	ibiQRaycastingViewer(parent), volume(0)
{

}

NrrdVolumeViewer::~NrrdVolumeViewer()
{

}

void NrrdVolumeViewer::initRaycasting()
{
	// Initialize TextureManager
	textureManager = TextureManager::getInstance();
	textureManager->loadPlugin("../ibi/build/lib/libtexture_loader_nrrd3D.so");
	textureManager->loadPlugin(
			"../ibi/build/lib/libtexture_loader_transfer_func.so");

	if (volume)
	{
		// Prepare loading info for nrrd volume
		TextureLoadingInfo info;
		info.target = GL_TEXTURE_3D;
		info.texture_type = "nrrd3D";
		info.options["nrrd"] = volume;

		// Load volume texture
		Texture* volumeTexture = textureManager->load(info);

		// Set the texture
		setVolume(volumeTexture);
	}

	if (transfer_function_filename != "")
	{
		// Prepare transfer function loading info
		TextureLoadingInfo txf_info;
		txf_info.target = GL_TEXTURE_1D;
		txf_info.texture_type = "transfer_func";
		txf_info.options["filename"] = transfer_function_filename;

		Texture* transfer_function = textureManager->load(txf_info);

		setTransferFunction(transfer_function);
	}
}

void NrrdVolumeViewer::setTransferFunctionFilename(String filename)
{
	this->transfer_function_filename = filename;
}

void NrrdVolumeViewer::setVolumeNrrd(Nrrd* nin)
{
	this->volume = nin;
}
