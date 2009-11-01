#include "Viewer.h"

#include <GL/gl.h>
#include <GL/glut.h>
#include <QtGui>
#include <Cg/cgGL.h>
#include <iostream>
#include "ibi_geometry/Vector3.h"

using namespace qglviewer;
using namespace std;

#define WINDOW_SIZE 800
#define VOLUME_TEX_SIZE 128
#define METABALL_TEX_SIZE 128

Viewer::Viewer(QWidget *parent) :
	QGLViewer(parent)
{
	resize(WINDOW_SIZE, WINDOW_SIZE);
}

Viewer::~Viewer()
{
	cgDestroyContext(cg_context);
	checkError("Error destroying context");
}

void Viewer::init()
{
//	restoreStateFromFile();

	glewInit();

	// initialize all the OpenGL extensions
	if (glewGetExtension("glMultiTexCoord2fvARB"))
		cout << "GL_ARB_multitexture support " << endl;
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
		exit(1);
	}

	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glDisable(GL_LIGHTING);

	create_metaball_texture();

	toggle_visuals = true;
	stepsize = 1.0 / 50.0;
	num_metaballs = 6;

	for (int i = 0; i < num_metaballs; ++i)
	{
		Metaballs[i][0] = 0;
		Metaballs[i][1] = 0;
		Metaballs[i][2] = 0;
		Metaballs[i][3] = 0.1;
	}

	initCG();

}

void Viewer::initCG()
{
	//init cg
	cg_context = cgCreateContext();
	checkError("Error creating context");
	cgGLRegisterStates(cg_context);
	checkError("Error registering states");

	//init profiles
	cg_vprofile = cgGLGetLatestProfile(CG_GL_VERTEX);
	cg_gprofile = cgGLGetLatestProfile(CG_GL_GEOMETRY);
	cg_fprofile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

	//load programs
	vertexprogram = cgCreateProgramFromFile(cg_context, CG_SOURCE,
			"Shaders/Raycasting.cg", cg_vprofile, "vertex_main", 0);
	checkError("Unable to compile vertex program");
	cgGLLoadProgram(vertexprogram);
	checkError("Unable to load vertex program");

	fragmentprogram = cgCreateProgramFromFile(cg_context, CG_SOURCE,
			"Shaders/Raycasting.cg", cg_fprofile, "fragment_main", 0);
	checkError("Unable to compile fragment program");
	cgGLLoadProgram(fragmentprogram);
	checkError("Unable to load fragment program");

	//init FBO
	// Create the to FBO's one for the backside of the volumecube and one for the finalimage rendering
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

	glGenTextures(1, &backface_buffer);
	glBindTexture(GL_TEXTURE_2D, backface_buffer);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, WINDOW_SIZE, WINDOW_SIZE, 0,
			GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, backface_buffer, 0);

	glGenTextures(1, &final_image);
	glBindTexture(GL_TEXTURE_2D, final_image);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, WINDOW_SIZE, WINDOW_SIZE, 0,
			GL_RGBA, GL_FLOAT, NULL);

	glGenRenderbuffersEXT(1, &renderbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
			WINDOW_SIZE, WINDOW_SIZE);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_RENDERBUFFER_EXT, renderbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	set_tex_param("metaballs_tex", metaball_texture, fragmentprogram, param2);
}

void Viewer::vertex(float x, float y, float z)
{
	glColor3f(x, y, z);
	//	glTexCoord3f(x,y,z);
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

void Viewer::enable_renderbuffers()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
}

void Viewer::disable_renderbuffers()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

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

void Viewer::resizep(int w, int h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 0.01, 400.0);
	glMatrixMode(GL_MODELVIEW);
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

void Viewer::raycasting_pass()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, final_image, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cgGLEnableProfile(cg_vprofile);
	cgGLEnableProfile(cg_fprofile);
	cgGLBindProgram(vertexprogram);
	cgGLBindProgram(fragmentprogram);
	cgGLSetParameter1f(cgGetNamedParameter(fragmentprogram, "stepsize"),
			stepsize);

	//init params
	nummetaballs = cgGetNamedParameter(fragmentprogram, "num_metaballs");
	checkError("Unable to find num_Metaballs parameter");

	metaballs = cgGetNamedParameter(fragmentprogram, "Metaballs");
	checkError("Unable to find metaballs parameter");

	cgGLSetParameterArray4f(metaballs, 0, 10, Metaballs[0]);
	checkError("Error setting Metaballs parameter");

	cgSetParameter1i(nummetaballs, num_metaballs);

	set_tex_param("tex", backface_buffer, fragmentprogram, param1);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawQuads(1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);
	cgGLDisableProfile(cg_vprofile);
	cgGLDisableProfile(cg_fprofile);
}

void Viewer::set_tex_param(char* par, GLuint tex, const CGprogram &program,
		CGparameter param)
{
	param = cgGetNamedParameter(program, par);
	cgGLSetTextureParameter(param, tex);
	cgGLEnableTextureParameter(param);
}

void Viewer::create_metaball_texture()
{
	int size = METABALL_TEX_SIZE;
	GLfloat *data = new GLfloat[size];

	const float a = -0.444444;
	const float b = 1.888889;
	const float c = -2.444444;

	for (int i = 0; i < METABALL_TEX_SIZE; ++i)
	{
		float dist = ((float) i) / METABALL_TEX_SIZE;
		float dist2 = dist * dist;
		float dist4 = dist2 * dist2;
		float dist6 = dist4 * dist4;
		float r = (a * dist6) + (b * dist4) + (c * dist2) + 1;
		printf("%f,", r);
		data[i] = r;
	}

	//	glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &metaball_texture);
	glBindTexture(GL_TEXTURE_1D, metaball_texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_ALPHA, METABALL_TEX_SIZE, 0, GL_ALPHA,
			GL_FLOAT, data);

	delete[] data;
	cout << "metaball texture created" << endl;

}

//void Viewer::preDraw()
//{
//	emit drawNeeded();
//}

void Viewer::draw()
{
	//	resizep(WINDOW_SIZE, WINDOW_SIZE);
	glMatrixMode(GL_MODELVIEW);

	enable_renderbuffers();

	//	glLoadIdentity();
	//	glTranslatef(0, 0, -2.25);
	//	glRotatef(rotate, 0, 1, 1);
	glTranslatef(-0.5, -0.5, -0.5);

	render_backface();

	raycasting_pass();

	disable_renderbuffers();

	render_buffer_to_screen();

	resizep(800, 800);
}

void Viewer::animate()
{
	t += 0.06;
	//	Metaballs[0][2] = -3 + 1 * sin(t);
	Metaballs[0][0] = (sin(t) / 3);
	Metaballs[0][2] = (sin(t) / 3);

	Metaballs[1][0] = (-sin(t) / 3);
	Metaballs[1][1] = (-cos(t) / 6);

	Metaballs[2][1] = (cos(t) / 6);
	Metaballs[2][2] = (cos(t) / 6);

	Metaballs[3][2] = (sin(t) / 9);
	Metaballs[3][1] = (-sin(t) / 6);

	Metaballs[4][1] = (cos(t) / 9);
	Metaballs[4][0] = (-cos(t) / 6);

	Metaballs[5][1] = (sin(t) / 6);
	Metaballs[5][0] = (-cos(t) / 9);
	Metaballs[5][3] = 0.2 * ((cos(t) + 1)/2) + 0.02;
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
	const Qt::KeyboardModifiers modifiers = e->modifiers();

	bool handled = false;
	if ((e->key() == Qt::Key_W) && (modifiers == Qt::NoButton))
	{
		stepsize += 1.0 / 2048.0;
		handled = true;
		updateGL();
	}
	else if ((e->key() == Qt::Key_S) && (modifiers == Qt::NoButton))
	{
		stepsize -= 1.0 / 2048.0;
		handled = true;
		updateGL();
	}
	else if ((e->key() == Qt::Key_Q) && (modifiers == Qt::NoButton))
	{
		toggle_visuals = !toggle_visuals;
		handled = true;
		updateGL();
	}

	if (!handled)
		QGLViewer::keyPressEvent(e);
}

void Viewer::checkError(const char *situation)
{
	char buffer[4096];
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR)
	{
		if (error == CG_COMPILER_ERROR)
		{
			sprintf(buffer, "Situation: %s\n"
				"Error: %s\n\n"
				"Cg compiler output...\n", situation, string);
			printf("%s", buffer);
			printf("%s\n", cgGetLastListing(cg_context));
		}
		else
		{
			sprintf(buffer, "Situation: %s\n"
				"Error: %s", situation, string);
			printf("%s\n", buffer);
		}
		exit(1);
	}
}
