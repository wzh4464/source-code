////////////////////////////////////////////////////////////////////////////////////////////////////



// Mini Projecto of CG by WU Zihan


////////////////////////////////////////////////////////////////////////////////////////////////////


#include <stdlib.h>			// file reading
#include <time.h>			// for annimation
#include <math.h>			// for cos() and sin()
#include "glut.h"			// all gl~ commands
#include <Windows.h>		// for Sleep()
#include <iostream>			// io
#include <fstream>			// file reading

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>		// image reading
using namespace std;

//////////////////////////////////////////////////////////////////
// 
// Include the header file of our rotation user-interface.
// 
#include "gsrc.h"
// 
//////////////////////////////////////////////////////////////////

#define PI 3.141592654
#define GRIDSIZE 31
#define WIN_POSX   150
#define WIN_POSY   40
#define WIN_WIDTH  1000
#define WIN_HEIGHT 1000
#define OK 0
#define R1 1
#define R2 0.1		// small radius
#define THICK 0.1
#define HEIGHT 1
#define SS 0.2



static GLuint texNameArray[4];

unsigned char* mjk_image;
int mjk_depth;
int mjk_height;
int mjk_width;

int width;
int height;
int depth;
unsigned char* bits;

GLUquadricObj* quadratic;


typedef struct point { // define a structure for 3D point (x, y, z)
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vertex;

GLdouble t1 = 0;


vertex mesh[GRIDSIZE][GRIDSIZE];           // define a mesh whose elements are 3D point (x, y, z)
GLdouble theta = 0;


int LoadGLTextures(const char* FileName, GLuint* ttexture)
// load textures from FileName to ttexture
{
	
	mjk_image = stbi_load(FileName, &mjk_width, &mjk_height, &mjk_depth, 0);
	depth = mjk_depth;
	width = mjk_width;
	height = mjk_height;
	bits = mjk_image;
	glBindTexture(GL_TEXTURE_2D, *ttexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, depth, width, height,
		GL_RGB, GL_UNSIGNED_BYTE, bits);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	return OK;
}

void calculatecircle(void)
// calculate the parameters of the circle mesh
{
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)
		{
			mesh[i][j].x = cos((double)i / ((double)GRIDSIZE - 1) * PI - PI / 2) * cos((double)j / ((double)GRIDSIZE - 1) * 2 * PI);
			mesh[i][j].y = cos((double)i / ((double)GRIDSIZE - 1) * PI - PI / 2) * sin((double)j / ((double)GRIDSIZE - 1) * 2 * PI);
			mesh[i][j].z = sin((double)i / ((double)GRIDSIZE - 1) * PI - PI / 2);
		}
}

float ratiolize(int num)
// return ratio of num in GRIDSIZE
{
	return (double)num / ((double)GRIDSIZE - 1);
}

void ball(void)
// draw a ball which can be textured
{
	int i, j;
	for (i = 1; i < GRIDSIZE; i++)
	{
		for (j = 1; j < GRIDSIZE; j++)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(ratiolize(i - 1), ratiolize(j - 1));
			glVertex3f(mesh[i - 1][j - 1].x, mesh[i - 1][j - 1].y, mesh[i - 1][j - 1].z);
			glTexCoord2f(ratiolize(i), ratiolize(j - 1));
			glVertex3f(mesh[i][j - 1].x, mesh[i][j - 1].y, mesh[i][j - 1].z);
			glTexCoord2f(ratiolize(i), ratiolize(j));
			glVertex3f(mesh[i][j].x, mesh[i][j].y, mesh[i][j].z);
			glTexCoord2f(ratiolize(i - 1), ratiolize(j));
			glVertex3f(mesh[i - 1][j].x, mesh[i - 1][j].y, mesh[i - 1][j].z);
			glEnd();
		}
	}
}

void earthmoon(void)
// small rotating system
{
	int i;

	glPushMatrix();
	glScalef(0.05, 0.05, 0.05);
	glBindTexture(GL_TEXTURE_2D, texNameArray[3]);
	glEnable(GL_TEXTURE_2D);
	ball();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	for (i = 0; i < 3; i++)
	{
		glPushMatrix();
		glRotatef(t1 + i*360/3, 0, 0, 1);
		glTranslatef(0, R2, 0);
		glScalef(0.02, 0.02, 0.02);
		glBindTexture(GL_TEXTURE_2D, texNameArray[2]);
		glEnable(GL_TEXTURE_2D);
		ball();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

}

void triangle(float h, float s)
{
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5, 1.0); glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-s, -h, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(s, -h, 0.0);
	glEnd();
}

void skew(float h, float s, float t)
// skew face of the base
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-s, -h, -t / 2);
	glTexCoord2f(1.0, 0.0); glVertex3f(-s, -h, t / 2);
	glTexCoord2f(1.0, 1.0); glVertex3f(0, 0, t / 2);
	glTexCoord2f(0.0, 1.0); glVertex3f(0, 0, -t / 2);
	glEnd();
}

void base(float h, float s, float t)
// draw the metal silver base
{
	glPushMatrix();
	glTranslatef(0, 0, t / 2);
	triangle(h, s);
	glTranslatef(0, 0, -t);
	triangle(h, s);
	glPopMatrix();
	glPushMatrix();
	skew(h, s, t);
	glScalef(-1.0, 1.0, 1.0);
	skew(h, s, t);
	glPopMatrix();
}

void ground(float h)
// draw the ground
{
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-2, -h, -2);
	glTexCoord2f(1.0, 0.0); glVertex3f(-2, -h, 2);
	glTexCoord2f(1.0, 1.0); glVertex3f(2, -h, 2);
	glTexCoord2f(0.0, 1.0); glVertex3f(2, -h, -2);
	glEnd();
	glPopMatrix();
}

void theta_increase(void)
// parameter update during the animation
{
	theta = theta + 0.2;
	t1 = t1 + 0.4;

	if (theta > 360)
	{
		theta = theta - 350;
	}
	if (t1 > 360)
	{
		t1 = t1 - 350;
	}
	Sleep(8);
	glutPostRedisplay();
}

void light() 
// light settings
{
	glEnable(GL_LIGHT0);
	GLfloat Ambient[] = { 0.2f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
	GLfloat Diffuse[] = { 0.8f, 0.7f, 0.65f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
	GLfloat Specular[] = { 0.0f, 0.0f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
	GLfloat position[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	GLfloat spotDirection[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
	GLfloat light[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, light);
	GLfloat ambient_lightModel[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_lightModel);
	glEnable(GL_LIGHTING);
}


void branch(void)
// draw the 5 small system
{
	int i;
	quadratic = gluNewQuadric();
	glColor3d(0, 0, 0);
	
	for (i = 0; i < 10; i++)
	{
		glPushMatrix();
		glRotatef(theta + i*360/10, 0, 0, 1);
		glPushMatrix();
		glTranslatef(0.8 * HEIGHT, 0, 0);
		earthmoon();
		glPopMatrix();
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		gluCylinder(quadratic, 0.01, 0.01, HEIGHT * 0.8, 32, 32);
		glPopMatrix();
		glPopMatrix();
	}
}

void displayobject(void)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); // viewport is by default the display window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(viewport[2]) / viewport[3], 0.1, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1, 2, 3, 0, 0, 0, 0, 1, 0);
	glMultMatrixf(gsrc_getmo());  // get the rotation matrix from the rotation user-interface

  //
  //////////////////////////////////////////////////////////////////


	/*  Enable Z buffer method for visibility determination. */
	//  Z buffer code starts

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Z buffer code ends */

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);  // Clear display window.

	glBindTexture(GL_TEXTURE_2D, texNameArray[1]);
	glEnable(GL_TEXTURE_2D);
	base(HEIGHT, SS, THICK); // draw the base
	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texNameArray[0]);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	ground(HEIGHT);			// draw the ground
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	branch();				// draw the branch
	glPopMatrix();

	theta_increase();		// parameter update
	glutSwapBuffers();
}



void main(int argc, char** argv)
{

	calculatecircle();   // calculate data for the planar mesh


	glutInit(&argc, argv);                                      // Initialize GLUT
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);  // Set display mode.
	glutInitWindowPosition(WIN_POSX, WIN_POSY);                // Set display-window position at (WIN_POSX, WIN_POSY) 
																 // where (0, 0) is top left corner of monitor screen
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);		           // Set display-window width and height.
	glutCreateWindow("OpenGL Program for Quadrilateral Mesh"); // Create display window.

	//////////////////////////////////////////////////////////////////
	// 
	// Register mouse-click and mouse-move glut callback functions
	// for the rotation user-interface.
	// 
	glutMouseFunc(gsrc_mousebutton);
	glutMotionFunc(gsrc_mousemove);
	//
	//////////////////////////////////////////////////////////////////

	glutDisplayFunc(displayobject);	 // Send planar mesh to display window.

	glGenTextures(4, texNameArray);
	LoadGLTextures("wall.jpg", texNameArray);
	LoadGLTextures("metal.jpg", texNameArray + 1);
	LoadGLTextures("2k_sun.jpg", texNameArray + 2);
	LoadGLTextures("wood.jpg", texNameArray + 3);
	// read all the textures
	
	
	light();
	// light setting



	glutMainLoop();			               // Display everything and wait.
}