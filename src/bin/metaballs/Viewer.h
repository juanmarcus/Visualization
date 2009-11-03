#ifndef VIEWER_H
#define VIEWER_H

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>
#include <GL/glu.h>
#include <Cg/cg.h>

class Viewer : public QGLViewer
{
    Q_OBJECT

public:
    Viewer(QWidget *parent = 0);
    ~Viewer();
protected:
//	virtual void preDraw();
	virtual void draw();
	virtual void init();
	virtual void initCG();
	virtual void animate();
    virtual void keyPressEvent(QKeyEvent *e);
    void checkError(const char*);
    void drawQuads(float x, float y, float z);
    void vertex(float x, float y , float z);
    void render_backface();
    void enable_renderbuffers();
    void disable_renderbuffers();
    void render_buffer_to_screen();
    void reshape_ortho(int w, int h);
    void resizep(int w, int h);
    void draw_fullscreen_quad();
    void raycasting_pass();
    void create_metaball_texture();
    void set_tex_param(char* par, GLuint tex,const CGprogram &program,CGparameter param);
private:
	//animation data
	float angle;
	float maxangle;

	//shader data
	CGcontext cg_context;
	CGprofile cg_vprofile;
	CGprofile cg_gprofile;
	CGprofile cg_fprofile;
	CGprogram vertexprogram;
	CGprogram fragmentprogram;
	CGparameter param1,param2;
	CGparameter nummetaballs;
	CGparameter metaballs;

	//shader data
	float stepsize;
	GLuint metaball_texture;

	float t;
	int num_metaballs;
	float Metaballs[6][4];

	//viewing data
	bool toggle_visuals;

	//buffers
	GLuint renderbuffer;
	GLuint framebuffer;
	GLuint backface_buffer; // the FBO buffers
	GLuint final_image;

};

#endif // VIEWER_H
