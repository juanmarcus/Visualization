#ifndef RAYCASTINGVIEWER_H
#define RAYCASTINGVIEWER_H

#include "ibi_internal.h"
#include "ibi_gl/ibi_gl.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"
#include "ibi_qt/ibiQGLViewer.h"
#include "ibi_gl/Texture.h"
#include "ibi_gpuprogrammanager/GPUProgramManager.h"
#include "ibi_gl/FramebufferObject.h"
#include <teem/nrrd.h>

using namespace ibi;

class RaycastingViewer: public ibiQGLViewer
{
Q_OBJECT

public:
	RaycastingViewer(QWidget *parent = 0);
	~RaycastingViewer();

	void init();
	void initGlew();
	void initCG();
	void initFramebuffer();
	void draw();

	void setVolume(Nrrd* nin);
	void setTransferFunction(Texture* t);
	void setTransferFunction(QImage img);

public slots:
	void openVolumeSlot();
	void openTransferFunctionSlot();
	void setMultiplicativeFactorSlot(float value);
	void incMultiplicativeFactorSlot();
	void decMultiplicativeFactorSlot();

private:
	void createActions();
	void createMenus();

protected:
	// Draw geometry
	void vertex(float x, float y, float z);
	void drawQuads(float x, float y, float z);

	// Raycasting methods
	void render_backface();
	void raycasting_pass();
	void render_buffer_to_screen();

	/*
	 * Multiplicative factor
	 */
	float opacity;

	/*
	 * Distance between samples in a ray.
	 */
	float stepsize;

	bool toggle_visuals;

	// Volume data
	Nrrd* volume;
	NrrdRange* volume_range;
	float volume_scalex, volume_scaley, volume_scalez;

	// Textures
	Texture* volume_texture;
	Texture* backface;
	Texture* final_image;
	Texture* transfer_function;

	// GPU programs and parameters
	GPUProgramManager shaderManager;
	VertexProgram* vertexProgram;
	FragmentProgram* fragmentProgram;
	FragmentProgram::Parameter backface_texture_param;
	FragmentProgram::Parameter volume_texture_param;
	FragmentProgram::Parameter transfer_function_param;
	FragmentProgram::Parameter stepsize_param;
	FragmentProgram::Parameter opacity_param;

	// Framebuffer object
	FramebufferObject framebufferObject;

	// Actions
	QAction* openVolumeAct;
	QAction* openTransferFunctionAct;
	QAction* incMultiplicativeFactorAct;
	QAction* decMultiplicativeFactorAct;
};

#endif // RAYCASTINGVIEWER_H
