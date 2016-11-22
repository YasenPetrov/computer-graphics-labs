// CS3241Lab3.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#include "glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

// global variable

using namespace std;

bool g_tumble = false;
bool m_Smooth = false;
bool m_Highlight = false;
GLfloat angle = 0;   /* in degrees */
GLfloat angle2 = 0;   /* in degrees */
GLfloat zoom = 1.0;
GLfloat field_of_view = 40.0;
GLfloat x_translation = 0.0;
GLfloat aspect_ratio = 1.0;
GLfloat z_near = 1.0;
GLfloat z_far = 80.0;

GLfloat angle_def = 0;   /* in degrees */
GLfloat angle2_def = 0;   /* in degrees */
GLfloat zoom_def = 1.0;
GLfloat field_of_view_def = 40.0;
GLfloat x_translation_def = 0.0;
GLfloat aspect_ratio_def = 1.0;
GLfloat z_near_def = 1.0;
GLfloat z_far_def = 80.0;

int mouseButton = 0;
int moving, startx, starty;

#define NO_OBJECT 4;
int current_object = 0;

using namespace std;

void setupLighting()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// Lights, material properties
	GLfloat	ambientProperties[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat	diffuseProperties[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat	specularProperties[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { -100.0f, 100.0f, 100.0f, 1.0f };

	glClearDepth(1.0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularProperties);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

	// Default : lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

void drawSphere(double r, float red = 0.3, float green = 0.4, float blue = 0.7)
{

	float no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float mat_ambient[] = { red, green, blue, 0.5f };
	float mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
	float mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };
	float no_shininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

	if (m_Highlight)
	{
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_emission);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	}
	else {
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialf(GL_FRONT, GL_SHININESS, no_shininess);
	}


	int i, j;
	int n = 20;
	for (i = 0; i<n; i++)
		for (j = 0; j<2 * n; j++)
			if (m_Smooth)
			{
				glBegin(GL_POLYGON);

				// the normal of each vertex is actaully its own coordinates normalized for a sphere
				// your normal here
				glNormal3d(r*sin(i*M_PI / n)*cos(j*M_PI / n), r*cos(i*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos(j*M_PI / n), r*cos(i*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				// your normal here
				glNormal3d(r*sin((i + 1)*M_PI / n)*cos(j*M_PI / n), r*cos((i + 1)*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos(j*M_PI / n), r*cos((i + 1)*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				// your normal here
				glNormal3d(r*sin((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*cos((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*cos((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				// your normal here
				glNormal3d(r*sin(i*M_PI / n)*cos((j + 1)*M_PI / n), r*cos(i*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos((j + 1)*M_PI / n), r*cos(i*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glEnd();
			}
			else	{
				glBegin(GL_POLYGON);
				// Explanation: the normal of the whole polygon is the coordinate of the center of the polygon for a sphere
				glNormal3d(sin((i + 0.5)*M_PI / n)*cos((j + 0.5)*M_PI / n), cos((i + 0.5)*M_PI / n)*cos((j + 0.5)*M_PI / n), sin((j + 0.5)*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos(j*M_PI / n), r*cos(i*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos(j*M_PI / n), r*cos((i + 1)*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*cos((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos((j + 1)*M_PI / n), r*cos(i*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glEnd();
			}

}

double* calculateTorusNormal(double phi, double th)
{
	/*
		phi - outer angle
		th - inner angle
	*/
	/* tangent vector with respect to big circle */
	double tx = -sin(th);
	double ty = cos(th);
	double tz = 0;
	/* tangent vector with respect to little circle */
	double sx = cos(th)*(-sin(phi));
	double sy = sin(th)*(-sin(phi));
	double sz = cos(phi);
	/* normal is cross-product of tangents */
	double nx = ty*sz - tz*sy;
	double ny = tz*sx - tx*sz;
	double nz = tx*sy - ty*sx;

	double res[] = { nx, ny, nz };
	return res;

}

void drawTorus(double R, double r, float red = 0.5, float green = 0.7, float blue = 0.2)
{

	float no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float mat_ambient[] = { red, green, blue, 1.0f };
	float mat_diffuse[] = { 0.9f, 0.5f, 0.6f, 1.0f };
	float mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };
	float no_shininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

	if (m_Highlight)
	{
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_emission);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	}
	else {
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialf(GL_FRONT, GL_SHININESS, no_shininess);
	}


	int i, j;
	int n = 40;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			double th = j * 2 * M_PI / n;
			double th1 = (j + 0.5) * 2 * M_PI / n;
			double th2 = (j - 0.5) * 2 * M_PI / n;
			double phi = i * 2 * M_PI / n;
			double phi1 = (i + 0.5) * 2 * M_PI / n;
			double phi2 = (i - 0.5) * 2 * M_PI / n;

			glBegin(GL_POLYGON);
			if (m_Smooth)
			{
				glNormal3dv(calculateTorusNormal(th1, phi1));
				glVertex3d((R + r * cos(th1)) * cos(phi1), (R + r * cos(th1)) * sin(phi1), r * sin(th1));
				glNormal3dv(calculateTorusNormal(th2, phi1));
				glVertex3d((R + r * cos(th2)) * cos(phi1), (R + r * cos(th2)) * sin(phi1), r * sin(th2));
				glNormal3dv(calculateTorusNormal(th2, phi2));
				glVertex3d((R + r * cos(th2)) * cos(phi2), (R + r * cos(th2)) * sin(phi2), r * sin(th2));
				glNormal3dv(calculateTorusNormal(th1, phi2));
				glVertex3d((R + r * cos(th1)) * cos(phi2), (R + r * cos(th1)) * sin(phi2), r * sin(th1));
			}
			else
			{
				glNormal3dv(calculateTorusNormal(th, phi));

				glVertex3d((R + r * cos(th1)) * cos(phi1), (R + r * cos(th1)) * sin(phi1), r * sin(th1));
				glVertex3d((R + r * cos(th2)) * cos(phi1), (R + r * cos(th2)) * sin(phi1), r * sin(th2));
				glVertex3d((R + r * cos(th2)) * cos(phi2), (R + r * cos(th2)) * sin(phi2), r * sin(th2));
				glVertex3d((R + r * cos(th1)) * cos(phi2), (R + r * cos(th1)) * sin(phi2), r * sin(th1));
			}

			glEnd();
		}
	}

}

void drawRegularPolygon(int n, double r, bool facingUp)
{
	int i = 0;
	glBegin(GL_POLYGON);
	if (facingUp)
		glNormal3d(0, 0, 1);
	else
		glNormal3d(0, 0, -1);

	for (i = 0; i < n; i++)
	{
		double t = i * 2 * M_PI / n;
		double a = (i + 1) * 2 * M_PI / n;



		glVertex3d(r * cos(t), r * sin(t), 0);
		glVertex3d(r * cos(a), r * sin(a), 0);
	}
	glEnd();
}

void drawCilynder(double h, double r, bool drawBases = false, float red = 0.1, float green = 0.7, float blue = 0.3)
{
	float no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float mat_ambient[] = { red, green, blue, 1.0f };
	float mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
	float mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };
	float no_shininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

	if (m_Highlight)
	{
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_emission);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	}
	else {
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialf(GL_FRONT, GL_SHININESS, no_shininess);
	}

	int i;
	int n = 40;
	for (i = 0; i < n; i++)
	{
		double a = i * 2 * M_PI / n;
		double a1 = (i - 0.5) * 2 * M_PI / n;
		double a2 = (i + 0.5) * 2 * M_PI / n;

		glBegin(GL_POLYGON);
		if (m_Smooth)
		{
			glNormal3d(cos(a1), sin(a1), 0);
			glVertex3d(r * cos(a1), r * sin(a1), 0);

			glNormal3d(cos(a2),sin(a2), 0);
			glVertex3d(r * cos(a2), r * sin(a2), 0);

			glNormal3d(cos(a2),sin(a2), 0);
			glVertex3d(r * cos(a2), r * sin(a2), h);

			glNormal3d(cos(a1), sin(a1), 0);
			glVertex3d(r * cos(a1), r * sin(a1), h);
		}
		else
		{
				glNormal3d(cos(a), sin(a), 0);
				glVertex3d(r * cos(a1), r * sin(a1), h);
				glVertex3d(r * cos(a2), r * sin(a2), h);
				glVertex3d(r * cos(a2), r * sin(a2), 0);
				glVertex3d(r * cos(a1), r * sin(a1), 0);
		}
		glEnd();

	}

	if (drawBases)
	{
		drawRegularPolygon(n, r, false);
		glPushMatrix();
		glTranslated(0, 0, h);
		drawRegularPolygon(n, r, true);
		glPopMatrix();
	}
}

void drawCone(double h, double r, bool drawBase = false, float red = 0.8, float green = 0.1, float blue = 0.2)
{

	float no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float mat_ambient[] = { red, green, blue, 1.0f };
	float mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
	float mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };
	float no_shininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

	if (m_Highlight)
	{
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_emission);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	}
	else {
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialf(GL_FRONT, GL_SHININESS, no_shininess);
	}


	int i;
	int n = 30;
	for (i = 0; i < n; i++)
	{
		double t = i * 2 * M_PI / n;
		double a = (i + 1) * 2 * M_PI / n;
		double p = (i + 2) * 2 * M_PI / n;
		double s = (i - 1) * 2 * M_PI / n;


		glBegin(GL_POLYGON);
		if (m_Smooth)
		{
			glNormal3d(0, 0, 1);
			glVertex3d(0, 0, h);

			glNormal3d(r * h * (sin(a) - sin(s)), r * h * (cos(s) - cos(a)), r * r * (cos(s) * sin(a) - sin(s) * cos(a)));
			glVertex3d(r * cos(t), r * sin(t), 0);

			glNormal3d(r * h * (sin(p) - sin(t)), r * h * (cos(t) - cos(p)), r * r * (cos(t) * sin(p) - sin(t) * cos(p)));
			glVertex3d(r * cos(a), r * sin(a), 0);
		}
		else
		{
			glNormal3d(r * h * (sin(a) - sin(t)), r * h * (cos(t) - cos(a)), r * r * (cos(t) * sin(a) - sin(t) * cos(a)));
			glVertex3d(0, 0, h);
			glVertex3d(r * cos(t), r * sin(t), 0);

			glVertex3d(r * cos(a), r * sin(a), 0);
		}
		glEnd();
	}

	if (drawBase)
	{
		drawRegularPolygon(n, r, false);
	}

}

// The Mine
void drawMine(double r)
{
	glPushMatrix();
	drawSphere(r);
	int i, j;
	int n = 10;
	for(i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			double th = i * 2 * M_PI / n;
			double phi = i * 2 * M_PI / n;
			double th_d = i * 360.0f / n;
			double phi_d = j * 360.0f / n;
			glPushMatrix();
			glRotated(th_d, 0, 0, 1);
			glRotated(phi_d, cos(th), sin(th), 0);
			glTranslated(0, 0, r - r / 50);
			drawCone(r / 5, r / 10);
			glPopMatrix();
		}
	}
	glPopMatrix();
}

void drawBase()
{
	glPushMatrix();

	float r = 160 / 255.0f;
	float g = 120 / 255.0f;
	float b = 50 / 255.0f;

	drawCilynder(0.5, 5, true, r, g, b);
	glTranslated(0, 0, 0.5);
	drawCilynder(8, 0.5, false, r, g, b);
	glTranslated(0, 0, 8);
	drawCone(1, 0.5, true, r, g, b);
	glPopMatrix();
}

// Variables for the Tower of Hanoi
int g_frame = 0;
int g_max_frames = 50;
double ring_thickness = 0.75;
double t1_z0 = 8;
double t2_z0 = 10;
double t3_z0 = 12;
double step_size = - (t3_z0 - 3 * ring_thickness) / (g_max_frames - 5);


// Tower of Hanoi
void drawFrame(int f)
{
	//double s = f / double(g_max_frames);
	double t1_z = max(0, t1_z0 + f * step_size);
	double t2_z = max(1.5 * ring_thickness, t2_z0 + f * step_size);
	double t3_z = max(3 * ring_thickness, t3_z0 + f * step_size);

	glPushMatrix();
	drawBase();
	
	glPushMatrix();
	glTranslated(0, 0, 0.5 + ring_thickness);

	glPushMatrix();
	glTranslated(0, 0, t1_z);
	drawTorus(4, ring_thickness, 0.7, 0, 0);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, t2_z);
	drawTorus(3, ring_thickness, 0.7, 0.7, 0);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, t3_z);
	drawTorus(2, ring_thickness, 0, 0.7, 0);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void display(void)
{//Add Projection tool and Camera Movement somewhere here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(field_of_view, aspect_ratio, z_near, z_far);
	glMatrixMode(GL_MODELVIEW);
	int upVector = 1;
	gluLookAt(1, 1, 1, 1, 1, -1, 0, 1, 0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(0, 0, -6);
	glRotatef(angle2, 1.0, 0.0, 0.0);
	glRotatef(angle, 0.0, 1.0, 0.0);

	glScalef(zoom, zoom, zoom);
	
	switch (current_object) {
	case 0:
		drawSphere(1);
		break;
	case 1:
		drawTorus(1, 0.4);
		break;
	case 2:
		drawFrame(g_frame);
		break;
	case 3:
		drawMine(1);
		break;
	default:
		break;
	};
	glPopMatrix();
	glutSwapBuffers();
}


void resetCamera(){
	//fill in values below.
	zoom = zoom_def;
	angle = angle_def;
	angle2 = angle2_def;
	zoom = zoom_def;
	field_of_view = field_of_view_def;
	x_translation = x_translation_def;
	// include resetting of gluPerspective & gluLookAt - these are taken care of in display	
	return;
}

void setCameraBestAngle() {
	switch (current_object) {
	case 0:
		resetCamera();
		break;
	case 1:
		angle = -25;
		angle2 = -50;
		zoom = 1;
		field_of_view = 40;
		x_translation = 0;
		aspect_ratio = 1;
		z_near = 1;
		z_far = 80;
		break;
	case 2:
		angle = -5;
		angle2 = -54;
		zoom = 0.14;
		field_of_view = 40;
		x_translation = 0;
		aspect_ratio = 1;
		z_near = 1;
		z_far = 80;
		break;
	case 3:
		resetCamera();
		break;
	default:
		break;
	};
	return;
}

void keyboard(unsigned char key, int x, int y)
{//add additional commands here to change Field of View and movement
	switch (key) {
	case 'p':
	case 'P':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'w':
	case 'W':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'v':
	case 'V':
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 's':
	case 'S':
		m_Smooth = !m_Smooth;
		break;
	case 'h':
	case 'H':
		m_Highlight = !m_Highlight;
		break;

	case 'n':
		z_near -= 0.2;
		z_near = max(0, z_near);
		break;

	case 'N':
		z_near += 0.2;
		z_near = min(z_far, z_near);
		break;

	case 'f':
		z_far -= 0.5;
		z_far = max(z_near, z_far);
		break;

	case 'F':
		z_far += 0.5;
		break;

	case 'o':
		field_of_view -= 1.0;
		field_of_view = max(0, field_of_view);
		break;

	case 'O':
		field_of_view += 1.0;
		break;

	case 'r':
		resetCamera();
		break;

	case 'R':
		setCameraBestAngle();
		break;

	case 't':
		g_tumble = !g_tumble;
		break;

	case '1':
	case '2':
	case '3':
	case '4':
		current_object = key - '1';
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}

	glutPostRedisplay();
}


void idle()
{
	_sleep(40);
	g_frame += 1;
	if (g_frame == g_max_frames)
		g_frame = 0;
	if (g_tumble)
	{
		angle2 += 2;
		angle += 1;
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		mouseButton = button;
		moving = 1;
		startx = x;
		starty = y;
	}
	if (state == GLUT_UP) {
		mouseButton = button;
		moving = 0;
	}
}

void motion(int x, int y)
{
	if (moving) {
		if (mouseButton == GLUT_LEFT_BUTTON)
		{
			angle = angle + (x - startx);
			angle2 = angle2 + (y - starty);
		}
		else zoom += ((y - starty)*0.01);
		startx = x;
		starty = y;
		glutPostRedisplay();
	}

}


int main(int argc, char **argv)
{
	cout << "CS3241 Lab 3" << endl << endl;

	cout << "1-4: Draw different objects" << endl;
	cout << "S: Toggle Smooth Shading" << endl;
	cout << "H: Toggle Highlight" << endl;
	cout << "W: Draw Wireframe" << endl;
	cout << "P: Draw Polygon" << endl;
	cout << "V: Draw Vertices" << endl;
	cout << "n, N: Reduce or increase the distance of the near plane from the camera" << endl;
	cout << "f, F: Reduce or increase the distance of the far plane from the camera" << endl;
	cout << "o, O: Reduce or increase the distance of the povy plane from the camera" << endl;
	cout << "r: Reset camera to the initial parameters when the program starts" << endl;
	cout << "R: Change camera to another setting that is has the best viewing angle for your object" << endl;
	cout << "t: Toggle tumble mode" << endl;
	cout << "ESC: Quit" << endl << endl;


	cout << "Left mouse click and drag: rotate the object" << endl;
	cout << "Right mouse click and drag: zooming" << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("CS3241 Assignment 3");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(display);
	glMatrixMode(GL_PROJECTION);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	setupLighting();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glutMainLoop();

	return 0;
}