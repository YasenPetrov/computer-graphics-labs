// CS3241Lab5.cpp 
#include <cmath>
#include <iostream>
#include "GL\glut.h"
#include "vector3D.h"
#include <chrono>

using namespace std;

#define WINWIDTH 600
#define WINHEIGHT 400
#define NUM_OBJECTS 4
#define MAX_RT_LEVEL 50
#define NUM_SCENE 2

float* pixelBuffer = new float[WINWIDTH * WINHEIGHT * 3];

class Ray { // a ray that start with "start" and going in the direction "dir"
public:
	Vector3 start, dir;
};

class RtObject {

public:
	virtual double intersectWithRay(Ray, Vector3& pos, Vector3& normal) = 0; // return a -ve if there is no intersection. Otherwise, return the smallest postive value of t

	// Materials Properties
	double ambiantReflection[3] ;
	double diffusetReflection[3] ;
	double specularReflection[3] ;
	double speN = 300;
	double alpha = 1;
	double nu = 1;
};

class Sphere : public RtObject {

	Vector3 center_;
	double r_;
public:
	Sphere(Vector3 c, double r) { center_ = c; r_ = r; };
	Sphere() {};
	void set(Vector3 c, double r) { center_ = c; r_ = r; };
	double intersectWithRay(Ray, Vector3& pos, Vector3& normal);
};

RtObject **objList; // The list of all objects in the scene

// Global Variables
// Camera Settings
Vector3 cameraPos(0,0,-500);

// assume the the following two vectors are normalised
Vector3 lookAtDir(0,0,1);
Vector3 upVector(0,1,0);
Vector3 leftVector(1, 0, 0);
float focalLen = 500;

// Light Settings
Vector3 lightPos(900,1000,-1500);
double ambiantLight[3] = { 0.4,0.4,0.4 };
double diffuseLight[3] = { 0.7,0.7, 0.7 };
double specularLight[3] = { 0.5,0.5, 0.5 };


double bgColor[3] = { 0.1,0.1,0.4 };

int sceneNo = 0;

bool solveQuadraticEqn(double a, double b, double c, double& x1, double& x2)
// Return false if no solution
{
	// Compute determinant
	double D = b * b - 4 * a * c;

	// Check if we have a solution
	if (D < 0)
	{
		x1 = NULL;
		x2 = NULL;
		return false;
	}
	else
	{
		x1 = (-b - sqrt(D)) / (2 * a);
		x2 = (-b + sqrt(D)) / (2 * a);
		return true;
	}
}

Vector3 computeRefractedRayDirection(Vector3 ray, Vector3 normal, double nu_i, double nu_t)
{
	double nu_it = nu_i / nu_t;
	
	double cos_i = dot_prod(normal, -ray);
	double sin_i = sqrt(1 - pow(cos_i, 2));

	// Apply Snell's law
	double sin_t = sin_i * nu_it;
	double cos_t = sqrt(1 - pow(sin_t, 2));

	// The formula from Lecture 11, slide 20
	Vector3 t = ray * nu_it + normal * (dot_prod(normal, -ray) * nu_it - cos_t);
	return t;
}

double Sphere::intersectWithRay(Ray ray, Vector3& intersection, Vector3& normal)
// return a -ve if there is no intersection. Otherwise, return the smallest postive value of t
{

	// Step 1
	// If the ray is given by r + Dt and the sphere is centered at R and has radius q
	// we want to solve at^2 + bt + c = 0, where a = D dot D
	// b = 2D dot (r - R), c = R dot R + r dot r - 2r dot R - q^2

	// Get params
	Vector3 D = ray.dir;
	Vector3 r = ray.start;
	Vector3 R = center_;
	double q = r_;

	// Normalize D
	D.normalize();

	// Compute coefficients
	double a = dot_prod(D, D);
	double b = dot_prod(D * 2, r - R);
	double c = dot_prod(R, R) + dot_prod(r, r) - dot_prod(r * 2, R) - q * q;

	double t1, t2;
	bool hasSolution = solveQuadraticEqn(a, b, c, t1, t2);
	if (hasSolution && t1 != t2 && (t1 > 0 || t2 > 0)) // We have two unique solutions, at least one is positive
	{
		// Get the min positive solution
		double minPos = t1;
		if (t1 < 0 || (t2 > 0 && t2 < t1))
			minPos = t2;

		// Set out params values
		intersection = r + D * minPos;
		normal = intersection - R;
		normal.normalize();

		return minPos;
	}
	else // No solution to the quadratic eqn - no intersection
	{
		return -1;
	}
}

void rayTrace(Ray ray, double& r, double& g, double& b, int fromObj = -1 ,int level = 0)
{
	// Step 4
	// Check if we have exceeded the recursion limit
	if (level > MAX_RT_LEVEL)
	{
		r = 0;
		g = 0;
		b = 0;
		return;
	}

	int goBackGround = 1, i = 0;

	Vector3 intersection, normal;
	Vector3 lightV;
	Vector3 viewV;
	Vector3 lightReflectionV;
	Vector3 rayReflectionV;

	Ray newRay;
	double mint = DBL_MAX, t;
	int mintObjIndex = -1;

	// Find the closest point we hit and the object it's on
	for (i = 0; i < NUM_OBJECTS; i++) // Step 2
	{
		if (i != fromObj) // We don't want to reflect off the current object
		{
			Vector3 currentIntersection, currentNormal;
			t = objList[i]->intersectWithRay(ray, currentIntersection, currentNormal);
			if (t > 0 && t < mint) // Remove the latter check when implementing transparency
			{
				mintObjIndex = i;
				goBackGround = 0; // We've hit an object, we won't draw the background
				mint = t; // This is the new closest position
				intersection = currentIntersection;
				normal = currentNormal;
			}
		}
	}

	if (goBackGround)
	{
		r = bgColor[0];
		g = bgColor[1];
		b = bgColor[2];
	}
	else
	{
		// Step 3
		RtObject *closestObj = objList[mintObjIndex];
		// Get the light vector for the diffuce and specular terms
		lightV = lightPos - intersection;
		lightV.normalize();
		// Compute the reflection vector for the specular term
		lightReflectionV = normal * 2 * dot_prod(normal, lightV) - lightV;
		lightReflectionV.normalize();
		// Compute the view vector for the specular term - it's the negation of the ray
		viewV = Vector3(0.0, 0.0, 0.0) - ray.dir;
		viewV.normalize();
		// Compute the new ray - it is the reflection of view vector
		newRay.dir = normal * 2 * dot_prod(normal, viewV) - viewV;
		newRay.start = intersection;


		// Check for shadows
		Ray shadowRay;
		shadowRay.start = intersection;
		shadowRay.dir = lightV;
		bool hasShadow = false;
		for (int k = 0; k < NUM_OBJECTS; k++)
		{
			if (k != mintObjIndex) // No shadow cast object on itself
			{
				Vector3 _, __;
				double tr = objList[k]->intersectWithRay(shadowRay, _, __);
				if (tr > 0) // Something is casting a shadow on us
				{
					hasShadow = true;
					break;
				}
			}
		}


		// Compute the coefficient for the diffuse term
		double f_d = 1.0;
		// Compute the angle between R and V - for the speculat term
		double alpha = acos(dot_prod(lightReflectionV, viewV));
		// Compute the coefficient for the specular term
		double f_c = 1.0;

		// Compute the values for each color
		double finalColor[3] = { 0.0, 0.0, 0.0 };
		for (int j = 0; j < 3; j++)
		{
			// Ambient
			finalColor[j] += ambiantLight[j] * closestObj->ambiantReflection[j];
			if (!hasShadow) {
				// Diffuse
				finalColor[j] += f_d * diffuseLight[j] * closestObj->diffusetReflection[j] * dot_prod(lightV, normal);
				// Specular
				finalColor[j] += f_c * specularLight[j] * closestObj->specularReflection[j] * pow(dot_prod(lightReflectionV, viewV), closestObj->speN);
			}
		}
		r = finalColor[0];
		g = finalColor[1];
		b = finalColor[2];

		// Compute the reflections and add them to the current color
		// Use the specular weights for reflection weights
		double r_refl, g_refl, b_refl;
		rayTrace(newRay, r_refl, g_refl, b_refl, mintObjIndex, level + 1);
		r += closestObj->specularReflection[0] * r_refl;
		g += closestObj->specularReflection[1] * g_refl;
		b += closestObj->specularReflection[2] * b_refl;

		double obj_alpha = closestObj->alpha;
		if (obj_alpha < 1)
		{
			// Compute the refractions
			double r_refr, g_refr, b_refr;
			Ray refr_ray;
			refr_ray.start = intersection;
			refr_ray.dir = computeRefractedRayDirection(-viewV, normal, 1.0, closestObj->nu);
			rayTrace(refr_ray, r_refr, g_refr, b_refr, mintObjIndex, level + 1);
			r = obj_alpha * r + r_refr * (1 - obj_alpha);
			g = obj_alpha * g + g_refr * (1 - obj_alpha);
			b = obj_alpha * b + b_refr * (1 - obj_alpha);
		}
	}

}


void drawInPixelBuffer(int x, int y, double r, double g, double b)
{
	pixelBuffer[(y*WINWIDTH + x) * 3] = (float)r;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 1] = (float)g;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 2] = (float)b;
}

void renderScene()
{
	int x, y;
	Ray ray;
	double r, g, b;

	cout << "Rendering Scene " << sceneNo << " with resolution " << WINWIDTH << "x" << WINHEIGHT << "........... ";
	__int64 time1 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the starting time

	ray.start = cameraPos;

	Vector3 vpCenter = cameraPos + lookAtDir * focalLen;  // viewplane center
	Vector3 startingPt = vpCenter + leftVector * (-WINWIDTH / 2.0) + upVector * (-WINHEIGHT / 2.0);
	Vector3 currPt;

	for(x=0;x<WINWIDTH;x++)
		for (y = 0; y < WINHEIGHT; y++)
		{
			currPt = startingPt + leftVector*x + upVector*y;
			ray.dir = currPt-cameraPos;
			ray.dir.normalize();
			rayTrace(ray, r, g, b);
			drawInPixelBuffer(x, y, r, g, b);
		}

	__int64 time2 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the ending time

	cout << "Done! \nRendering time = " << time2 - time1 << "ms" << endl << endl;
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_DOUBLEBUFFER);
	glDrawPixels(WINWIDTH, WINHEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
	glutSwapBuffers();
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


void setScene(int i = 0)
{
	if (i > NUM_SCENE)
	{
		cout << "Warning: Invalid Scene Number" << endl;
		return;
	}

	if (i == 0)
	{

		((Sphere*)objList[0])->set(Vector3(-130, 80, 120), 100);
		((Sphere*)objList[1])->set(Vector3(130, -80, -80), 100);
		((Sphere*)objList[2])->set(Vector3(-130, -80, -80), 100);
		((Sphere*)objList[3])->set(Vector3(130, 80, 120), 100);

		objList[0]->ambiantReflection[0] = 0.1;
		objList[0]->ambiantReflection[1] = 0.4;
		objList[0]->ambiantReflection[2] = 0.4;
		objList[0]->diffusetReflection[0] = 0;
		objList[0]->diffusetReflection[1] = 1;
		objList[0]->diffusetReflection[2] = 1;
		objList[0]->specularReflection[0] = 0.2;
		objList[0]->specularReflection[1] = 0.4;
		objList[0]->specularReflection[2] = 0.4;
		objList[0]->speN = 300;
		objList[0]->alpha = 1;


		objList[1]->ambiantReflection[0] = 0.6;
		objList[1]->ambiantReflection[1] = 0.6;
		objList[1]->ambiantReflection[2] = 0.2;
		objList[1]->diffusetReflection[0] = 1;
		objList[1]->diffusetReflection[1] = 1;
		objList[1]->diffusetReflection[2] = 0;
		objList[1]->specularReflection[0] = 0.0;
		objList[1]->specularReflection[1] = 0.0;
		objList[1]->specularReflection[2] = 0.0;
		objList[1]->speN = 50;
		objList[1]->alpha = 1;


		objList[2]->ambiantReflection[0] = 0.1;
		objList[2]->ambiantReflection[1] = 0.6;
		objList[2]->ambiantReflection[2] = 0.1;
		objList[2]->diffusetReflection[0] = 0.1;
		objList[2]->diffusetReflection[1] = 1;
		objList[2]->diffusetReflection[2] = 0.1;
		objList[2]->specularReflection[0] = 0.3;
		objList[2]->specularReflection[1] = 0.7;
		objList[2]->specularReflection[2] = 0.3;
		objList[2]->speN = 650;
		objList[2]->alpha = 1;


		objList[3]->ambiantReflection[0] = 0.3;
		objList[3]->ambiantReflection[1] = 0.3;
		objList[3]->ambiantReflection[2] = 0.3;
		objList[3]->diffusetReflection[0] = 0.7;
		objList[3]->diffusetReflection[1] = 0.7;
		objList[3]->diffusetReflection[2] = 0.7;
		objList[3]->specularReflection[0] = 0.6;
		objList[3]->specularReflection[1] = 0.6;
		objList[3]->specularReflection[2] = 0.6;
		objList[3]->speN = 650;
		objList[3]->alpha = 1;


	}
	if (i == 1)
	{

		((Sphere*)objList[0])->set(Vector3(-130, -10, -80), 100);
		((Sphere*)objList[1])->set(Vector3(0, 100, 0), 40);
		((Sphere*)objList[2])->set(Vector3(-130, 80, 120), 100);
		((Sphere*)objList[3])->set(Vector3(130, 10, 10), 100);

		objList[0]->ambiantReflection[0] = 0.4;
		objList[0]->ambiantReflection[1] = 0.4;
		objList[0]->ambiantReflection[2] = 0.4;
		objList[0]->diffusetReflection[0] = 0.4;
		objList[0]->diffusetReflection[1] = 0.4;
		objList[0]->diffusetReflection[2] = 0.2;
		objList[0]->specularReflection[0] = 1;
		objList[0]->specularReflection[1] = 1;
		objList[0]->specularReflection[2] = 1;
		objList[0]->speN = 300;
		objList[0]->alpha = 0.2;
		objList[0]->nu = 1.05;

		objList[1]->ambiantReflection[0] = 0.2;
		objList[1]->ambiantReflection[1] = 0.7;
		objList[1]->ambiantReflection[2] = 0.2;
		objList[1]->diffusetReflection[0] = 0.5;
		objList[1]->diffusetReflection[1] = 0.7;
		objList[1]->diffusetReflection[2] = 0;
		objList[1]->specularReflection[0] = 0.6;
		objList[1]->specularReflection[1] = 0.6;
		objList[1]->specularReflection[2] = 0.6;
		objList[1]->speN = 50;
		objList[2]->alpha = 1;

		objList[2]->ambiantReflection[0] = 0.4;
		objList[2]->ambiantReflection[1] = 0.4;
		objList[2]->ambiantReflection[2] = 0.4;
		objList[2]->diffusetReflection[0] = 0.4;
		objList[2]->diffusetReflection[1] = 0.4;
		objList[2]->diffusetReflection[2] = 0.4;
		objList[2]->specularReflection[0] = 1;
		objList[2]->specularReflection[1] = 1;
		objList[2]->specularReflection[2] = 1;
		objList[2]->speN = 300;
		objList[2]->alpha = 1;

		objList[3]->ambiantReflection[0] = 0.8;
		objList[3]->ambiantReflection[1] = 0.3;
		objList[3]->ambiantReflection[2] = 0.3;
		objList[3]->diffusetReflection[0] = 0.8;
		objList[3]->diffusetReflection[1] = 0.7;
		objList[3]->diffusetReflection[2] = 0.7;
		objList[3]->specularReflection[0] = 0.4;
		objList[3]->specularReflection[1] = 0.3;
		objList[3]->specularReflection[2] = 0.7;
		objList[3]->speN = 650;
		objList[3]->alpha = 1;


	}
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {
	case 's':
	case 'S':
		sceneNo = (sceneNo + 1 ) % NUM_SCENE;
		setScene(sceneNo);
		renderScene();
		glutPostRedisplay();
		break;
	case 'q':
	case 'Q':
		exit(0);

		default:
		break;
	}
}

int main(int argc, char **argv)
{

	
	cout<<"<<CS3241 Lab 5>>\n\n"<<endl;
	cout << "S to go to next scene" << endl;
	cout << "Q to quit"<<endl;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (WINWIDTH, WINHEIGHT);

	glutCreateWindow ("CS3241 Lab 5: Ray Tracing");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboard);

	objList = new RtObject*[NUM_OBJECTS];

	// create four spheres
	objList[0] = new Sphere(Vector3(-130, 80, 120), 100);
	objList[1] = new Sphere(Vector3(130, -80, -80), 100);
	objList[2] = new Sphere(Vector3(-130, -80, -80), 100);
	objList[3] = new Sphere(Vector3(130, 80, 120), 100);

	setScene(0);

	setScene(sceneNo);
	renderScene();

	glutMainLoop();

	for (int i = 0; i < NUM_OBJECTS; i++)
		delete objList[i];
	delete[] objList;

	delete[] pixelBuffer;

	return 0;
}
