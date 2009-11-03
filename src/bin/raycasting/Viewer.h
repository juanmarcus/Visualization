#ifndef VIEWER_H
#define VIEWER_H

#include "ibi_gl/ibi_gl.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"
#include "ibi_qt/ibiQGLViewer.h"

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

	// Error handling
	void checkError();

	// Texture control
	void create_volumetexture();

	// Shader control
	void load_vertex_program(CGprogram &v_program, char *shader_path,
			char *program_name);
	void load_fragment_program(CGprogram &f_program, char *shader_path,
			char *program_name);
	void set_tex_param(char* par, GLuint tex, const CGprogram &program,
			CGparameter param);

	// the volume texture
	GLuint volume_texture;

	// CG stuff
	CGcontext context;
	CGprofile vertexProfile, fragmentProfile;
	GLuint renderbuffer;
	GLuint framebuffer;

	// shaders
	CGprogram vertex_main, fragment_main; // the raycasting shader programs
	CGparameter param1, param2;

	// the buffer textures
	GLuint backface_buffer; // the FBO buffers
	GLuint final_image;

	//rendering parameter
	float stepsize;
	bool toggle_visuals;
};

#endif // VIEWER_H
