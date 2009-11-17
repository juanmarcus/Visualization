#include "RaycastingViewer.h"

#include "ibi_geometry/Vector3.h"
#include <iostream>

using namespace std;

RaycastingViewer::RaycastingViewer(QWidget *parent) :
	ibiQGLViewer(parent)
{
	toggle_visuals = true;
	stepsize = 1.0 / 100.0;
	volume = 0;
	transfer_function = 0;

	setDesiredAspectRatio(1.0);
}

RaycastingViewer::~RaycastingViewer()
{

}

void RaycastingViewer::init()
{
	initGlew();
	initCG();
	initFramebuffer();

	// Set some rendering parameters
	glDisable(GL_LIGHTING);
	setSceneRadius(0.65);
	showEntireScene();
	setDesiredAspectRatio(1.0);

	// Call user initialization
	initRaycasting();
}

void RaycastingViewer::initGlew()
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
}

void RaycastingViewer::initCG()
{
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
}

void RaycastingViewer::initFramebuffer()
{
	// Start framebuffer
	framebufferObject.init();

	// Start texture manager
	loadPlugin("../ibi/build/lib/libtexture_loader_empty.so");

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
	backface = loadTexture(info);
	backface->enable();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	backface->disable();

	// Final image
	final_image = loadTexture(info);
	final_image->enable();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	final_image->disable();

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

}

void RaycastingViewer::initRaycasting()
{

}

void RaycastingViewer::vertex(float x, float y, float z)
{
	glColor3f(x, y, z);
	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, x, y, z);
	glVertex3f(x, y, z);
}

// this method is used to draw the front and backside of the volume
void RaycastingViewer::drawQuads(float x, float y, float z)
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
void RaycastingViewer::render_backface()
{
	// draw to backface
	framebufferObject.setTarget(backface);
	framebufferObject.bind();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	drawQuads(1.0, 1.0, 1.0);

	framebufferObject.release();
}

void RaycastingViewer::raycasting_pass()
{
	// Draw to final image
	framebufferObject.setTarget(final_image);
	framebufferObject.bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vertexProgram->enable();
	fragmentProgram->enable();

	cgGLSetParameter1f(stepsize_param.cgparameter, stepsize);

	backface_texture_param.setTexture(backface);
	volume_texture_param.setTexture(volume);
	transfer_function_param.setTexture(transfer_function);

	glCullFace(GL_BACK);
	drawQuads(1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);

	vertexProgram->disable();
	fragmentProgram->disable();

	framebufferObject.release();
}

// display the final image on the screen
void RaycastingViewer::render_buffer_to_screen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (toggle_visuals)
		final_image->enable();
	else
		backface->enable();

	start2DMode();

	drawFullScreenQuad();

	stop2DMode();

	if (toggle_visuals)
		final_image->disable();
	else
		backface->disable();

}

void RaycastingViewer::draw()
{
	// do not draw if volume or transfer function not set
	if (!this->volume)
	{
		return;
	}

	if (!this->transfer_function)
	{
		return;
	}

	glTranslatef(-0.5, -0.5, -0.5); // center the texture cube

	render_backface();

	raycasting_pass();

	render_buffer_to_screen();
}

void RaycastingViewer::setVolume(Texture* t)
{
	if (this->volume)
	{
		delete this->volume;
	}
	this->volume = t;
}

void RaycastingViewer::setTransferFunction(Texture* t)
{
	this->transfer_function = t;
}

void RaycastingViewer::setTransferFunction(QImage img)
{
	makeCurrent();

	QImage imggl = QGLWidget::convertToGLFormat(img);
	Texture* t = new Texture();
	t->setTarget(GL_TEXTURE_1D);
	t->init();
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, imggl.width(), 0, GL_RGBA,
			GL_UNSIGNED_BYTE, imggl.bits());

	t->setDimensions(imggl.width());
	setTransferFunction(t);

	t->disable();
}
