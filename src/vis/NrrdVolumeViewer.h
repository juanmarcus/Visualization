#ifndef VIEWER_H
#define VIEWER_H

#include "RaycastingViewer.h"

#include <teem/nrrd.h>

using namespace ibi;

class NrrdVolumeViewer: public RaycastingViewer
{
Q_OBJECT

public:
	NrrdVolumeViewer(QWidget *parent = 0);
	~NrrdVolumeViewer();

	void initRaycasting();

	void setVolumeNrrd(Nrrd* nin);
	void setTransferFunctionFilename(String filename);
private:
	// Volume to render
	Nrrd* volume;

	// Transfer function filename
	String transfer_function_filename;

	// Texture manager
	TextureManager* textureManager;
};

#endif // VIEWER_H
