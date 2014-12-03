#include <math.h> 
#include <stdio.h> 
#define ESC 27

#include <iostream>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include<set>
#include<sstream>
#include<vector>
#include<stdlib.h>
#include<ctime>
#include "imageloader.h"
#include "pos.h"
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
using namespace std;



const int nummoto = 1;
const int NUM_FOSSILS = 100;
const float breadth = 100.0f;
float randgen() {
	return (float)rand() / ((float)RAND_MAX + 1);
}

//float x = 0.0, y = -5.0; // initially 5 units south of origin
int movflag = 0; // initially camera doesn't move
//float cube_x=0.0f,cube_y=0.0f,cube_z=0.0f;
//float lx = 0.0, ly = 1.0; // camera points initially along y-axis
//float angle = 0.0; // angle of rotation for the camera direction
//float deltaAngle = 0.0; // additional angle change when dragging
//bool rightturn=false,leftturn=false;
//float half=1.2f;
int flagv=0;
//int isDragging = 0; // true when dragging
//int xDragStart = 0; // records the x-coordinate when dragging starts
class Terrain {
	private:
		int tw; //Width
		int tl; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int tw2, int tl2) {
			tw = tw2;
			tl = tl2;

			hs = new float*[tl];
			for(int i = 0; i < tl; i++) {
				hs[i] = new float[tw];
			}

			normals = new Vec3f*[tl];
			for(int i = 0; i < tl; i++) {
				normals[i] = new Vec3f[tw];
			}

			computedNormals = false;
		}

		~Terrain() {
			for(int i = 0; i < tl; i++) {
				delete[] hs[i];
			}
			delete[] hs;

			for(int i = 0; i < tl; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}

		int width() {
			return tw;
		}

		int length() {
			return tl;
		}

		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}

		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}

		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}

			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[tl];
			for(int i = 0; i < tl; i++) {
				normals2[i] = new Vec3f[tw];
			}

			for(int z = 0; z < tl; z++) {
				for(int x = 0; x < tw; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);

					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < tl - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < tw - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}

					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < tl - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < tw - 1 && z < tl - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < tw - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}

					normals2[z][x] = sum;
				}
			}

			//Smooth out the normals
			const float F_RATIO = 0.5f;
			for(int z = 0; z < tl; z++) {
				for(int x = 0; x < tw; x++) {
					Vec3f sum = normals2[z][x];

					if (x > 0) {
						sum += normals2[z][x - 1] * F_RATIO;
					}
					if (x < tw - 1) {
						sum += normals2[z][x + 1] * F_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * F_RATIO;
					}
					if (z < tl - 1) {
						sum += normals2[z + 1][x] * F_RATIO;
					}

					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}

			for(int i = 0; i < tl; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;

			computedNormals = true;
		}

		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};

Terrain* bringterrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int i = 0; i < image->height; i++) {
		for(int j = 0; j < image->width; j++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (i * image->width + j)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(j, i, h);
		}
	}

	delete image;
	t->computeNormals();
	return t;
}

float findheight(Terrain* terrain, float x, float z) {
	//Make (x, z) lie within the bounds of the terrain
	if (x < 0) {
		x = 0;
	}
	else if (x > terrain->width() - 1) {
		x = terrain->width() - 1;
	}
	if (z < 0) {
		z = 0;
	}
	else if (z > terrain->length() - 1) {
		z = terrain->length() - 1;
	}

	//Compute the grid cell in which (x, z) lies and how close we are to the
	//left and outward edges
	int leftX = (int)x;
	if (leftX == terrain->width() - 1) {
		leftX--;
	}
	float fracX = x - leftX;

	int outZ = (int)z;
	if (outZ == terrain->width() - 1) {
		outZ--;
	}
	float fracZ = z - outZ;

	//Compute the four heights for the grid cell
	float h11 = terrain->getHeight(leftX, outZ);
	float h12 = terrain->getHeight(leftX, outZ + 1);
	float h21 = terrain->getHeight(leftX + 1, outZ);
	float h22 = terrain->getHeight(leftX + 1, outZ + 1);

	//Take a weighted average of the four heights
	return (1 - fracX) * ((1 - fracZ) * h11 + fracZ * h12) +
		fracX * ((1 - fracZ) * h21 + fracZ * h22);
}

class vehicle {
	private:
		//		MD2Model* model;
		Terrain* terrain;
		float terrainScale; //The scaling factor for the terrain
		float x0;
		float z0;
		float speed;
	public:
		float angle;
		float radius0; //The approximate radius of the guy
		vehicle(Terrain* terrain1,
				float terrainScale1) {
			terrain = terrain1;
			terrainScale = terrainScale1;
			radius0 = 0.4f * randgen() + 0.25f;
			x0 = randgen() *
				(terrainScale * (terrain->width() - 1));
			z0 = randgen() *
				(terrainScale * (terrain->length() - 1));
			speed =5.5f;
			angle=0;
		}


		void draw() {

			float scale = radius0 / 2.5f;
			glPushMatrix();
			glTranslatef(x0, 0.5f + y(), z0);
			glColor3f(1, 0, 0);
			glScalef(scale, scale, scale);
			glutSolidCube(4.0f);
			glPopMatrix();
		}
		void step() {
			float maxX = terrainScale * (terrain->width() - 1) - radius0;
			float maxZ = terrainScale * (terrain->length() - 1) - radius0;

			x0 +=0.1*speed*movflag*cos(DEG2RAD(angle));// velocityX() * GUY_STEP_TIME;
			z0 +=0.1*speed*movflag*sin(DEG2RAD(angle));// velocityZ() * GUY_STEP_TIME;
//			cout<<"x "<<x0<< " y "<<z0<<endl;
		}

		float x() {
			return x0;
		}

		float z() {
			return z0;
		}

		float y() {
			return terrainScale *
				findheight(terrain, x0 / terrainScale, z0 / terrainScale);
		}

		float walkAngle() {
			return angle;
		}	
};

vector<vehicle*> makevehicles(int numvehicles,  Terrain* terrain)
{
	vector<vehicle*> motorbike;
	for(int i = 0; i < numvehicles; i++)
		motorbike.push_back(new vehicle( terrain, breadth / (terrain->width() - 1)));
	return motorbike;
}


class Fossil
{
	private:
		//		MD2Model* model;
		Terrain* terrain;
		float terrainScale; //The scaling factor for the terrain
		float x0;
		float z0;
		float speed;
	public:
		float angle;
		float radius0; //The approximate radius of the guy
		Fossil(Terrain* terrain1,
				float terrainScale1) {
			terrain = terrain1;
			terrainScale = terrainScale1;
			radius0 = 0.4f * randgen() + 0.25f;
		
			x0=rand()%100;
			z0=rand()%100;
			speed =5.0f;
			angle=0;
		}


		void draw() {

			float scale = radius0 / 2.5f;
			glPushMatrix();
			glTranslatef(x0, 0.5f + y(), z0);
			glColor3f(1, 1, 1);
			glScalef(scale, scale, scale);
			glutSolidSphere(0.75, 20, 10);
			glPopMatrix();
		}
		void step() {
			float maxX = terrainScale * (terrain->width() - 1) - radius0;
			float maxZ = terrainScale * (terrain->length() - 1) - radius0;

			x0 +=0.1*speed*movflag*cos(DEG2RAD(angle));// velocityX() * GUY_STEP_TIME;
			z0 +=0.1*speed*movflag*sin(DEG2RAD(angle));// velocityZ() * GUY_STEP_TIME;
			cout<<"x "<<x0<< " y "<<z0<<endl;
		}

		float x() {
			return x0;
		}

		float z() {
			return z0;
		}

		float y() {
			return terrainScale *
				findheight(terrain, x0 / terrainScale, z0 / terrainScale);
		}

		float walkAngle() {
			return angle;
		}	


};

//vector<fossils*> fossilobjects;

vector<Fossil*> makefossils(int numfossils,  Terrain* terrain)
{
	vector<Fossil*> fossils;
	for(int i = 0; i < numfossils; i++)
		fossils.push_back(new Fossil( terrain, breadth / (terrain->width() - 1)));
	return fossils;
}

vector<Fossil*> _fossils;



void drawTerrain(Terrain* terrain) {
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < terrain->length() - 1; z++) {
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < terrain->width(); x++) {
			Vec3f normal = terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z), z);
			normal = terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
}

vector<vehicle*> _motorbike;
Terrain* _terrain;
float _angle = 0;


GLuint loadTexture(Image *image)
{
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	return textureId;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}





void cleanup() {
	//	delete _model;
	//	delete _motorbike[0];
	for(unsigned int i = 0; i < _motorbike.size(); i++)
		delete _motorbike[i];
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}


void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q') 
	{
		//cleanup;
		exit(0);
	}
	else if(key==49)
		flagv=1;
	else if(key==50)
		flagv=2;
	else if(key==51)
		flagv=3;
	else if(key==52)
		flagv=4;
	else if(key==53)
		flagv=5;
} 
void renderScene(void) 
{
	int i,j;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float scale = breadth / (_terrain->width() - 1);

	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat lightPos0[] = {-0.2f, 0.3f, -1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	if(flagv==4)
	{	
		gluLookAt(
		 _motorbike[0]->x()-(20*sin(DEG2RAD(_motorbike[0]->angle))), 2,_motorbike[0]->z()-(20*cos(DEG2RAD(_motorbike[0]->angle))),
				           _motorbike[0]->x(),   1 ,  _motorbike[0]->z(),
				             0.0,    1.0,    0.0
				             );
	}
	
	
	if(flagv==3)
	{

	  gluLookAt(
			               _motorbike[0]->x()  ,10, _motorbike[0]->z(),
			               _motorbike[0]->x()+sin(DEG2RAD(_motorbike[0]->angle)),1, _motorbike[0]->z()+cos(DEG2RAD(_motorbike[0]->angle)),
			               0.0,    1.0,    0.0
			              );






	}
	if(flagv==2)
	{
		gluLookAt(
				_motorbike[0]->x(), _motorbike[0]->y()+2.0f ,_motorbike[0]->z(),
				_motorbike[0]->x() + cos(DEG2RAD(_motorbike[0]->angle)), _motorbike[0]->y()+2.0f,_motorbike[0]->z() + sin(DEG2RAD(_motorbike[0]->angle)),            
				0.0,    1.0,    0.0);
	}

	if(flagv==1)
	{
		gluLookAt(
				_motorbike[0]->x()+1.5,  _motorbike[0]->y()+2.0f,    _motorbike[0]->z()+1.5,
				_motorbike[0]->x()+1.5 + cos(DEG2RAD(_motorbike[0]->angle)), _motorbike[0]->y()+2.1f,_motorbike[0]->z() + 1.5+ sin(DEG2RAD(_motorbike[0]->angle)),
				0.0,    1.0,    0.0);
	}

	if(flagv==5)
	{
		gluLookAt(
				_motorbike[0]->x()-(30*cos(DEG2RAD(_motorbike[0]->angle))), _motorbike[0]->y()+4.0f ,_motorbike[0]->z()-(30*sin(DEG2RAD(_motorbike[0]->angle))),
				_motorbike[0]->x(),   1 ,  _motorbike[0]->z(),
				0.0,    1.0,    0.0
			 );
	}
	if(flagv==0)
	{
		float scale = breadth / (_terrain->width() - 1);
		glTranslatef(0.0f,0.0f,0.0f);
		glTranslatef(0, 0, -1.0f * scale * (_terrain->length() - 1));
		glRotatef(30, 1, 0, 0);
		glRotatef(_angle, 0, 1, 0);
		glTranslatef(-breadth / 2, 0, -scale * (_terrain->length() - 1) / 2);
	}



	_motorbike[0]->draw();
	for(int i=0;i<_fossils.size();i++)
	_fossils[i]->draw();
	
	glScalef(scale, scale, scale);
	drawTerrain(_terrain);

	glutSwapBuffers();
} 
void update(int value) 
{
//	if(index<100);
//	{
//	f[index].x=rand()%140;
//	f[index++].y=rand()%140;
//	}
	_motorbike[0]->step();
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}



void pressSpecialKey(int key, int xx, int yy)
{
	switch (key) {
		case GLUT_KEY_UP : movflag = 1.0; break;
		case GLUT_KEY_DOWN : movflag =- 1.0; break;
		case GLUT_KEY_LEFT : _motorbike[0]->angle -= 3.5f; break;
		case GLUT_KEY_RIGHT :_motorbike[0]->angle += 3.5f; break;
	}
} 

void releaseSpecialKey(int key, int x, int y) 
{
	switch (key) {
		case GLUT_KEY_UP : movflag = 0.0; break;
		case GLUT_KEY_DOWN : movflag = 0.0; break;
				     //		case GLUT_KEY_LEFT : leftturn = false; break;
				     //		case GLUT_KEY_RIGHT : rightturn = false; break;
	}
} 
int main(int argc, char **argv) 
{
	srand((unsigned int)time(0));
/*	printf("\n\
			-----------------------------------------------------------------------\n\
			OpenGL Sample Program:\n\
			- Drag mouse left-right to rotate camera\n\
			- Hold up-arrow/down-arrow to move camera forward/backward\n\
			- q or ESC to quit\n\
			-----------------------------------------------------------------------\n");
*/
	// general initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	//	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("MOTOCROSS");
	initRendering();
	_terrain = bringterrain("map.bmp", 35.0f);
	_motorbike = makevehicles(nummoto, _terrain);
	_fossils = makefossils(NUM_FOSSILS, _terrain);
	float scaledTerrainLength = breadth / (_terrain->width() - 1) * (_terrain->length() - 1);
	// register callbacks
	glutDisplayFunc(renderScene); // (re)display callback
	glutReshapeFunc(handleResize); // window reshape callback
	glutKeyboardFunc(processNormalKeys); // process standard key clicks
	//	glutIdleFunc(update); // incremental update 
	//	glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
	//	glutMouseFunc(mouseButton); // process mouse button push/release
	//	glutMotionFunc(mouseMove); // process mouse dragging motion
	glutTimerFunc(25, update, 0);
	glutSpecialFunc(pressSpecialKey); // process special key pressed
	// Warning: Nonstandard function! Delete if desired.
	glutSpecialUpFunc(releaseSpecialKey); // process special key release

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0; // this is just to keep the compiler happy
}
