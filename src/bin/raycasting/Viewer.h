#ifndef VIEWER_H
#define VIEWER_H

#include "ibi_gl/ibi_gl.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"
#include "ibi_qt/ibiQGLViewer.h"
#include "ibi_gl/Texture.h"
#include "ibi_texturemanager/TextureManager.h"
#include "ibi_gpuprogrammanager/GPUProgramManager.h"

using namespace ibi;

// Volume size
#define VOLUME_TEX_SIZE 128
#define WINDOW_SIZE 800

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
	void reshape_ortho(int w, int h);
	void draw_fullscreen_quad();

	//renderbuffer control
	void enable_renderbuffers();
	void disable_renderbuffers();

	// Texture control
	void create_volumetexture();

	// CG stuff
	GLuint renderbuffer;
	GLuint framebuffer;

	// the buffer textures
	GLuint backface_buffer; // the FBO buffers
	GLuint final_image;

	//rendering parameter
	float stepsize;
	bool toggle_visuals;

	//--------------------------------
	// Textures
	TextureManager textureManager;
	Texture* volume;

	//Shaders
	GPUProgramManager shaderManager;
	VertexProgram* vertexProgram;
	FragmentProgram* fragmentProgram;
	FragmentProgram::Parameter backface_texture_param;
	FragmentProgram::Parameter volume_texture_param;
	FragmentProgram::Parameter stepsize_param;
};

#endif // VIEWER_H
