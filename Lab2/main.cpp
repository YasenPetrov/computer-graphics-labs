// CS3241Lab2.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <vector>
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

#define numStars 100
#define numPlanets 9

enum Mode
{
	Normal,
	Clock
};

int fps = 25;

void drawUnitDisk(GLfloat color[3], float alpha, bool half)
{
	int iterations = 100;
	glBegin(GL_POLYGON);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glColor4f(color[0], color[1], color[2], alpha);
	for (int i = 0; i < iterations; i++)
	{
		if ((i > iterations / 2 && half))
		{
			glColor4f(color[0] * 0.75, color[1] * 0.75, color[2] * 0.75, alpha);
		}
		float x = cos((M_PI / 2) + i * 2.0 * M_PI / iterations);
		float y = sin((M_PI / 2) + i * 2.0 * M_PI / iterations);
		glVertex2f(x, y);
	}

	glEnd();
}

class planet
{
	public:
		enum Type
		{
			Sun,
			HoursHand,
			MinutesHand,
			SecondsHand,
			PlainOlPlanet
		};
		Type type;
		bool active;
		float distFromRef;
		float angularSpeed;	// degrees per second
		GLfloat color[3];
		float size;			// 0-1, w.r.t. parent
		float angle;		// degrees
		vector<planet*> satellites;

		planet()
		{
			distFromRef = 0;
			angularSpeed = 0;
			color[0] = color[1] = color[2] = 0;
			size = 0;
			angle = 0;
			satellites = vector<planet*>();
			type = PlainOlPlanet;
			active = true;
		}

		planet(float _distFromRef, float _angularSpeed, GLfloat _color[3], float _size, float _angle, Type _type = PlainOlPlanet, bool _active = true):
			distFromRef(_distFromRef),
			angularSpeed(_angularSpeed),
			size(_size),
			angle(_angle),
			type(_type),
			active(_active)
		{
			color[0] = _color[0];
			color[1] = _color[1];
			color[2] = _color[2];
			satellites = vector<planet*>();
		}

		void addSatellite(planet* p)
		{
			satellites.push_back(p);
		}

		void draw()
		{
			glPushMatrix();

			glRotatef(angle, 0, 0, 1);
			glTranslatef(distFromRef, 0, 0);
			glScalef(size, size, size);
			
			float alpha = active ? 1 : 0.5;		// if the planet is not active, draw it half transparent
			drawUnitDisk(color, alpha, type!=Sun);

			for (vector<planet*>::iterator it = satellites.begin(); it != satellites.end(); it++)
			{
				(*it)->draw();
			}

			glPopMatrix();
		}

		void update()
		{
			if (type != PlainOlPlanet && type != Sun)
			{
				time_t current_time = time(NULL);
				struct tm * now = localtime(&current_time);

				switch (type)
				{
				case HoursHand:
					angle = 90 - (float)(now->tm_hour * 360) / 12;
					break;
				case MinutesHand:
					angle = 90 - (float)(now->tm_min * 360) / 60;
					break;
				case SecondsHand:
					angle = 90 - (float)(now->tm_sec * 360) / 60;
					break;
				default:
					break;
				}
			}
			else if (type == PlainOlPlanet)
			{
				if (active)
				{
					angle += angularSpeed / fps;
					if (angle > 360) angle -= 360;
				}
			}
			for (vector<planet*>::iterator it = satellites.begin(); it != satellites.end(); it++)
			{
				(*it)->update();
			}
		}

		void setActive(bool _active)
		{
			active = _active;
			for (vector<planet*>::iterator it = satellites.begin(); it != satellites.end(); it++)
			{
				(*it)->setActive(active);
			}
		}

		void toggleActive()
		{
			active = !active;
			for (vector<planet*>::iterator it = satellites.begin(); it != satellites.end(); it++)
			{
				(*it)->setActive(active);
			}
		}
};

float alpha = 0.0, k=0.5;
float tx = 0.0, ty=0.0;
Mode mode = Normal;
vector<planet*> plainOlPlanets;

GLfloat red[3] = { 1.0f, 0.3f, 0.3f };
GLfloat blue[3] = { 0.2f, 0.2f, 1.0f };
GLfloat green[3] = { 0.2f, 1.0f, 0.2f };
GLfloat yellow[3] = { 1.0f, 1.0f, 0.1f };

planet hour(3, 90, red, 0.5, 90);// , planet::Type::HoursHand);
planet minute(5, -30, green, 0.3, 90);// , planet::Type::MinutesHand);
planet second(7, 60, blue, 0.2, 45);// , planet::Type::SecondsHand);
planet sun(0, 0, yellow, 1, 0, planet::Type::Sun);

planet p1(3, 140, new GLfloat[3]{ 0.9f, 0.9f, 0.9f }, 0.5, 0);

planet p2(12, 30, new GLfloat[3]{ 0.5f, 0.4f, 0.9f }, 0.4, 15);
planet p3(10, -38, new GLfloat[3]{ 0.9f, 0.4f, 0.1f }, 0.8, 100);
planet p4(15, 56, new GLfloat[3]{ 0.6f, 1.0f, 0.3f }, 0.3, 140);

void initPlanets()
{	
	p3.addSatellite(new planet(4, -180, new GLfloat[3]{ 0.5f, 0.7f, 0.9f }, 0.6, 40));

	plainOlPlanets.push_back(&p2);
	plainOlPlanets.push_back(&p3);
	plainOlPlanets.push_back(&p4);

	sun.addSatellite(&p2);
	sun.addSatellite(&p3);
	sun.addSatellite(&p4);
	sun.addSatellite(&hour);
	sun.addSatellite(&minute);
	sun.addSatellite(&second);
	minute.addSatellite(&p1);
}

void toggleClock()
{
	if (mode == Clock)
	{
		mode = Normal;

		for (vector<planet*>::iterator it = plainOlPlanets.begin(); it != plainOlPlanets.end(); it++)
		{
			(*it)->setActive(true);
		}

		hour.type = planet::Type::PlainOlPlanet;
		minute.type = planet::Type::PlainOlPlanet;
		second.type = planet::Type::PlainOlPlanet;

	}
	else
	{
		mode = Clock;

		for (vector<planet*>::iterator it = plainOlPlanets.begin(); it != plainOlPlanets.end(); it++)
		{
			(*it)->setActive(false);
		}

		hour.type = planet::Type::HoursHand;
		minute.type = planet::Type::MinutesHand;
		second.type = planet::Type::SecondsHand;

	}
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
	glClearColor (0.0, 0.0, 0.3, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);

	//draw stuff here!

	sun.draw();

	glPopMatrix();
	glFlush ();
}

void idle()
{
	//update animation here

	_sleep(1000 / fps);
	sun.update();
	glutPostRedisplay();	//after updating, draw the screen again
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

		case 't':
			toggleClock();
		break;
		
		case 27:
			exit(0);
		break;

		default:
		break;
	}
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 2\n\n";
	cout<<"+++++CONTROL BUTTONS+++++++\n\n";
	cout<<"Scale Up/Down: Q/E\n";
	cout<<"Rotate Clockwise/Counter-clockwise: A/D\n";
	cout<<"Move Up/Down: W/S\n";
	cout<<"Move Left/Right: Z/C\n";
	cout <<"ESC: Quit\n";

	initPlanets();

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);	
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
