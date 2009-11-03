#include "Viewer.h"

#include "ibi_geometry/Vector3.h"
#include <iostream>

using namespace std;
using namespace ibi;

// create a test volume texture, here you could load your own volume
void Viewer::create_volumetexture()
{
	int size = VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4;
	GLubyte *data = new GLubyte[size];

	for (int x = 0; x < VOLUME_TEX_SIZE; x++)
	{
		for (int y = 0; y < VOLUME_TEX_SIZE; y++)
		{
			for (int z = 0; z < VOLUME_TEX_SIZE; z++)
			{
				data[(x * 4) + (y * VOLUME_TEX_SIZE * 4) + (z * VOLUME_TEX_SIZE
						* VOLUME_TEX_SIZE * 4)] = z % 250;
				data[(x * 4) + 1 + (y * VOLUME_TEX_SIZE * 4) + (z
						* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y % 250;
				data[(x * 4) + 2 + (y * VOLUME_TEX_SIZE * 4) + (z
						* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
				data[(x * 4) + 3 + (y * VOLUME_TEX_SIZE * 4) + (z
						* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 230;

				Vector3 p = Vector3(x, y, z) - Vector3(VOLUME_TEX_SIZE - 20,
						VOLUME_TEX_SIZE - 30, VOLUME_TEX_SIZE - 30);
				bool test = (p.length() < 42);
				if (test)
					data[(x * 4) + 3 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 0;

				p = Vector3(x, y, z) - Vector3(VOLUME_TEX_SIZE / 2,
						VOLUME_TEX_SIZE / 2, VOLUME_TEX_SIZE / 2);
				test = (p.length() < 24);
				if (test)
					data[(x * 4) + 3 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 0;

				if (x > 20 && x < 40 && y > 0 && y < VOLUME_TEX_SIZE && z > 10
						&& z < 50)
				{

					data[(x * 4) + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 100;
					data[(x * 4) + 1 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
					data[(x * 4) + 2 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y % 100;
					data[(x * 4) + 3 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
				}

				if (x > 50 && x < 70 && y > 0 && y < VOLUME_TEX_SIZE && z > 10
						&& z < 50)
				{

					data[(x * 4) + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
					data[(x * 4) + 1 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
					data[(x * 4) + 2 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y % 100;
					data[(x * 4) + 3 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
				}

				if (x > 80 && x < 100 && y > 0 && y < VOLUME_TEX_SIZE && z > 10
						&& z < 50)
				{

					data[(x * 4) + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
					data[(x * 4) + 1 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 70;
					data[(x * 4) + 2 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = y % 100;
					data[(x * 4) + 3 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 250;
				}

				p = Vector3(x, y, z) - Vector3(24, 24, 24);
				test = (p.length() < 40);
				if (test)
					data[(x * 4) + 3 + (y * VOLUME_TEX_SIZE * 4) + (z
							* VOLUME_TEX_SIZE * VOLUME_TEX_SIZE * 4)] = 0;

			}
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &volume_texture);
	glBindTexture(GL_TEXTURE_3D, volume_texture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, VOLUME_TEX_SIZE, VOLUME_TEX_SIZE,
			VOLUME_TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	delete[] data;
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

// load_vertex_program: loading a vertex program
void Viewer::load_vertex_program(CGprogram &v_program, char *shader_path,
		char *program_name)
{
	assert(cgIsContext(context));
	v_program = cgCreateProgramFromFile(context, CG_SOURCE, shader_path,
			vertexProfile, program_name, NULL);
	if (!cgIsProgramCompiled(v_program))
		cgCompileProgram(v_program);

	cgGLEnableProfile(vertexProfile);
	cgGLLoadProgram(v_program);
	cgGLDisableProfile(vertexProfile);
}

// load_fragment_program: loading a fragment program
void Viewer::load_fragment_program(CGprogram &f_program, char *shader_path,
		char *program_name)
{
	assert(cgIsContext(context));
	f_program = cgCreateProgramFromFile(context, CG_SOURCE, shader_path,
			fragmentProfile, program_name, NULL);
	if (!cgIsProgramCompiled(f_program))
		cgCompileProgram(f_program);

	cgGLEnableProfile(fragmentProfile);
	cgGLLoadProgram(f_program);
	cgGLDisableProfile(fragmentProfile);
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
	//	cgSetErrorCallback(cgErrorCallback);
	context = cgCreateContext();
	if (cgGLIsProfileSupported(CG_PROFILE_VP40))
	{
		vertexProfile = CG_PROFILE_VP40;
		cout << "CG_PROFILE_VP40 supported." << endl;
	}
	else
	{
		if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
			vertexProfile = CG_PROFILE_ARBVP1;
		else
		{
			cout
					<< "Neither arbvp1 or vp40 vertex profiles supported on this system."
					<< endl;
			exit(1);
		}
	}

	if (cgGLIsProfileSupported(CG_PROFILE_FP40))
	{
		fragmentProfile = CG_PROFILE_FP40;
		cout << "CG_PROFILE_FP40 supported." << endl;
	}
	else
	{
		if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
			fragmentProfile = CG_PROFILE_ARBFP1;
		else
		{
			cout
					<< "Neither arbfp1 or fp40 fragment profiles supported on this system."
					<< endl;
			exit(1);
		}
	}

	// load the vertex and fragment raycasting programs
	load_vertex_program(vertex_main, "Shaders/raycasting_shader.cg",
			"vertex_main");
	checkError();
	load_fragment_program(fragment_main, "Shaders/raycasting_shader.cg",
			"fragment_main");
	checkError();

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

	glDisable(GL_LIGHTING);

}

void Viewer::checkError()
{
	CGerror lastError = cgGetError();
	if (lastError)
	{
		cout << cgGetErrorString(lastError) << endl;
		if (context != NULL)
			cout << cgGetLastListing(context) << endl;
	}
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

// Sets a uniform texture parameter
void Viewer::set_tex_param(char* par, GLuint tex, const CGprogram &program,
		CGparameter param)
{
	param = cgGetNamedParameter(program, par);
	cgGLSetTextureParameter(param, tex);
	cgGLEnableTextureParameter(param);
}

void Viewer::raycasting_pass()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, final_image, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cgGLEnableProfile(vertexProfile);
	cgGLEnableProfile(fragmentProfile);
	cgGLBindProgram(vertex_main);
	cgGLBindProgram(fragment_main);
	cgGLSetParameter1f(cgGetNamedParameter(fragment_main, "stepsize"), stepsize);
	set_tex_param("tex", backface_buffer, fragment_main, param1);
	set_tex_param("volume_tex", volume_texture, fragment_main, param2);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawQuads(1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);
	cgGLDisableProfile(vertexProfile);
	cgGLDisableProfile(fragmentProfile);
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

	glTexCoord2f(0,0);
	glVertex2f(0,0);

	glTexCoord2f(1,0);
	glVertex2f(1,0);

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
