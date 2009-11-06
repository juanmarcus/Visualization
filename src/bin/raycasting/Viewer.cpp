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
	textureManager.loadPlugin("../ibi/build/lib/libtexture_loader_empty.so");
	textureManager.loadPlugin("../ibi/build/lib/libtexture_loader_transfer_func.so");

	Nrrd* nin = nrrdNew();
	if (nrrdLoad(nin, "data/A-spgr-deface_quant.nhdr", NULL))
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

	nrrdNuke(nin);

	cout << "volume texture created" << endl;

}

Viewer::Viewer(QWidget *parent) :
	ibiQGLViewer(parent)
{
	toggle_visuals = true;
	stepsize = 1.0 / 100.0;

	setDesiredAspectRatio(1.0);
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
	create_volumetexture();

	// CG init
	shaderManager.init();
	vertexProgram = shaderManager.loadVertexProgram(
			"Shaders/raycasting_shader.cg", "vertex_main");

	fragmentProgram = shaderManager.loadFragmentProgram(
			"Shaders/raycasting_shader.cg", "fragment_main");

	backface_texture_param = fragmentProgram->getNamedParameter("tex");
	volume_texture_param = fragmentProgram->getNamedParameter("volume_tex");
	transfer_function_param = fragmentProgram->getNamedParameter("txf_func");
	stepsize_param = fragmentProgram->getNamedParameter("stepsize");

	// Start framebuffer
	framebuffer.init();
	framebuffer.enable();

	// Render textures specifications
	TextureLoadingInfo info;
	info.target = GL_TEXTURE_2D;
	info.texture_type = "empty";
	info.options["width"] = 800;
	info.options["height"] = 800;
	info.options["internalformat"] = GL_RGBA16F_ARB;
	info.options["format"] = GL_RGBA;
	info.options["type"] = GL_FLOAT;

	// Backface
	backface = textureManager.load(info);
	backface->enable();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Final image
	final_image = textureManager.load(info);
	final_image->enable();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Transfer function
	TextureLoadingInfo txf_info;
	txf_info.target = GL_TEXTURE_1D;
	txf_info.texture_type = "transfer_func";
	txf_info.options["filename"] = String("data/txf_func.png");

	transfer_function = textureManager.load(txf_info);
	transfer_function->disable();

	// Set target texture to render
	framebuffer.setTarget(backface);

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
	framebuffer.disable();

	glDisable(GL_LIGHTING);

	setSceneRadius(0.65);
	showEntireScene();
	setDesiredAspectRatio(1.0);
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
	// draw to backface
	framebuffer.setTarget(backface);
	framebuffer.beginRender();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	drawQuads(1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);

	framebuffer.endRender();
}

void Viewer::raycasting_pass()
{
	// Draw to final image
	framebuffer.setTarget(final_image);
	framebuffer.beginRender();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vertexProgram->enable();
	fragmentProgram->enable();

	cgGLSetParameter1f(stepsize_param.cgparameter, stepsize);

	backface_texture_param.setTexture(backface);
	volume_texture_param.setTexture(volume);
	transfer_function_param.setTexture(transfer_function);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawQuads(1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);

	vertexProgram->disable();
	fragmentProgram->disable();

	framebuffer.endRender();
}

// display the final image on the screen
void Viewer::render_buffer_to_screen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glLoadIdentity();
	if (toggle_visuals)
		final_image->enable();
	else
		backface->enable();

	start2DMode();

	drawFullScreenQuad();

	stop2DMode();

}

void Viewer::draw()
{
	glDisable(GL_TEXTURE_2D);

	glTranslatef(-0.5, -0.5, -0.5); // center the texturecube

	framebuffer.enable();

	render_backface();

	raycasting_pass();
	//	glClearColor(0.2, 0.2, 0.2, 0.0);

	framebuffer.disable();

	render_buffer_to_screen();
}
