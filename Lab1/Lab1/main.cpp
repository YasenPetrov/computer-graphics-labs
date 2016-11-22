// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

/* Include header files depending on platform */
#ifdef _WIN32
	#include "glut.h"
	#define M_PI 3.14159
#elif __APPLE__
	#include <OpenGL/gl.h>
	#include <GLUT/GLUT.h>
#endif

using namespace std;

enum Mode
{
	SierpinskiAnimation		= 1,
	SierpinskiInteractive	= 2,
	RecursiveCircles		= 3
};

float alpha = 0.0, k=1;
float tx = 0.0, ty=0.0;


// Global
bool	g_blackBackground = false;
Mode	g_mode;
float	g_mainColor[3] = {0.9, 0.1, 0.3};

// Sierpinski triangle variables
int		s_sierpinskiIterations = 0;
int		s_sierpinskiCounter = 0;
bool	s_interactive = false;

// Recursive circles variables
int		rc_branchFactor = 2;
float	rc_scaleFactor = 0.5;
float	rc_scaleFactorIncrementSize = 0.02;
int		rc_iterations = 7;
float	rc_angleOffset = 0.0;
float	rc_angleOffsetIncrementSize = M_PI / 60;
bool	rc_random = false;

int randInt(int min, int max)
{
	if (max == min) return max;
	return min + (rand() % (max - min));
}

float randFloat(float min, float max)
{
	if (max == min) return max;
	return min + (max - min) * ((float)rand()) / ((float)RAND_MAX);
}

void drawEquilateralTriangle()
{
	glBegin(GL_POLYGON);

	if (g_blackBackground)
		glColor3f(0, 0, 0);
	else
		glColor3f(1, 1, 1);

	glVertex2f(-1, 0);
	glVertex2f(1, 0);
	glVertex2f(0, -sqrt(3));
	glEnd();
}

void drawCircle(float r, float g, float b)
{
	int iterations = 100;
	glBegin(GL_LINE_LOOP);

	r = g_mainColor[0];
	g = g_mainColor[1];
	b = g_mainColor[2];

	glColor3f(r, g, b);
	for (int i = 0; i < iterations; i++)
	{
		float x = cos(i * 2.0 * M_PI / iterations);
		float y = sin(i * 2.0 * M_PI / iterations);
		glVertex2f(x, y);
	}
	glEnd(); 
}

void recursiveCircles(int n)
{
	if (n == 0) return;

	float scale = 0.5, offsetAngle = 0;
	int branchFactor = 2;

	if (rc_random)
	{
		branchFactor = randInt(2, 8);
		offsetAngle = randFloat(0.0, 2 * M_PI / (float)branchFactor);
		scale = randFloat(0.5, 0.7);
	}
	else
	{
		scale = rc_scaleFactor;
		branchFactor = rc_branchFactor;

		offsetAngle = rc_angleOffset * (n - rc_iterations);
	}

	glPushMatrix();
	
	drawCircle(0.7, 0.7 , 0.9);

	for (int i = 0; i < branchFactor; i++, offsetAngle += 2 * M_PI / (float)branchFactor)
	{
		glTranslatef(cos(offsetAngle), sin(offsetAngle), 0);
		glScalef(scale, scale, scale);
		recursiveCircles(n - 1);

		glPopMatrix();
		glPushMatrix();
	}

	glPopMatrix();
}

// Generate a random color
void generateColorScheme()
{
	for (int i = 0; i < 3; i++)
	{
		g_mainColor[i] = randFloat(0, 1);
	}
}

void drawSierpinskiTriangleHelper(int n)
{
	if (n == 0)
	{
		return;
	}

	glPushMatrix();


	// Draw a triangle that is the same color as the background, repeat
	// in all three corners of the triangle
	drawEquilateralTriangle();

	glTranslatef(0, sqrt(3) / 2, 0);
	glScalef(1.0 / 2, 1.0 / 2, 1.0 / 2);
	drawSierpinskiTriangleHelper(n - 1);

	glPopMatrix();
	glPushMatrix();

	glTranslatef(1, -sqrt(3) / 2, 0);
	glScalef(1.0 / 2, 1.0 / 2, 1.0 / 2);
	drawSierpinskiTriangleHelper(n - 1);

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-1, -sqrt(3) / 2, 0);
	glScalef(1.0 / 2, 1.0 / 2, 1.0 / 2);
	drawSierpinskiTriangleHelper(n - 1);

	glPopMatrix();

}


void drawSierpinskiTriangle(int n)
{
	glBegin(GL_POLYGON);

	float r = g_mainColor[0];
	float g = g_mainColor[1];
	float b = g_mainColor[2];

	// Draw the coloured triangle and call the helper
	// method to 'remove' triangles recursively
	glColor3f(r, g, b);
	glVertex2f(0, sqrt(3));
	glVertex2f(2, -sqrt(3));
	glColor3f(1 - r, 1 - g, 1 - b);
	glVertex2f(-2, -sqrt(3));
	glEnd();

	drawSierpinskiTriangleHelper(n);
}

void displaySierpinskiFrame()
{
	glPushMatrix();

	if (s_sierpinskiCounter++ > 8) s_sierpinskiCounter = 0;
	drawSierpinskiTriangle(s_sierpinskiCounter);

	glPopMatrix();
}

void displaySierpinskiInteractive(void)
{
	glPushMatrix();

	drawSierpinskiTriangle(s_sierpinskiIterations);

	glPopMatrix();
	glFlush();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);
	
	glScalef(5, 5, 5);
	//draw your stuff here
	switch (g_mode)
	{
		case SierpinskiAnimation: 
			displaySierpinskiFrame();
			break;
		case SierpinskiInteractive:
			displaySierpinskiInteractive();
			break;
		case RecursiveCircles: 
			recursiveCircles(rc_iterations);
			break;
		default:
			break;
	}

	glPopMatrix();
	glFlush ();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init(void)
{
	glClearColor (1, 1, 1, 1.0);
	glShadeModel (GL_SMOOTH);
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

		case 'a':
			alpha+=10;
			glutPostRedisplay();
		break;

		case 'd':
			alpha-=10;
			glutPostRedisplay();
		break;

		case 'q':
			k+=0.1;
			glutPostRedisplay();
		break;

		case 'e':
			if(k>0.1)
				k-=0.1;
			glutPostRedisplay();
		break;

		case 'z':
			tx-=0.1;
			glutPostRedisplay();
		break;

		case 'c':
			tx+=0.1;
			glutPostRedisplay();
		break;

		case 's':
			ty-=0.1;
			glutPostRedisplay();
		break;

		case 'w':
			ty+=0.1;
			glutPostRedisplay();
		break;

		case 'o':
			s_sierpinskiIterations = max(0, --s_sierpinskiIterations);
			glutPostRedisplay();
			break;

		case 'p':
			s_sierpinskiIterations++;
			glutPostRedisplay();
			break;

		case 'k':
			rc_branchFactor = max(1, --rc_branchFactor);
			glutPostRedisplay();
			break;

		case 'l':
			rc_branchFactor++;
			glutPostRedisplay();
			break;

		case 'n':
			rc_scaleFactor -= rc_scaleFactorIncrementSize;
			if (rc_scaleFactor < rc_scaleFactorIncrementSize) rc_scaleFactor = rc_scaleFactorIncrementSize;
			glutPostRedisplay();
			break;

		case 'm':
			rc_scaleFactor += rc_scaleFactorIncrementSize;
			glutPostRedisplay();
			break;

		case 'h':
			rc_iterations = max(1, --rc_iterations);
			glutPostRedisplay();
			break;

		case 'j':
			rc_iterations++;
			glutPostRedisplay();
			break;

		case 'r':
			rc_random = !rc_random;
			glutPostRedisplay();
			break;

		case 'b':
			if (g_blackBackground)
			{
				glClearColor(1, 1, 1, 1);
				g_blackBackground = false;
			}
			else
			{
				glClearColor(0, 0, 0, 1);
				g_blackBackground = true;
			}
			glutPostRedisplay();
			break;

		case 'v':
			generateColorScheme();
			glutPostRedisplay();
			break;

		case 'u':
			rc_angleOffset -= rc_angleOffsetIncrementSize;
			glutPostRedisplay();
			break;

		case 'i':
			rc_angleOffset += rc_angleOffsetIncrementSize;
			glutPostRedisplay();
			break;

		case 27:
			exit(0);
		break;

		default:
		break;
	}
}

void idle()
{
	_sleep(250);
	glutPostRedisplay();
}

Mode menu()
{
	int m_choice = 0;
	int s_choice = 0;

	cout << "CS3241 Lab 1\n\n";
	cout << "What would you like to draw?\n\n";
	cout << "1. Sierpinski triangle\n";
	cout << "2. Recursive circles\n";

	while (m_choice != 1 && m_choice != 2)
	{
		cout << "\nYour choice: ";
		cin >> m_choice;
	}

	switch (m_choice)
	{
		case 1:
			cout << "\n\n\n\n\n\n\n";
			cout << "Mkay... now do you want to:\n";
			cout << "1. See an animation of a Sierpinski triangle\n";
			cout << "2. See an interactive version of a Sierpinski triangle\n";

			while (s_choice != 1 && s_choice != 2)
			{
				cout << "\nYour choice: ";
				cin >> s_choice;
			}

			switch (s_choice)
			{
				case 1: return SierpinskiAnimation;
				case 2: return SierpinskiInteractive;
				default: break;
			}
		case 2: return RecursiveCircles;
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	// Parse command line arguments or display menu
	if (argc > 1 && atoi(argv[1]) > 0 && atoi(argv[1]) < 4)
		g_mode = (Mode)atoi(argv[1]);
	else
		g_mode = menu();

	cout << "\n\n\n\n\n\n\n\n\n";
	cout<<"CS3241 Lab 1\n\n";
	cout<<"+++++CONTROL BUTTONS+++++++\n\n";
	cout<<"Scale Up/Down: Q/E\n";
	cout<<"Rotate Clockwise/Counter-clockwise: A/D\n";
	cout<<"Move Up/Down: W/S\n";
	cout<<"Move Left/Right: Z/C\n\n";
	cout << "Toggle white/black background:		B\n";
	cout << "-/+ Sierpinski triangle iterations:	O/P\n\n";
	cout << "-/+ Recursive circles branch factor:	K/L\n";
	cout << "-/+ Recursive circles scale factor:	N/M\n";
	cout << "-/+ Recursive circles iterations:	H/J\n";
	cout << "-/+ recursive circles angle offset:	U/I\n";
	cout << "Toggle recursive circles randomness:	R\n";
	cout <<"ESC: Quit\n";

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	
	// If we want an animated triangle, we register an idle funciton
	if (g_mode == SierpinskiAnimation) glutIdleFunc(idle);
	glutDisplayFunc(display);

	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
