#ifndef VIEWER_H
#define VIEWER_H

#include "ibi_gl/ibi_gl.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"
#include "ibi_qt/ibiQGLViewer.h"
#include "ibi_gl/Texture.h"
#include "ibi_texturemanager/TextureManager.h"
#include "ibi_gpuprogrammanager/GPUProgramManager.h"
#include "ibi_framebuffer/Framebuffer.h"

using namespace ibi;

class Viewer: public ibiQGLViewer
{
Q_OBJECT

public:
	Viewer(QWidget *parent = 0);
	~Viewer();

	void init();
	void draw();

	//draw geometry
	void vertex(float x, float y, float z);
	void drawQuads(float x, float y, float z);

	//raycasting methods
	void render_backface();
	void raycasting_pass();
	void render_buffer_to_screen();

	// Texture control
	void create_volumetexture();

	//rendering parameter
	float stepsize;
	bool toggle_visuals;

	//--------------------------------
	// Textures
	TextureManager textureManager;
	Texture* volume;
	Texture* backface;
	Texture* final_image;

	// Shaders
	GPUProgramManager shaderManager;
	VertexProgram* vertexProgram;
	FragmentProgram* fragmentProgram;
	FragmentProgram::Parameter backface_texture_param;
	FragmentProgram::Parameter volume_texture_param;
	FragmentProgram::Parameter stepsize_param;

	// Framebuffer
	Framebuffer framebuffer;
};

#endif // VIEWER_H
