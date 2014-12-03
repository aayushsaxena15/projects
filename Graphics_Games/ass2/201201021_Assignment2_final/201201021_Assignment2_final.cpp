#include <stdlib.h>
#include<math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<time.h>
#define ESC 27
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define RAD2DEG(rad) (rad * 180 / PI)
#include "imageloader.h"
#include "vec3f.h"

#include <iostream>
using namespace std;
int score=0;
char text[10]="\0";
char displaytime[10]="\0";
char over[25]="\0";
int rotationdirection,brakeflag=0,jumpflag=0,flagv=0;
float hprev=-1,previous_pitch=0,acceleration=0,retardation=0,vel=0,yaw=0,pitch=0; //angle to increase height
float roll=0;//tilt
float centre_x=0,centre_z=0,centre_y=0.5;
float deltaMove = 0.0; // initially camera doesn't move
float lx = 0.0, ly = 1.0; // camera points initially along y-axis
float angle = 0.0; // angle of rotation for the camera direction
float deltaRotate=0.0;
float avg_h,height1,height2;
//vector < pair<int,int> >fossil;
typedef struct fossils
{
	float first,second,third;
	int taken;
}fossils;
fossils fossil[10000];
int fossilindex=0;
float fossildim=0.5f;
float tyre_angle=0;
void draw_bike()
{
	glColor3f( 0.556863,0.137255,0.419608); 
	glPushMatrix();
	glTranslatef(centre_x,0,centre_z);
	glRotatef(yaw,0,1,0);
	glRotatef(-pitch,1,0,0);
	glRotatef(roll,0,0,1);
	glTranslatef(0,centre_y+0.04,0);//to keep it till ground
	glPushMatrix();
	glTranslatef(0.0,0.0,-0.12);
	glutSolidCube(0.35);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0,0.0,0.12);
	glutSolidCube(0.35);
	glTranslatef(0.0,0.0,0.12);

	//headlight
	glPushMatrix();
	glColor3f(1.0,1.0,0.0); 
	glTranslatef(0.0,0.175,0.0);
	glutSolidCone(0.05,0.2,20,20);
	glPopMatrix();

	//handle
	glPushMatrix();
	glColor3f(1.0,0.0,0.0); 
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(0.12,0.12,0.0);
	glVertex3f(0.375,0.375,0.0);
	glVertex3f(0.375,0.375,0.0);
	glVertex3f(0.5,0.0,-0.5);
	glVertex3f(-0.12,0.12,0.0);
	glVertex3f(-0.375,0.375,0.0);
	glVertex3f(-0.375,0.375,0.0);
	glVertex3f(-0.5,0.0,-0.5);
	glEnd();
	glPopMatrix();

	glPopMatrix();


	tyre_angle+=vel/0.02;

	//back wheel
	glPushMatrix();
	glColor3f(0.0,0.0,0.0); 
	glTranslatef(0.0,-0.12,-0.375);
	glRotatef(-90,1,0,0);
	glRotatef(90,0,1,0);
	glRotatef(RAD2DEG(tyre_angle),0,0,1);//wheel rotation
	glutWireTorus(0.06,0.12,10,10);
	glPopMatrix();

	//front wheel
	glPushMatrix();
	glColor3f(0.0,0.0,0.0);
	glTranslatef(0.0,-0.12,0.375);
	glRotatef(-90,1,0,0);
	glRotatef(90,0,1,0);
	glRotatef(RAD2DEG(tyre_angle),0,0,1);//wheel rotation
	glutWireTorus(0.06,0.12,10,10);
	glPopMatrix();

	glPopMatrix();
}
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;

			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}

			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}

			computedNormals = false;
		}

		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;

			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}

		int width() {
			return w;
		}

		int length() {
			return l;
		}

		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}

		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			if(z>=0 && x>=0)
				return hs[z][x];
			else
				return 0;
		}

		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}

			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}

			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);

					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < w - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}

					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}

					normals2[z][x] = sum;
				}
			}

			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];

					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}

					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}

			for(int i = 0; i < l; i++) {
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

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	int  span=2;
	Terrain* t = new Terrain(image->width*span, image->height*span);
	for(int y = 0; y < image->height*span; y++) {
		for(int x = 0; x < image->width*span; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * ((y%image->height) * image->width + x%image->width)];
			float h = height * ((color / 255.0f) - 0.5f);
			if(h<0)
				t->setHeight(x, y, 0);
			else
				t->setHeight(x, y, h);

		}
	}

	delete image;
	t->computeNormals();
	return t;
}

float _angle = 0.0f;
Terrain* _terrain;
int winflag=0;
void cleanup() {
	delete _terrain;
}

//Returns the approximate height of the terrain at the specified (x, z) position
float heightAt(Terrain* terrain, float x, float z) {
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



float distance(float a,float b,float c)
{
	return sqrt((a*a)+(b*b)+(c*c));
}

int timer=5000;

int overflag=0;
void drawScene() {


	glClearColor(0.0,0.2,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vec3f tempnormal = _terrain->getNormal(centre_x, centre_z);
	if(overflag==1||winflag==1)
	{
		glClearColor(0.0,0.2,1.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if(winflag==1)
			sprintf(over,"%s","YOU WIN--- YOUR SCORE ");
		else if(overflag==1)
			sprintf(over,"%s","GAME OVER--- YOUR SCORE ");
		glColor3f(1.0f, 0.0f, 1.0f);
		glRasterPos3f(-1.0f, 0.0f,-5.0f);
		for(int i=0;i<25;i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, over[i]);


		cout <<"time"<<timer<<endl;
		cout <<" over "<<overflag;
		glPushMatrix();
		sprintf(text,"%d",score);
		glColor3f(0.0f, 1.0f, 0.0f);
		glRasterPos3f(1.2f, 0.0f,-5.0f);
		for(int i=0;i<10;i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
		glPopMatrix();
		glutSwapBuffers();
	}
	else
	{
		glPushMatrix();
		sprintf(text,"%d",score);
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(0.0f, 1.7f,-5.0f);
		for(int i=0;i<10;i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
		glPopMatrix();

		glPushMatrix();
		sprintf(displaytime,"%d",timer);
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(1.0f, 1.7f,-5.0f);
		for(int i=0;i<10;i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, displaytime[i]);
		glPopMatrix();

		glTranslatef(0.0f, 0.0f, 0.0f);
		if(jumpflag!=1)
		{
			float temp,tempb,tempc,tempd;
			tempb=sinf(DEG2RAD(yaw));
			tempc=0;//sinf(DEG2RAD(pitch));
			tempd=cosf(DEG2RAD(yaw));
			temp=((tempnormal[0]*tempb+tempnormal[2]*tempd+tempnormal[1]*tempc)/(distance(tempnormal[0],tempnormal[1],tempnormal[2])*distance(tempb,tempc,tempd)));
			temp=RAD2DEG(acos(temp));
			pitch=-(90-temp);
		}
		centre_y=height1+0.25;
		if(flagv==0)
		{
			gluLookAt(
					centre_x-(2*sin(DEG2RAD(yaw))), centre_y+1,centre_z-(2*cos(DEG2RAD(yaw))),
					centre_x,      centre_y+1,      centre_z,
					0.0,    1.0,    0.0
				 );
		}
		else if(flagv==1)
		{
			gluLookAt(
					centre_x,      centre_y+0.25,      centre_z,
					centre_x+2*sinf(DEG2RAD(yaw)), centre_y+0.25,centre_z+2*cosf(DEG2RAD(yaw)),
					0.0,    1.0,    0.0
				 );
		}
		else if(flagv==2)
		{
			gluLookAt(
					centre_x,      centre_y+10,      centre_z,
					centre_x+sin(DEG2RAD(yaw)), centre_y,centre_z+cos(DEG2RAD(yaw)),
					0.0,    1.0,    0.0
				 );
		}
		else if(flagv==3)
		{
			gluLookAt(
					centre_x-3,     centre_y,centre_z,
					centre_x, centre_y+0.25, centre_z,
					0.0,    1.0,    0.0
				 );
		}
		else if(flagv==4)
		{
			gluLookAt(
					centre_x-(3.5*sin(DEG2RAD(yaw))), centre_y+1,centre_z-(3.5*cos(DEG2RAD(yaw))),
					centre_x,      centre_y+1,      centre_z,
					0.0,    1.0,    0.0
				 );
		}
		GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
		GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
		GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
		glColor3f(0.3f, 0.9f, 0.0f);
		for(int z = 0; z < _terrain->length() - 1; z++) {
			//Makes OpenGL draw a triangle at every three consecutive vertices
			glBegin(GL_TRIANGLE_STRIP);
			for(int x = 0; x <  _terrain->width(); x++) {
				if(z>0.8* _terrain->width()&&z< _terrain->width())
				{
					int temp=rand()%50;
					if(temp<15)
					{
						glColor3f(0.0f,0.0f,1.0f);
					}
					else
						glColor3f(0.0f,0.0f,0.5f);
				}


				Vec3f normal = _terrain->getNormal(x, z);
				glNormal3f(normal[0], normal[1], normal[2]);
				glVertex3f(x, _terrain->getHeight(x, z), z);
				normal = _terrain->getNormal(x, z + 1);
				glNormal3f(normal[0], normal[1], normal[2]);
				glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
			}
			glEnd();
		} 
		if(brakeflag==1)
		{
			retardation=0;
			vel=0;
			acceleration=0;
		}
		if(vel>=0 || pitch!=0) 
		{
			centre_x+=vel*sinf(DEG2RAD(yaw));
			centre_z+=vel*cosf(DEG2RAD(yaw));
		}
		else if(vel<0)
		{
			vel=0;
			acceleration=0;
			retardation=0;
		}
		if(centre_z<0)
			centre_z=0;
		if(centre_x<0)
			centre_x=0;
		if(centre_z>96)
			centre_z=96;
		if(centre_x>119)
			centre_x=119;
		height1=heightAt(_terrain,centre_x,centre_z);
		height2=height1;
		if(jumpflag==1)
			height1=hprev-0.1;
		if(hprev-(height2)>0.2)
			jumpflag=1;
		else if(height1<0)
			jumpflag=0;
		hprev=height1;
		draw_bike();
		for(int k=0;k<50;k++)
		{
			glPushMatrix();
			fossil[k].third=heightAt(_terrain,fossil[k].first,fossil[k].second);
			if(fossil[k].taken==0)
			{
				glTranslatef(fossil[k].first,fossil[k].third,fossil[k].second);
				glColor3f(1.0f, 1.0f, 1.0f);
				glutSolidCube(fossildim);
			}
			glPopMatrix();
		}
		glutSwapBuffers();
	}
}
void update() 
{
	if(timer!=0)
	{
		if(score==50)
			winflag=1;
		timer-=1;
		if(jumpflag==1)
		{
			glutPostRedisplay(); // redisplay everything
			return;
		}
		if(previous_pitch<0 && pitch==0)
		{
			retardation=-0.003;
		}
		previous_pitch=pitch;
		if(vel>0)
			vel=vel+acceleration+retardation-(0.003*sinf(DEG2RAD(pitch)));
		else 
		{
			retardation=0;
			vel=vel+acceleration-(0.002*sinf(DEG2RAD(pitch)));
		}
		if(deltaRotate==-1)
			yaw+=3;
		else if(deltaRotate==1)
			yaw-=3;
		if( -45< (roll+(5*rotationdirection)) && roll+(5*rotationdirection)<45)
			roll=roll+(5*rotationdirection);
		if(rotationdirection)
			if(vel!=0 && vel>0.05)
			{
				yaw+=-0.7*tanf(DEG2RAD(roll))/vel;

			}
		for(int i=0;i<50;i++)
		{

		//	if((abs(fossil[i].first-centre_x<0.3))&&(abs(centre_y-fossil[i].third<0.3))&&(abs(fossil[i].second-centre_z<0.1))&&fossil[i].taken==0)
			if(sqrt(((fossil[i].first-centre_x)*(fossil[i].first-centre_x))+((centre_y-fossil[i].third)*(centre_y-fossil[i].third))+((fossil[i].second-centre_z)*(fossil[i].second-centre_z)))<0.5&&(fossil[i].taken==0))
			{
				fossil[i].taken=1;
				score++;
			}

		}
		glutPostRedisplay(); 
	}
	else if (timer==0)
	{
		cout <<"time"<<timer;
		overflag=1;
		glutPostRedisplay();
	}
}

void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC)
	{
		cleanup();
		exit(0);
	}
	if(key==49)
	{
		flagv=0;
	}
	else if(key==50)
	{
		flagv=1;
	}
	else if(key==51)
	{
		flagv=2;
	}
	if(key==52)
	{
		flagv=3;
	}
	if(key==53)
	{
		flagv=4;
	}
} 
void keyboardmove(unsigned char key, int x, int y) 
{
	if (key=='a') 
		deltaRotate = 0.0; 
	else if(key== 'd')
		deltaRotate = 0.0; 

}  
void pressSpecialKey(int key, int xx, int yy)
{
	switch (key) {
		case GLUT_KEY_UP : 
			deltaMove = 1.0;
			acceleration=0.004;
			break;
		case GLUT_KEY_DOWN : 
			deltaMove = -1.0;
			brakeflag=1;
			break;
		case GLUT_KEY_RIGHT :
			rotationdirection=1;
			break;
		case GLUT_KEY_LEFT : 
			rotationdirection=-1;
			break;
	}
} 

void releaseSpecialKey(int key, int x, int y) 
{
	switch (key) {
		case GLUT_KEY_UP : 
			deltaMove = 0.0; 
			retardation=-0.004;
			acceleration=0;
			break;
		case GLUT_KEY_DOWN : 
			brakeflag=0;
			deltaMove = 0.0; 
			acceleration=0;
			break;
		case GLUT_KEY_LEFT :
			deltaRotate = 0.0; 
			roll=0;
			rotationdirection=0;
			break;
		case GLUT_KEY_RIGHT : 
			deltaRotate = 0.0; 
			roll=0;
			rotationdirection=0;
			break;
	}
} 
void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1300, 700);

	glutCreateWindow("Motocross");
	initRendering();

	_terrain = loadTerrain("heightmap.bmp", 30);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(handleResize);
	glutIdleFunc(update); // incremental update 
	glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
	glutKeyboardFunc(processNormalKeys); // process standard key clicks
	glutSpecialFunc(pressSpecialKey); // process special key pressed
	glutKeyboardUpFunc(keyboardmove); 
	// Warning: Nonstandard function! Delete if desired.
	glutSpecialUpFunc(releaseSpecialKey); // process special key release
	srand(time(NULL));
	/*for(int i =0; i<50;i++)
	  {
	  fossil[i].first=rand()%119 + 1;
	  fossil[i].second=rand()%119 + 1;
	  fossil[i].taken=0;
	  }*/
	int i =0;
	while(i<50)
	{
		fossil[i].first=rand()%119 + 1;
		fossil[i].second=rand()%96 + 1;
		fossil[i].taken=0;
		i++;
	}
	glutMainLoop();
	return 0;
}
