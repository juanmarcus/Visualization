#ifndef VIEWER_H
#define VIEWER_H

#include "RaycastingViewer.h"

using namespace ibi;

class Viewer: public RaycastingViewer
{
Q_OBJECT

public:
	Viewer(QWidget *parent = 0);
	~Viewer();

	void initRaycasting();

	Texture* loadVolume(String filename);
private:
	// Volume texture
	Texture* volumeTexture;

	// Transfer function
	Texture* transferFunction;

	// Texture manager
	TextureManager* textureManager;
};

#endif // VIEWER_H
