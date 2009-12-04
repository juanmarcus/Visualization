#include "RaycastingViewer.h"

#include "ibi_geometry/Vector3.h"
#include "ibi_error/Exception.h"
#include <QtGui/QFileDialog>
#include <QtGui/QAction>
#include <iostream>
#include "ibi_qt/TextureConfigurator_Qt.h"
#include "TextureConfigurator_Nrrd.h"

using namespace std;

RaycastingViewer::RaycastingViewer(QWidget *parent) :
	ibiQGLViewer(parent)
{
	toggle_visuals = true;
	opacity = 1.0;
	stepsize = 1.0 / 100.0;
	volume = 0;
	volume_range = 0;
	volume_texture = 0;
	transfer_function = 0;
	volume_scalex = 1.0;
	volume_scaley = 1.0;
	volume_scalez = 1.0;

	createActions();
	createMenus();
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
	setDesiredAspectRatio(1.0);

}

void RaycastingViewer::initGlew()
{
	// Initialize glew
	GLenum err = glewInit();

	// initialize all OpenGL extensions
	glewGetExtension("glMultiTexCoord2fvARB");
	glewGetExtension("GL_EXT_framebuffer_object");
	glewGetExtension("GL_EXT_renderbuffer_object");
	glewGetExtension("GL_ARB_vertex_buffer_object");

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
	opacity_param = fragmentProgram->getNamedParameter("opacity");
}

void RaycastingViewer::initFramebuffer()
{
	// Start framebuffer
	framebufferObject.init();

	// Render textures specifications
	TextureLoadingInfo info;
	info.width = 800;
	info.height = 800;
	info.internalformat = GL_RGBA16F_ARB;
	info.format = GL_RGBA;
	info.type = GL_FLOAT;

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
	cgGLSetParameter1f(opacity_param.cgparameter, opacity);

	backface_texture_param.setTexture(backface);
	volume_texture_param.setTexture(volume_texture);
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
	if (!this->volume_texture)
	{
		return;
	}

	if (!this->transfer_function)
	{
		return;
	}

	glPushMatrix();

	glScalef(volume_scalex, volume_scaley, volume_scalez); // scale the texture cube
	glTranslatef(-0.5, -0.5, -0.5); // center the texture cube

	render_backface();

	raycasting_pass();

	render_buffer_to_screen();

	glPopMatrix();
}

void RaycastingViewer::setVolume(Nrrd* nin)
{
	this->volume = nin;

	// Load volume texture
	TextureLoadingInfo info = TextureConfigurator_Nrrd::fromNrrd3D(nin);
	Texture* volumeTexture = loadTexture(info);

	volumeTexture->enable();
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	volumeTexture->disable();

	this->volume_texture = volumeTexture;

	if (volume_range)
	{
		nrrdRangeNix(volume_range);
	}

	// Determine volume range
	volume_range = nrrdRangeNewSet(nin, 0);

	// Determine scaling
	volume_scalex = nin->axis[0].spaceDirection[0] * nin->axis[0].size;
	volume_scaley = nin->axis[1].spaceDirection[1] * nin->axis[1].size;
	volume_scalez = nin->axis[2].spaceDirection[2] * nin->axis[2].size;

	volume_scalex = volume_scalex / volume_scalez;
	volume_scaley = volume_scaley / volume_scalez;
	volume_scalez = 1.0;

	setSceneRadius(qMax(qMax(volume_scalex, volume_scaley), qMax(volume_scaley, volume_scalez)));
	showEntireScene();

}

void RaycastingViewer::setTransferFunction(Texture* t)
{
	this->transfer_function = t;

	t->enable();
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	t->disable();
}

void RaycastingViewer::setTransferFunction(QImage img)
{
	TextureLoadingInfo info = TextureConfigurator_Qt::fromQImage1D(img);
	Texture* t = loadTexture(info);

	setTransferFunction(t);
}

void RaycastingViewer::openVolumeSlot()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open volume", ".",
			"*.nhdr");
	if (!filename.isEmpty())
	{
		Nrrd* nin = nrrdNew();
		if (nrrdLoad(nin, filename.toStdString().c_str(), 0))
		{
			char* err = biffGetDone(NRRD);
			throw Exception("RaycastingViewer.cpp", "Problem loading nrrd.",
					"File not found: " + filename.toStdString());
		}

		setVolume(nin);
	}
}

void RaycastingViewer::openTransferFunctionSlot()
{
	QString filename = QFileDialog::getOpenFileName(this,
			"Open transfer function", ".", "*.png");
	if (!filename.isEmpty())
	{
		QImage img(filename);

		setTransferFunction(img);
	}
}

void RaycastingViewer::setMultiplicativeFactorSlot(float value)
{
	this->opacity = value;
	updateGL();
}

void RaycastingViewer::incMultiplicativeFactorSlot()
{
	opacity += 0.1;
	updateGL();
}

void RaycastingViewer::decMultiplicativeFactorSlot()
{
	opacity -= 0.1;
	updateGL();
}

void RaycastingViewer::createActions()
{
	openVolumeAct = new QAction(tr("Open volume"), this);
	openVolumeAct->setShortcut(QKeySequence::Open);
	connect(openVolumeAct, SIGNAL(triggered()), this, SLOT(openVolumeSlot()));
	addAction(openVolumeAct);

	openTransferFunctionAct = new QAction(tr("Open transfer function"), this);
	openTransferFunctionAct->setShortcut(tr("Ctrl+T"));
	connect(openTransferFunctionAct, SIGNAL(triggered()), this,
			SLOT(openTransferFunctionSlot()));
	addAction(openTransferFunctionAct);

	incMultiplicativeFactorAct = new QAction(tr(
			"Increase multiplicative factor"), this);
	incMultiplicativeFactorAct->setShortcut(tr("Ctrl+Up"));
	connect(incMultiplicativeFactorAct, SIGNAL(triggered()), this,
			SLOT(incMultiplicativeFactorSlot()));
	addAction(incMultiplicativeFactorAct);

	decMultiplicativeFactorAct = new QAction(tr(
			"Increase multiplicative factor"), this);
	decMultiplicativeFactorAct->setShortcut(tr("Ctrl+Down"));
	connect(decMultiplicativeFactorAct, SIGNAL(triggered()), this,
			SLOT(decMultiplicativeFactorSlot()));
	addAction(decMultiplicativeFactorAct);
}

void RaycastingViewer::createMenus()
{

}
