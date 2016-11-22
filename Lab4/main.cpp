// CS3241Lab4.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <iostream>
#include <fstream>
#include <math.h>

/* Include header files depending on platform */
#ifdef _WIN32
	#include "glut.h"
	#define M_PI 3.14159
#elif __APPLE__
	#include <OpenGL/gl.h>
	#include <GLUT/GLUT.h>
#endif

#define MAXPTNO 1000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 8

using namespace std;

// Global variables that you can use
struct Point {
	int x,y;
};

// Storage of control points
int nPt = 0;
Point ptList[MAXPTNO];
Point c1_ptList[MAXPTNO];

// Display options
bool displayControlPoints = true;
bool displayControlLines = true;
bool displayTangentVectors = false;
bool displayObjects = false;
bool C1Continuity = false;

// Animation settings
int frameNumber = 0;
bool animationActive = false;

void drawLine(int n)
{
	glBegin(GL_LINES);
	if (n % 2 == 0)
		glColor3f(0.0, 0.0, 1.0);
	else
		glColor3f(0.0, 1.0, 0.0);
	glVertex2f(0, 0);
	if (n % 2 == 0)
		glColor3f(0.0, 1.0, 0.0);
	else
		glColor3f(0.0, 0.0, 1.0);
	glVertex2f(0, 0.2);
	glEnd();
}

void drawFractalTree(int n)
{
	drawLine(n);
	if (n == 0) return;

	glPushMatrix();

	glTranslatef(0, 0.2, 0);
	glPushMatrix();

	glRotatef(-30, 0, 0, 1);
	glScalef(0.7, 0.7, 0.7);
	drawFractalTree(n - 1);

	glPopMatrix();
	glPushMatrix();

	glRotatef(60, 0, 0, 1);
	glScalef(0.5, 0.5, 0.5);
	drawFractalTree(n - 1);

	glPopMatrix();

	glPopMatrix();
}

void drawFractalTreeN()
{
	drawFractalTree(6);
}

void drawRightArrow()
{
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
		glVertex2f(0,0);
		glVertex2f(100,0);
		glVertex2f(95,5);
		glVertex2f(100,0);
		glVertex2f(95,-5);
	glEnd();
}

int fact(int n)
{
	int res = 1;
	for (int i = 1; i <= n; i++)
	{
		res *= i;
	}
	return res;
}

int choose(int n, int k)
// returns n choose k
{
	return fact(n) / (fact(k) * fact(n - k));
}

void drawBezierCurves(Point pts[])
{
	glColor3f(0, 0, 1);
	for (int ptIndex = 3; ptIndex < nPt; ptIndex += 3)
	{
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= NLINESEGMENT; i++)
		{
			float t = float(i) / NLINESEGMENT;
			float x = 0;
			float y = 0;
			for (int j = 0; j <= 3; j++)
			{
				x += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].x;
				y += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].y;
			}
			glVertex2f(x, y);
		}
		glEnd();
	}
}

void drawTangentVectors(Point pts[])
{
	glColor3f(0, 1, 1);
	for (int ptIndex = 3; ptIndex < nPt; ptIndex += 3)
	{
		for (int i = 0; i <= NOBJECTONCURVE; i++)
		{
			float t = float(i) / NOBJECTONCURVE;
			float x = 0;
			float y = 0;
			// coords of the tangent
			float x_t = 0;
			float y_t = 0;
			for (int j = 0; j <= 3; j++)
			{
				x += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].x;
				y += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].y;
				x_t += choose(3, j) * (pow(t, j) * (3 - j) * pow((1 - t), (2 - j)) -
					pow((1 - t), (3 - j)) * j * pow(t, (j - 1))) * pts[ptIndex - j].x;
				y_t += choose(3, j) * (pow(t, j) * (3 - j) * pow((1 - t), (2 - j)) -
					pow((1 - t), (3 - j)) * j * pow(t, (j - 1))) * pts[ptIndex - j].y;
			}
			float angle = atan2(y_t, x_t); //angle in radians
			glPushMatrix();
			glTranslatef(x, y, 0);
			glRotatef(angle * 180 / M_PI, 0, 0, 1);
			drawRightArrow();
			glPopMatrix();
		}
	}
}

void drawObjects(Point pts[], void (*drawingFunc)())
{
	glColor3f(0, 1, 1);
	for (int ptIndex = 3; ptIndex < nPt; ptIndex += 3)
	{
		for (int i = 0; i <= NOBJECTONCURVE; i++)
		{
			float t = float(i) / NOBJECTONCURVE;
			float x = 0;
			float y = 0;
			// coords of the tangent
			float x_t = 0;
			float y_t = 0;
			for (int j = 0; j <= 3; j++)
			{
				x += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].x;
				y += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].y;
				x_t += choose(3, j) * (pow(t, j) * (3 - j) * pow((1 - t), (2 - j)) -
					pow((1 - t), (3 - j)) * j * pow(t, (j - 1))) * pts[ptIndex - j].x;
				y_t += choose(3, j) * (pow(t, j) * (3 - j) * pow((1 - t), (2 - j)) -
					pow((1 - t), (3 - j)) * j * pow(t, (j - 1))) * pts[ptIndex - j].y;
			}
			float angle = atan2(y_t, x_t); //angle in radians
			glPushMatrix();
			glTranslatef(x, y, 0);
			glRotatef(angle * 180 / M_PI, 0, 0, 1);
			glScalef(100, 100, 100);
			drawingFunc();
			glPopMatrix();
		}
	}
}

void drawFrame(int frame, Point pts[], void(*drawingFunc)())
{
	cout << frame << endl;
	for (int ptIndex = 3; ptIndex < nPt; ptIndex += 3)
	{
		for (int i = 0; i <= NOBJECTONCURVE; i++)
		{
			int curve = ptIndex / 3 - 1; //0-based
			int f = curve * (NOBJECTONCURVE - 1) + i;
			if (f == frame)
			{
				float t = float(i) / NOBJECTONCURVE;
				float x = 0;
				float y = 0;
				// coords of the tangent
				float x_t = 0;
				float y_t = 0;
				for (int j = 0; j <= 3; j++)
				{
					x += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].x;
					y += choose(3, j) * pow(t, j) * pow((1 - t), (3 - j)) * pts[ptIndex - j].y;
					x_t += choose(3, j) * (pow(t, j) * (3 - j) * pow((1 - t), (2 - j)) -
						pow((1 - t), (3 - j)) * j * pow(t, (j - 1))) * pts[ptIndex - j].x;
					y_t += choose(3, j) * (pow(t, j) * (3 - j) * pow((1 - t), (2 - j)) -
						pow((1 - t), (3 - j)) * j * pow(t, (j - 1))) * pts[ptIndex - j].y;
				}
				float angle = atan2(y_t, x_t); //angle in radians
				glPushMatrix();
				glTranslatef(x, y, 0);
				glRotatef(angle * 180 / M_PI, 0, 0, 1);
				glScalef(50, 50, 50);
				drawingFunc();
				glPopMatrix();
				break;
			}
		}
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	if(displayControlPoints)
	{

		glPointSize(5);
		glBegin(GL_POINTS);
		for(int i=0;i<nPt; i++)
		{
				glColor3f(0,0,0);
				if (C1Continuity)
				{
					if (i > 3 && i % 3 == 1)
					{
						glColor3f(0.7, 0.7, 0.7);
						glVertex2d(ptList[i].x, ptList[i].y);
						glColor3f(1, 0, 0);
					}
					glVertex2d(c1_ptList[i].x, c1_ptList[i].y);
				}
				else
				{
					glVertex2d(ptList[i].x, ptList[i].y);
				}
		}
		glEnd();
		glPointSize(1);

	}

	if(displayControlLines)
	{
		glColor3f(0,1,0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i<nPt; i++)
		{
			if (C1Continuity)
			{
				glVertex2d(c1_ptList[i].x, c1_ptList[i].y);
			}
			else
			{
				glVertex2d(ptList[i].x, ptList[i].y);
			}
		}
		glEnd();
	}

	if (C1Continuity)
		drawBezierCurves(c1_ptList);
	else
		drawBezierCurves(ptList);

	if (displayTangentVectors)
	{
		if (C1Continuity)
			drawTangentVectors(c1_ptList);
		else
			drawTangentVectors(ptList);
	}

	if (displayObjects)
	{
		if (!animationActive)
		{
			if (C1Continuity)
				drawObjects(c1_ptList, drawFractalTreeN);
			else
				drawObjects(ptList, drawFractalTreeN);
		}
		else
		{
			if (C1Continuity)
				drawFrame(frameNumber, c1_ptList, drawFractalTreeN);
			else
				drawFrame(frameNumber, ptList, drawFractalTreeN);
		}
	}

	glPopMatrix();
	glutSwapBuffers ();
}

void idle()
{
	if (animationActive)
	{
		_sleep(50);
		frameNumber++;
		if (frameNumber > ((nPt - 1) / 3) * (NOBJECTONCURVE - 1) + 1)
			frameNumber = 0;
	}
	glutPostRedisplay();
}	

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);  
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void init(void)
{
	glClearColor (1.0,1.0,1.0, 1.0);
}

void readFile()
{

	std::ifstream file;
    file.open("savefile.txt");
	file >> nPt;

	if(nPt>MAXPTNO)
	{
		cout << "Error: File contains more than the maximum number of points." << endl;
		nPt = MAXPTNO;
	}

	for(int i=0;i<nPt;i++)
	{
		file >> ptList[i].x;
		file >> ptList[i].y;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void writeFile()
{
	std::ofstream file;
    file.open("savefile.txt");
    file << nPt << endl;

	for(int i=0;i<nPt;i++)
	{
		file << ptList[i].x << " ";
		file << ptList[i].y << endl;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
		case 'r':
		case 'R':
			readFile();
		break;

		case 'w':
		case 'W':
			writeFile();
		break;

		case 'T':
		case 't':
			displayTangentVectors = !displayTangentVectors;
		break;

		case 'o':
		case 'O':
			displayObjects = !displayObjects;
			if (!displayObjects)
				animationActive = false;
		break;

		case 'p':
		case 'P':
			displayControlPoints = !displayControlPoints;
		break;

		case 'L':
		case 'l':
			displayControlLines = !displayControlLines;
		break;

		case 'C':
		case 'c':
			C1Continuity = !C1Continuity;
		break;

		case 'e':
		case 'E':
			nPt = 0;
		break;

		case 'a':
		case 'A':
			if(displayObjects)
				animationActive = !animationActive;
			break;

		case 27:
			exit(0);
		break;

		default:
		break;
	}

	glutPostRedisplay();
}



void mouse(int button, int state, int x, int y)
{
	/*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
	/*state: GLUT_UP or GLUT_DOWN */
	enum
	{
		MOUSE_LEFT_BUTTON = 0,
		MOUSE_MIDDLE_BUTTON = 1,
		MOUSE_RIGHT_BUTTON = 2,
		MOUSE_SCROLL_UP = 3,
		MOUSE_SCROLL_DOWN = 4
	};
	if((button == MOUSE_LEFT_BUTTON)&&(state == GLUT_UP))
	{
		if(nPt==MAXPTNO)
		{
			cout << "Error: Exceeded the maximum number of points." << endl;
			return;
		}
		ptList[nPt].x=x;
		ptList[nPt].y=y;
		// Add points to c1 array here
		if (nPt > 3 && nPt % 3 == 1)
		{
			// Notice that for C1 cont. between curves a and b we need
			// b_2 = 2 * a_4 - a_3
			c1_ptList[nPt].x = 2 * ptList[nPt - 1].x - ptList[nPt - 2].x;
			c1_ptList[nPt].y = 2 * ptList[nPt - 1].y - ptList[nPt - 2].y;
		} 
		else 
		{
			c1_ptList[nPt].x = x;
			c1_ptList[nPt].y = y;
		}
		nPt++;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 4"<< endl<< endl;
	cout << "Left mouse click: Add a control point"<<endl;
	cout << "ESC: Quit" <<endl;
	cout << "P: Toggle displaying control points" <<endl;
	cout << "L: Toggle displaying control lines" <<endl;
	cout << "E: Erase all points (Clear)" << endl;
	cout << "C: Toggle C1 continuity" <<endl;	
	cout << "T: Toggle displaying tangent vectors" <<endl;
	cout << "O: Toggle displaying objects" <<endl;
	cout << "A: Toggle object animation" << endl;
	cout << "R: Read in control points from \"savefile.txt\"" <<endl;
	cout << "W: Write control points to \"savefile.txt\"" <<endl;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("CS3241 Assignment 4");
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}
