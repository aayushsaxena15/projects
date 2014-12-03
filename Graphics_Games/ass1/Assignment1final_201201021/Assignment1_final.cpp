#include <iostream>
#include <stdio.h>
#include <cmath>
#include <GL/glut.h>
#include <time.h>
#define BUFSIZE 512
using namespace std;
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
void drawcanon();
float p1_x,p1_y,p2_x,p2_y;
float theta = 0.0f;
int paused=0;
int overflag=0,rotcan=0;
//float box_len = 4.0f;
float box_len;
float rightm,leftm;
float greenrightm,greenleftm;
float redrightm,redleftm;
float red_x;
float red_y;
int selc=0,selg=0,selr=0;
float green_x;
float green_y;
float canon_x;
float canon_y;
int flagc=0,flagr=0,flagg=0;

float minvel=0.003f,maxvel=0.009f;
float v;
int difficulty;

void actualcod(int x, int y);

int score=0;
char text[10]="\0";
char over[10]="\0";

float l=0.25f;
int index=0;
int flagbeam=0;float rad=DEG2RAD(theta);
time_t prev=time(NULL);
float spider_x,spider_y;
//float spidervel=0.003f;
float spidervel;
void drawScene();
void update(int value);
void drawBox(float len);
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int status, int x, int y);
void dragwithmouse(int x,int y);
float mouseactual_x,mouseactual_y;

typedef struct ref
{
	float x,y,angle,l;
	int fin,started;
}ref;
ref rays[10000];
int refind=0;

typedef struct spiders
{
	float x,y,vel;
	int c,cond,in,kill,score;
}spiders;
spiders s[1000000];


typedef struct laser
{
	float x,y,angle,l;
	int fin,reflected;
}laser;
laser beams[1000000];
int beamind=0;
void update2(int value)
{
	if(paused==0&&overflag==0)
	{
		float a,d;
		int mad1=(box_len-0.6)*1000;	
		int mad2=(box_len/2-0.3)*1000;	

		a=(float)((rand()%mad1-mad2))/1000;
		s[index].x=a;
		s[index].y=(box_len/2)-0.2;
		int max=(int)(maxvel*1000);
		int min=(int)(minvel*1000);
		//	cout<<max<<min<<endl;
		float newvel=(float)((rand()%max-min))/1000;
	//	s[index].vel=spidervel;
		s[index].vel=((rand()%5+6)/1000.0)*difficulty;
		int color=rand()%3;
		s[index++].c=color;


	}
	glutTimerFunc((3000)/difficulty, update2, 0);//update func is called after 10 msec
}

int main(int argc, char **argv)
{

	// to play
	for(int i=0;i<1000000;i++)
	{
		s[i].cond=0;
		s[i].kill=0;
		s[i].in=0;
	}
	//	cout<<"argu"<<argv<<endl;
	cin>>box_len;
	spider_y=(box_len/2)-0.2;
	//cout<<box_len/2<<"aaaaaaaa";
	cin>>difficulty;
	spidervel=0.003*difficulty;
	cin>>red_x;
//	cin>>red_y;
	red_y=-box_len/2+0.2;
	cin>>green_x;
//	cin>>green_y;
	green_y=-box_len/2+0.2;
	cin>>canon_x;
	canon_y=-box_len/2+0.2;

//	cin>>canon_y;
	rightm=box_len/2;
	leftm=-box_len/2;
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	int windowWidth = w;
	int windowHeight = h;

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

	glutCreateWindow("Arachnophobia");  // Setup the window
	initRendering(); //draw the scene
	// Register callbacks
	//	glutDisplayFunc(drawScene);// when u draw , it is called...most important
	glutIdleFunc(drawScene);// when reendering on the current frame is done ....animation
	glutDisplayFunc(drawScene);// when u draw , it is called...most important

	glutKeyboardFunc(handleKeypress1);//when u press keyboard or mouse...
	glutSpecialFunc(handleKeypress2);//handles arrow keys etc
	glutMouseFunc(handleMouseclick);
	glutMotionFunc(dragwithmouse);
	glutReshapeFunc(handleResize); //when window is reshaped
	glutTimerFunc(10, update, 0);//update func is called after 10 msec
	glutTimerFunc(3000/difficulty, update2, 0);//update func is called after 10 msec

	glutMainLoop();
	return 0;
}
// Function to draw objects on the screen
void drawScene() {   //most important
	if(overflag==1)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();  
		sprintf(over,"%s","GAME OVER");
		glColor3f(0.0f, 0.0f, 1.0f); 
		glRasterPos3f(-1.0f, 0.0f,-5.0f); 
		for(int i=0;i<10;i++) 
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, over[i]); 
		
		sprintf(text,"Score: %d",score);
		glColor3f(0.0f, 0.0f, 1.0f); 
		glRasterPos3f(0.5f, 0.0f,-5.0f); 
		for(int i=0;i<10;i++) 
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]); 
		glutSwapBuffers();
	}
	//{
	if(paused==0&&overflag==0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();  
		glPushMatrix(); 

		// Draw Box   // f means float
		glTranslatef(0.0f, 0.0f, -5.0f);  // into the screen...drawing at 0,0,-5 initially
		glColor3f(0.0f, 0.0f, 1.0f);  // rgb arguments 
		drawBox(box_len);
		//////////////////////////////////////////////////////////////////////////////////////////

		glPushMatrix();

		glTranslatef(red_x*2,-2.8, -5.0f);  // into the screen...drawing at 0,0,-5 initially
		glBegin(GL_LINE_LOOP);
		glColor3f(0.0, 0.0, 0.0);
		for(int i=0;i<360;i++)
		{
			if (flagr==1)
				glColor3f(0.8, 0.498039, 0.196078);
			float rad = (i*3.14159)/180.0;
			glVertex2f(cos(rad)*0.4,sin(rad)*0.1);
		}
		glEnd();
		glPopMatrix();




		glPushMatrix();
		glLineWidth(2.5);
		glColor3f(1.0, 0.0, 0.0);
		if (flagr==1)
			glColor3f(0.8, 0.498039, 0.196078);
		glBegin(GL_LINES);
		glVertex2f(red_x+0.2, red_y-0.05);//second
		glVertex2f(red_x+0.2,red_y+0.4);//third
		glLineWidth(2.5);
		glColor3f(1.0, 0.0, 0.0);
		if (flagr==1)
			glColor3f(0.8, 0.498039, 0.196078);
		glBegin(GL_LINES);
		glVertex2f(red_x+0.2,red_y+0.4);//third
		glVertex2f(red_x-0.2,red_y+0.4);//fourth
		glLineWidth(2.5);
		glColor3f(1.0, 0.0, 0.0);
		if (flagr==1)
			glColor3f(0.8, 0.498039, 0.196078);
		glBegin(GL_LINES);
		glVertex2f(red_x-0.2,red_y+0.4);//fourth
		glVertex2f(red_x-0.2,red_y-0.05);//first coordinate
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(red_x*2,-3.7, -5.0f);  // into the screen...drawing at 0,0,-5 initially
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0, 0.0, 0.0);
		for(int i=180;i<360;i++)
		{
			if (flagr==1)
				glColor3f(0.8, 0.498039, 0.196078);
			float rad = (i*3.14159)/180.0;
			glVertex2f(cos(rad)*0.4,sin(rad)*0.1);
		}
		glEnd();
		glPopMatrix();
		//////////////////////////////////////////////////////////////////////


		glPushMatrix();
		glTranslatef(green_x*2,-2.8, -5.0f);  // into the screen...drawing at 0,0,-5 initially
		glBegin(GL_LINE_LOOP);
		glColor3f(0.0, 0.0, 0.0);
		for(int i=0;i<360;i++)
		{
			if (flagg==1)
				glColor3f(0.8, 0.498039, 0.196078);
			float rad = (i*3.14159)/180.0;
			glVertex2f(cos(rad)*0.4,sin(rad)*0.1);
		}
		glEnd();
		glPopMatrix();


		glPushMatrix();
		glLineWidth(2.5);
		glColor3f(0.0, 1.0, 0.0);
		if (flagg==1)
			glColor3f(0.8, 0.498039, 0.196078);
		glBegin(GL_LINES);
		glVertex2f(green_x+0.2, green_y-0.05);//second
		glVertex2f(green_x+0.2,green_y+0.4);//third
		glLineWidth(2.5);
		glColor3f(0.0, 1.0, 0.0);
		if (flagg==1)
			glColor3f(0.8, 0.498039, 0.196078);
		glBegin(GL_LINES);
		glVertex2f(green_x+0.2,green_y+0.4);//third
		glVertex2f(green_x-0.2,green_y+0.4);//fourth
		glLineWidth(2.5);
		glColor3f(0.0, 1.0, 0.0);
		if (flagg==1)
			glColor3f(0.8, 0.498039, 0.196078);
		glBegin(GL_LINES);
		glVertex2f(green_x-0.2,green_y+0.4);//fourth
		glVertex2f(green_x-0.2,green_y-0.05);//first coordinate
		glEnd();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(green_x*2,-3.7, -5.0f);  // into the screen...drawing at 0,0,-5 initially
		glBegin(GL_LINE_STRIP);
		glColor3f(0.0, 0.0, 0.0);
		for(int i=180;i<360;i++)
		{
			if (flagg==1)
				glColor3f(0.8, 0.498039, 0.196078);
			float rad = (i*3.14159)/180.0;
			glVertex2f(cos(rad)*0.4,sin(rad)*0.1);
		}
		glEnd();
		glPopMatrix();
		////////////////////////////////////////////////////////////	


		glPushMatrix(); 
		sprintf(text,"%d",score);
		glColor3f(1.0f, 0.0f, 0.0f); 
		glRasterPos3f(2.8f, 2.8f, -3.0f); 
		for(int i=0;i<10;i++) 
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]); 
		glPopMatrix();

		/*		if(overflag==1)
				{
				glPushMatrix(); 
				sprintf(over,"%s","overrrr");
				glColor3f(1.0f, 0.0f, 0.0f); 
				glRasterPos3f(1.0f, 0.0f, -3.0f); 
				for(int i=0;i<10;i++) 
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, over[i]); 
				glPopMatrix();
				}
				*/		glPushMatrix();
		//rad=DEG2RAD(theta);
		glTranslatef(canon_x,canon_y,0.0f);
		glRotatef(theta,0.0f,0.0f,1.0f);
		glPushMatrix();
		drawcanon();
		glPopMatrix();
		glPopMatrix();

		/////////////////////////////////////////////////////

		////    beams   
		for(int i=0;i<beamind;i++)
		{
			if(beams[i].fin!=1)
			{
				glPushMatrix();
				glLineWidth(2.5);
				glColor3f(0.0, 0.0, 1.0);
				glColor3f(0.6, 0.196078, 0.8);
				glBegin(GL_LINES);
				glVertex3f(beams[i].x, beams[i].y, 0.0);
				glVertex3f(beams[i].x+(beams[i].l)*cos(DEG2RAD(beams[i].angle)),beams[i].y+(beams[i].l)*sin(DEG2RAD(beams[i].angle)), 0);
				glEnd();
				glPopMatrix();
			}
		}
		/////////////////////////////////////
		/////   spiders

		glPushMatrix();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//not filled with color...for main screen
		//	glBegin(GL_QUADS);//making quadrilaterals
		for(int i=0;i<index;i++)
		{
			if(s[i].kill!=1)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//not filled with color...for main screen
				glBegin(GL_QUADS);//making quadrilaterals
				if(s[i].in!=1)
				{
					if(s[i].c==0)
						glColor3f(1.0f, 0.0f, 0.0f);
					else if(s[i].c==1)
						glColor3f(0.0f, 1.0f, 0.0f);
					else
						glColor3f(0.0f, 0.0f, 0.0f);

					glVertex2f(s[i].x-0.025,s[i].y-0.1);//first coordinate
					glVertex2f(s[i].x+0.025,s[i].y-0.1);//second
					glVertex2f(s[i].x+0.025,s[i].y+0.1);//third
					glVertex2f(s[i].x-0.025,s[i].y+0.1);//fourth
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025,s[i].y+0.1,0.0);
					glVertex3f(s[i].x-0.025+0.05*cos(DEG2RAD(115.0)), s[i].y+0.1+0.05*sin(DEG2RAD(115.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025+0.05*cos(DEG2RAD(115.0)), s[i].y+0.1+0.05*sin(DEG2RAD(115.0)), 0);
					glVertex3f(s[i].x-0.025+0.05*cos(DEG2RAD(115.0)), s[i].y+0.2+0.05*sin(DEG2RAD(115.0)), 0);
					glEnd();


					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025,s[i].y+0.1,0.0);
					glVertex3f(s[i].x+0.025+0.05*cos(DEG2RAD(70.0)), s[i].y+0.1+0.05*sin(DEG2RAD(70.0)), 0);
					glEnd();
					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025+0.05*cos(DEG2RAD(70.0)), s[i].y+0.1+0.05*sin(DEG2RAD(70.0)), 0);
					glVertex3f(s[i].x+0.025+0.05*cos(DEG2RAD(70.0)), s[i].y+0.2+0.05*sin(DEG2RAD(70.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025,s[i].y+0.01,0.0);
					glVertex3f(s[i].x-0.025-0.1, s[i].y+0.01, 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025-0.1,s[i].y+0.01,0.0);
					glVertex3f(s[i].x-0.025-0.1+0.05*cos(DEG2RAD(115.0)), s[i].y+0.01+0.05*sin(DEG2RAD(115.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025,s[i].y+0.01,0.0);
					glVertex3f(s[i].x+0.025+0.1, s[i].y+0.01, 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025+0.1,s[i].y+0.01,0.0);
					glVertex3f(s[i].x+0.025+0.1+0.025*cos(DEG2RAD(70.0)), s[i].y+0.05+0.025*sin(DEG2RAD(115.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025,s[i].y-0.01,0.0);
					glVertex3f(s[i].x-0.025-0.1, s[i].y-0.01, 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025-0.1,s[i].y-0.01,0.0);
					glVertex3f(s[i].x-0.025-0.1+0.025*cos(DEG2RAD(115.0)), s[i].y-0.05-0.025*sin(DEG2RAD(115.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025,s[i].y-0.01,0.0);
					glVertex3f(s[i].x+0.025+0.1, s[i].y-0.01, 0);
					glEnd();
					/////////////////////////////////////////////	
					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025+0.1,s[i].y-0.01,0.0);
					glVertex3f(s[i].x+0.025+0.1+0.05*cos(DEG2RAD(80.0)), s[i].y-0.01-0.05*sin(DEG2RAD(80.0)), 0);
					glEnd();


					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025,s[i].y-0.1,0.0);
					glVertex3f(s[i].x-0.025+0.05*cos(DEG2RAD(115.0)), s[i].y-0.1-0.05*sin(DEG2RAD(115.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x-0.025+0.05*cos(DEG2RAD(115.0)), s[i].y-0.1-0.05*sin(DEG2RAD(115.0)), 0);
					glVertex3f(s[i].x-0.025+0.05*cos(DEG2RAD(115.0)), s[i].y-0.2-0.05*sin(DEG2RAD(115.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025,s[i].y-0.1,0.0);
					glVertex3f(s[i].x+0.025+0.05*cos(DEG2RAD(70.0)), s[i].y-0.1-0.05*sin(DEG2RAD(70.0)), 0);
					glEnd();

					glLineWidth(2.5);
					glBegin(GL_LINES);
					glVertex3f(s[i].x+0.025+0.05*cos(DEG2RAD(70.0)), s[i].y-0.1-0.05*sin(DEG2RAD(70.0)), 0);
					glVertex3f(s[i].x+0.025+0.05*cos(DEG2RAD(70.0)), s[i].y-0.2-0.05*sin(DEG2RAD(70.0)), 0);
					glEnd();

				}
			}
		}
		//	glEnd();
		glPopMatrix();
		//////////////////////
		glPushMatrix();
		glLineWidth(2.5);
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(-box_len/2, -box_len/2+(box_len*0.1), 0.0);
		glVertex3f(box_len/2, -box_len/2+(box_len*0.1), 0);
		glEnd();
		glPopMatrix();





		glPopMatrix();
		glutSwapBuffers();
	}
	//	}
}
// Function to handle all calculations in the scene
// updated evry 10 milliseconds

void drawcanon()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//not filled with color...for main screen
	glBegin(GL_QUADS);//making quadrilaterals
	glColor3f(0.0, 0.0, 1.0);
	if (flagc==1)
		glColor3f(0.6, 0.196078, 0.8);

	glVertex3f(-0.15,-0.1,0.0);
	glVertex3f( +0.15,  -0.1, 0);
	glVertex3f( +0.15,  +0.1, 0);
	glVertex3f( -0.15,  +0.1, 0);
	glEnd();



	//	glPushMatrix();
	glLineWidth(2.5);
	glColor3f(0.0, 0.0, 1.0);
	if (flagc==1)
		glColor3f(0.6, 0.196078, 0.8);
	glBegin(GL_LINES);
	glVertex3f( +0.15, 0,0.0);
	glVertex3f( +0.2,  0, 0);
	glEnd();
	//	glPopMatrix();

	//	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//not filled with color...for main screen
	glBegin(GL_QUADS);//making quadrilaterals
	glColor3f(0.0, 0.0, 1.0);
	if (flagc==1)
		glColor3f(0.6, 0.196078, 0.8);
	glVertex3f( +0.2, -0.1,0.0);
	glVertex3f( +0.3,  -0.1, 0);
	glVertex3f( +0.3,  +0.1, 0);
	glVertex3f( +0.2,  +0.1, 0);
	glEnd();
	//	glPopMatrix();

	//	glPushMatrix();
	glLineWidth(2.5);
	glColor3f(0.0, 0.0, 1.0);
	if (flagc==1)
		glColor3f(0.6, 0.196078, 0.8);
	glBegin(GL_LINES);
	glVertex3f( -0.15, 0,0.0);
	glVertex3f( -0.2, 0 , 0);
	glEnd();
	//	glPopMatrix();

	//	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//not filled with color...for main screen
	glBegin(GL_QUADS);//making quadrilaterals
	glColor3f(0.0, 0.0, 1.0);
	if (flagc==1)
		glColor3f(0.6, 0.196078, 0.8);
	glVertex3f( -0.3, -0.1,0.0);
	glVertex3f( -0.2,  -0.1, 0);
	glVertex3f( -0.2,  +0.1, 0);
	glVertex3f( -0.3,  +0.1, 0);
	glEnd();
	//	glPopMatrix();

	//	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//not filled with color...for main screen
	glBegin(GL_QUADS);//making quadrilaterals
	glColor3f(0.0, 0.0, 1.0);
	if (flagc==1)
		glColor3f(0.6, 0.196078, 0.8);
	glVertex3f( -0.05, +0.1,0.0);
	glVertex3f( +0.05,  +0.1, 0);
	glVertex3f( +0.05,  +0.4, 0);
	glVertex3f( -0.05,  +0.4, 0);
	glEnd();
}
//	glPopMatrix();
int sh=0;
void update(int value) 
{
	if( paused==0&&overflag==0)
	{

		for(int i=0;i<beamind;i++)
		{
			for(int j=0;j<index;j++)
			{
				if(s[j].cond!=1&&s[j].kill!=1)
				{
					if(s[j].score!=1&&s[j].kill!=1&&s[j].x+0.15>=beams[i].x&&s[j].y+0.2>=beams[i].y&&beams[i].x>=s[j].x-0.15&&beams[i].y>=s[j].y-0.2&&beams[i].fin!=1)
					{
						s[j].kill=1;
						beams[i].fin=1;
						if(s[j].c==2)
						{
							score=score+1;
							s[j].score=1;
						}
						//	sprintf(text,"%d",score);
					}
				}
			}
		}


		for(int i=0;i<index;i++)
		{

			if(s[i].y>=-1.7&&s[i].y-0.1<=-1.6&&s[i].x-0.1>=red_x-0.2&&s[i].x+0.1<=red_x+0.2&&s[i].cond!=1)
			{
				if(s[i].c==0&&s[i].score!=1)
				{	score++;
					s[i].in=1;
					s[i].score=1;
					//	sprintf(text,"%d",score);
				}
				else
				{
					if(s[i].score!=1&&(s[i].c==1||s[i].c==2))
					{	score--;
						s[i].score=1;
						s[i].in=1;
					}
					//				sprintf(text,"%d",score);

				}
			}
			if(s[i].y-0.1<=-1.6&&s[i].x-0.1>=green_x-0.2&&s[i].x+0.1<=green_x+0.2&&s[i].cond!=1)
			{
				if(s[i].c==1&&s[i].score!=1)
				{
					s[i].in=1;
					score++;
					s[i].score=1;
					//		sprintf(text,"%d",score);
				}
				else
				{
					if(s[i].score!=1&&(s[i].c==0||s[i].c==2))
					{	score--;
						s[i].score=1;
						s[i].in=1;
					}
					//	sprintf(text,"%d",score);
				}
			}


		}

		for(int i=0;i<beamind;i++)
		{
			if(beams[i].x<=box_len/2&&beams[i].x>=-box_len/2)
			{
				beams[i].x+=((0.08f)*cos(DEG2RAD(beams[i].angle)));
				beams[i].y+=((0.08f)*sin(DEG2RAD(beams[i].angle)));
			}
			else
			{
				if(beams[i].x>box_len/2)
				{
					beams[i].reflected=1;
					beams[i].x=box_len/2;
					beams[i].angle=180.0-beams[i].angle;
				}
				else if(beams[i].x<-box_len/2)
				{
					beams[i].x=-box_len/2;
					beams[i].angle=180.0-beams[i].angle;
				}

			}
		}
		for(int i=0;i<index;i++)
		{
			if(s[i].y>-box_len/2+0.3)
				s[i].y-=s[i].vel;
			else if((s[i].y<=-box_len/2+0.3)&&s[i].score!=1&&s[i].kill!=1&&s[i].in!=1)
			{
				s[i].cond=1;
				score-=5;
				s[i].score=1;
				//	sprintf(text,"%d",score);
			}
		}
		for(int i=0;i<index;i++)
			if(leftm<s[i].x+0.475&&s[i].kill!=1&&s[i].in!=1&&s[i].cond==1&&(s[i].x+0.475<canon_x))
				leftm=s[i].x+0.475;
		for(int i=0;i<index;i++)
		{
			if(rightm>s[i].x-0.475&&s[i].kill!=1&&s[i].in!=1&&s[i].cond==1&&(s[i].x-0.475>canon_x))
				rightm=s[i].x-0.475;
			//	cout <<"rrrr  "<<rightm<<endl;
		}
		for (int i=0;i<index;i++)
		{
			if(s[i].kill!=1&&s[i].cond!=1&&s[i].in!=1&&s[i].x-canon_x<0.175&&s[i].x-canon_x>-0.175&&s[i].y-canon_y<0.4)
			{
					overflag=1;
				//	glPushMatrix(); 
				//	glPopMatrix();

				//	exit(0);
			}
		}




	}
	glutTimerFunc(10, update, 0);
}

void drawBox(float len) {

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//not filled with color...for main screen
	glBegin(GL_QUADS);//making quadrilaterals
	glVertex2f(-len / 2, -len / 2);//first coordinate
	glVertex2f(len / 2, -len / 2);//second
	glVertex2f(len / 2, len / 2);//third
	glVertex2f(-len / 2, len / 2);//fourth
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void initRendering() {

	glEnable(GL_DEPTH_TEST);        
	glEnable(GL_COLOR_MATERIAL);    
	glClearColor(0.91f, 0.76f, 0.65f, 0.0f);   
}

void handleResize(int w, int h) {  

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void handleKeypress1(unsigned char key, int x, int y) {// keyboard
	if(key==' ')
	{
		if(flagc==1&&paused==0&&overflag==0)
		{

			time_t currtime=time(NULL);
			if(difftime(currtime,prev)>=1)
			{
				system("aplay LASER1.WAV &");
				beams[beamind].x=canon_x;
				beams[beamind].y=canon_y;
				beams[beamind].angle=theta+90;
				beams[beamind++].l=0.1*box_len;
				prev=currtime;
			}
		}

	}

	if (key == 27) {
		exit(0);     // escape key is pressed
	}

	if(key=='b'&&paused==0&&overflag==0)
	{
		flagc=1;
		flagr=0;
		flagg=0;
		glutSpecialFunc(handleKeypress2);//handles arrow keys etc
	}
	if(key=='r'&&paused==0&&overflag==0)
	{
		flagr=1;
		flagc=0;
		flagg=0;
		glutSpecialFunc(handleKeypress2);//handles arrow keys etc
	}
	if(key=='g'&&paused==0&&overflag==0)
	{
		flagc=0;
		flagr=0;
		flagg=1;
		glutSpecialFunc(handleKeypress2);//handles arrow keys etc
	}
	if(key=='p')
	{
		if(paused==0&&overflag==0)
			paused=1;
		else
			paused=0;
	}

}
int flagm=0;
void handleKeypress2(int key, int x, int y) 
{//arrow keys
	if(paused==0&&overflag==0)
	{
		if (key == GLUT_KEY_LEFT&&flagc==1&&canon_x>-box_len/2+0.3)
		{
			if(canon_x>leftm)
				canon_x-=0.1;

		}
		if (key == GLUT_KEY_RIGHT&&flagc==1&&canon_x<box_len/2-0.3)
		{
			if(canon_x<rightm)
				canon_x+=0.1;
		}
		if (key == GLUT_KEY_LEFT&&flagr==1&&red_x>-box_len/2+0.2)
			red_x-=0.1;
		if (key == GLUT_KEY_RIGHT&&flagr==1&&red_x<box_len/2-0.2)
			red_x+=0.1;
		if (key == GLUT_KEY_LEFT&&flagg==1&&green_x>-box_len/2+0.2)
			//			if(green_x>greenleftm)
			green_x-=0.1;
		if (key == GLUT_KEY_RIGHT&&flagg==1&&green_x<box_len/2-0.2)
			//			if(green_x<greenrightm)
			green_x+=0.1;
		if (key == GLUT_KEY_UP &&flagc==1)
			if(theta<=75)

				theta+=15;
		if (key == GLUT_KEY_DOWN&&flagc==1)
			if(theta>=-75)
				theta-=15;
	}
}
/*void handleMouseclick(int button, int status, int x, int y)
  {

  if (status == GLUT_DOWN)//down==press..........up=releasing
  {
  if (button == GLUT_LEFT_BUTTON)
  theta += 15;
  else if (button == GLUT_RIGHT_BUTTON)
  theta -=15;
  }
  i}*/

float distance(float a,float b,float c,float d)
{
	return sqrt((a-c)*(a-c)+(b-d)*(b-d));
}

float tempmousex,tempmousey,mouseposz;

void actualcod(int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	tempmousex=posX*100/2;
	tempmousey=(posY*100)/2;
	mouseposz=posZ*100;
}



void handleMouseclick(int button, int status, int x, int y)
{
	//	printf("x--- %d\n",x);
	//	printf("y--- %d\n",y);
	//	if(paused)
	//		return;
	// mousex=((float)x-455)/125;
	// mousey=(-(float)y+255)/125;
	actualcod(x,y);
	mouseactual_x=tempmousex;
	mouseactual_y=tempmousey;
	if (status == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			//cout<<x<<":"<<mousex<<": "<<can_x<<"\n";
			if(distance(mouseactual_x,mouseactual_y,canon_x,canon_y)<0.3)
			{
				flagc=1;
				flagg=0;
				flagr=0;

				selc=1;
				selg=0;
				selr=0;
				rotcan=0;

			}
			else if(distance(mouseactual_x,mouseactual_y,green_x,canon_y)<0.3)
			{
				flagc=0;
				flagg=1;
				flagr=0;
				selc=0;
				selg=1;
				selr=0;
				rotcan=0;
			}
			else if(distance(mouseactual_x,mouseactual_y,red_x,canon_y)<0.3)
			{
				flagc==0;
				flagg=0;
				flagr=1;
				selc=0;
				selg=0;
				selr=1;
				rotcan=0;
			}
			else
			{
				selc=0;
				selg=0;
				selr=0;
				rotcan=0;
			}

			//  cout<<"hi\n";
		}
		if(button == GLUT_RIGHT_BUTTON)
		{


			selc=0;
			selg=0;
			selr=0;
			if(distance(mouseactual_x,mouseactual_y,canon_x,canon_y)<0.8)
			{
				flagc=1;
				flagg=0;
				flagr=0;
				rotcan=1;
			}
			else
				rotcan=0;
		}
		//cout<<"x: "<<mousex<<"can_x: "<<can_x<<"\n";
		//cout<<selc<<rotcan<<selr<<selg<<mousex<<"\n"<<mousey<<"\n"<<distance(mousex,mousey,canon_x,canon_y)<<"\n";
		//else if (button == GLUT_RIGHT_BUTTON)
		//           cout<<"y: "<<mousey<<"\n";
	}
	glutPostRedisplay();
}

float drag(float position,float lefy,float rify)
{

	float  move;
	printf("mousex  %f  %f\n",mouseactual_x,position);
	printf("%f\n",distance(mouseactual_x,mouseactual_y,position,canon_y));
	if(distance(mouseactual_x,mouseactual_y,position,canon_y)<0.8)
	{   

		move=(floor)(2*(mouseactual_x-position));
		printf("mov    %f\n",move);
		if(move>0)
		{   
			if(position+(move*0.3)< rightm)
				position+=(move*0.3);
		}   
		else 
		{   
			if(position+(move*0.3)>leftm)
				position+=(move*0.3);
		}   
	}   
	return position;
}

void dragwithmouse(int x,int y)
{
	//	if(paused)
	//		return;
	actualcod(x,y);
	mouseactual_x=tempmousex;
	mouseactual_y=tempmousey;
	if(selr)
	{
		red_x=drag(red_x,-box_len/2+0.2,box_len/2-0.2);
	}
	else if(selg)
	{
		green_x=drag(green_x,-box_len/2+0.2,box_len/2-0.2);
	}
	else if(selc)
	{
		//	printf("**");
		canon_x=drag(canon_x,leftm,rightm);

	}
	if(rotcan && distance(mouseactual_x,mouseactual_y,canon_x,canon_y)<0.8)
	{
		if(canon_x-mouseactual_x>0)
		{
			if(theta <=75)
				theta +=5;
		}
		else
			if(theta >=-75)
				theta -= 5;
	}


}
