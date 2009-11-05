#include "Viewer.h"

#include "ibi_geometry/Vector3.h"
#include <teem/nrrd.h>
#include <iostream>

using namespace std;
using namespace ibi;

// create a test volume texture, here you could load your own volume
void Viewer::create_volumetexture()
{
	textureManager.loadPlugin("../ibi/build/lib/libtexture_loader_nrrd3D.so");

	Nrrd* nin = nrrdNew();
	if (nrrdLoad(nin, "data/A-spgr-deface.nhdr", NULL))
	{
		char* err = biffGetDone(NRRD);
		cerr << err << endl;
	}

	TextureLoadingInfo info;
	info.target = GL_TEXTURE_3D;
	info.texture_type = "nrrd3D";
	info.options["nrrd"] = nin;

	volume = textureManager.load(info);

	volume->disable();

	cout << "volume texture created" << endl;

}

Viewer::Viewer(QWidget *parent) :
	ibiQGLViewer(parent)
{
	toggle_visuals = true;
	stepsize = 1.0 / 50.0;
	resize(WINDOW_SIZE, WINDOW_SIZE);
}

Viewer::~Viewer()
{

}

void Viewer::init()
{
	// Initialize glew
	GLenum err = glewInit();

	// initialize all OpenGL extensions
	glewGetExtension("glMultiTexCoord2fvARB");
	if (glewGetExtension("GL_EXT_framebuffer_object"))
		cout << "GL_EXT_framebuffer_object support " << endl;
	if (glewGetExtension("GL_EXT_renderbuffer_object"))
		cout << "GL_EXT_renderbuffer_object support " << endl;
	if (glewGetExtension("GL_ARB_vertex_buffer_object"))
		cout << "GL_ARB_vertex_buffer_object support" << endl;
	if (GL_ARB_multitexture)
		cout << "GL_ARB_multitexture support " << endl;

	if (glewGetExtension("GL_ARB_fragment_shader") != GL_TRUE
			|| glewGetExtension("GL_ARB_vertex_shader") != GL_TRUE
			|| glewGetExtension("GL_ARB_shader_objects") != GL_TRUE
			|| glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
	{
		cout << "Driver does not support OpenGL Shading Language" << endl;
	}

	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0);
	create_volumetexture();

	// CG init
	shaderManager.init();
	vertexProgram = shaderManager.loadVertexProgram(
			"Shaders/raycasting_shader.cg", "vertex_main");

	fragmentProgram = shaderManager.loadFragmentProgram(
			"Shaders/raycasting_shader.cg", "fragment_main");

	backface_texture_param = fragmentProgram->getNamedParameter("tex");
	volume_texture_param = fragmentProgram->getNamedParameter("volume_tex");
	stepsize_param = fragmentProgram->getNamedParameter("stepsize");

	// start framebuffer
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

	//Empty texture
	glGenTextures(1, &backface_buffer);
	glBindTexture(GL_TEXTURE_2D, backface_buffer);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, WINDOW_SIZE, WINDOW_SIZE, 0,
			GL_RGBA, GL_FLOAT, NULL);

	// what is this?
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, backface_buffer, 0);

	//Empty texture
	glGenTextures(1, &final_image);
	glBindTexture(GL_TEXTURE_2D, final_image);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, WINDOW_SIZE, WINDOW_SIZE, 0,
			GL_RGBA, GL_FLOAT, NULL);

	// start renderbuffer
//	glGenRenderbuffersEXT(1, &renderbuffer);
//	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);

	//what?
//	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
//			WINDOW_SIZE, WINDOW_SIZE);

	//what?
//	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
//			GL_RENDERBUFFER_EXT, renderbuffer);

	// deactivate framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		assert(0);
	}

	glDisable(GL_LIGHTING);

}

void Viewer::enable_renderbuffers()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
//	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
}

void Viewer::disable_renderbuffers()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Viewer::vertex(float x, float y, float z)
{
	glColor3f(x, y, z);
	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, x, y, z);
	glVertex3f(x, y, z);
}

// this method is used to draw the front and backside of the volume
void Viewer::drawQuads(float x, float y, float z)
{

	glBegin(GL_QUADS);
	/* Back side */
	glNormal3f(0.0, 0.0, -1.0);
	vertex(0.0, 0.0, 0.0);
	vertex(0.0, y, 0.0);
	vertex(x, y, 0.0);
	vertex(x, 0.0, 0.0);

	/* Front side */
	glNormal3f(0.0, 0.0, 1.0);
	vertex(0.0, 0.0, z);
	vertex(x, 0.0, z);
	vertex(x, y, z);
	vertex(0.0, y, z);

	/* Top side */
	glNormal3f(0.0, 1.0, 0.0);
	vertex(0.0, y, 0.0);
	vertex(0.0, y, z);
	vertex(x, y, z);
	vertex(x, y, 0.0);

	/* Bottom side */
	glNormal3f(0.0, -1.0, 0.0);
	vertex(0.0, 0.0, 0.0);
	vertex(x, 0.0, 0.0);
	vertex(x, 0.0, z);
	vertex(0.0, 0.0, z);

	/* Left side */
	glNormal3f(-1.0, 0.0, 0.0);
	vertex(0.0, 0.0, 0.0);
	vertex(0.0, 0.0, z);
	vertex(0.0, y, z);
	vertex(0.0, y, 0.0);

	/* Right side */
	glNormal3f(1.0, 0.0, 0.0);
	vertex(x, 0.0, 0.0);
	vertex(x, y, 0.0);
	vertex(x, y, z);
	vertex(x, 0.0, z);
	glEnd();

}

// render the backface to the offscreen buffer backface_buffer
void Viewer::render_backface()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, backface_buffer, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	drawQuads(1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);
}

void Viewer::raycasting_pass()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, final_image, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vertexProgram->enable();
	fragmentProgram->enable();

	cgGLSetParameter1f(stepsize_param.cgparameter, stepsize);

	cgGLSetTextureParameter(backface_texture_param.cgparameter, backface_buffer);
	cgGLEnableTextureParameter(backface_texture_param.cgparameter);

	volume_texture_param.setTexture(volume);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawQuads(1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);

	vertexProgram->disable();
	fragmentProgram->disable();
}

void Viewer::reshape_ortho(int w, int h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
}

void Viewer::draw_fullscreen_quad()
{
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex2f(0, 0);

	glTexCoord2f(1, 0);
	glVertex2f(1, 0);

	glTexCoord2f(1, 1);

	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(0, 1);

	glEnd();
	glEnable(GL_DEPTH_TEST);

}

// display the final image on the screen
void Viewer::render_buffer_to_screen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	if (toggle_visuals)
		glBindTexture(GL_TEXTURE_2D, final_image);
	else
		glBindTexture(GL_TEXTURE_2D, backface_buffer);
	reshape_ortho(WINDOW_SIZE, WINDOW_SIZE);
	draw_fullscreen_quad();
	glDisable(GL_TEXTURE_2D);
}

void Viewer::draw()
{
	enable_renderbuffers();

	glTranslatef(-0.5, -0.5, -0.5); // center the texturecube

	render_backface();

	raycasting_pass();

	disable_renderbuffers();

	render_buffer_to_screen();
}
