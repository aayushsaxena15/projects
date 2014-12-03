
#include <iostream>
#include <stdlib.h>
#include<math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "imageloader.h"
#include "vec3f.h"
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<time.h>
#define ESC 27
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define RAD2DEG(rad) (rad * 180 / PI)

using namespace std;
void draw_cube();
void Set_Camera();
//Represents a terrain, by storing a set of heights and normals at 2D locations
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

float _angle = 60.0f;
Terrain* _terrain;

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
bool brake=0;
float prevpitch=0;
float acc=0,ret=0;
float vel=0;
float yaw=0; //angle about y to tell where it is going
float pitch=0; //angle to increase hight
float roll=0;//tilt
int cammode=0;
float bike_x=0,bike_z=0,bike_y=0.375;
float blook_x=0.0,blook_y=1.0,blook_z=0.0;
float x = 0.0, y = -5.0; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move
int ifrolled;
// Camera direction
float lx = 0.0, ly = 1.0; // camera points initially along y-axis
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging
float deltaRotate=0;
float avg_h,h1;//,h2,h3,h4,h5,h6,h7,h8,h9;
vector < pair<int,int> >fossil;
float fossil_size=0.5f;
float tyre_angle=0;

void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

float getmagnitude(float a,float b,float c)
{
    return sqrt((a*a)+(b*b)+(c*c));
}



void drawScene() {
    glClearColor(0.0,0.7,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, 0.0f);
    if(brake)
    {
        vel=0;
        ret=0;
        acc=0;
    }
    if(vel>=0 || pitch!=0) 
    {
        bike_z+=vel*cosf(DEG2RAD(yaw));
        bike_x+=vel*sinf(DEG2RAD(yaw));
    }
    else if(vel<0)
    {
       vel=0;
        acc=0;
        ret=0;
   }
    if(bike_z<0)
        bike_z=0;
    if(bike_x<0)
        bike_x=0;
    if(bike_z>117)
        bike_z=117;
    if(bike_x>117)
        bike_x=117;

    h1=heightAt(_terrain,bike_x,bike_z);
    /*_terrain->getHeight((int)bike_x,(int)bike_z);
    h2=_terrain->getHeight(((int)bike_x)+1,(int)bike_z);
    h3=_terrain->getHeight((int)bike_x,((int)bike_z)+1);
    h4=_terrain->getHeight(((int)bike_x)+1,((int)bike_z)+1);
    h5=_terrain->getHeight(((int)bike_x)-1,((int)bike_z)-1);
    h6=_terrain->getHeight(((int)bike_x)-1,((int)bike_z));
    h7=_terrain->getHeight(((int)bike_x),((int)bike_z)-1);
    h8=_terrain->getHeight(((int)bike_x)-1,((int)bike_z)+1);
    h9=_terrain->getHeight(((int)bike_x)+1,((int)bike_z)-1);
    avg_h=((h2/8)+(h3/8)+(h4/8)+(h5/8)+(h6/8)+(h7/8)+(h8/8)+(h9/8));
    avg_h=(avg_h/2)+(h1/2);
    pitch=RAD2DEG(atan((avg_h-bike_y)/1.414));
    pitch+=10.0265;
    */
    Vec3f tnormal = _terrain->getNormal(bike_x, bike_z);
    //cout<<"normal\n";
   // cout<<tnormal[0]<<" "<<tnormal[1]<<" "<<tnormal[2]<<"\n";
   float tryc,ab,ac,ad;//,ac,ab,ac;
   // mag=getmagnitude(bike_x,bike_y,bike_z);
    ab=sinf(DEG2RAD(yaw));
    ac=0;//sinf(DEG2RAD(pitch));
    ad=cosf(DEG2RAD(yaw));
    //ac=bike_x/mag;
    //ac=bike_x/mag;
    tryc=((tnormal[0]*ab+tnormal[2]*ad+tnormal[1]*ac)/(getmagnitude(tnormal[0],tnormal[1],tnormal[2])*getmagnitude(ab,ac,ad)));
    //cout<<(getmagnitude(tnormal[0],tnormal[1],tnormal[2]))<<"\n";
    //cout<<"bike look vector\n";
    //cout<<ab<<" "<<ad<<"\n";
    tryc=RAD2DEG(acos(tryc));
    pitch=-(90-tryc);
    //if(tnormal[0]>0)
      //  pitch=-pitch;
   // else if(tnorn
   // cout<<pitch<<"\n";
   // cout<<"sinpitch "<<-sinf(DEG2RAD(pitch))<<"\n";
    bike_y=h1+0.25;
/* 
        gluLookAt(
                //  bike_x,      bike_y-0.5,      bike_z+1,
                bike_x-(2*sinf(DEG2RAD(yaw))), bike_y+1,bike_z-(2*cosf(DEG2RAD(yaw))),
                bike_x+sinf(DEG2RAD(yaw)), bike_y+1,bike_z+cosf(DEG2RAD(yaw)),
                0.0,    1.0,    0.0
                );
  */
            Set_Camera();
    GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    // float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);

    glColor3f(0.3f, 0.9f, 0.0f);
    for(int z = 0; z < _terrain->length() - 1; z++) {
        //Makes OpenGL draw a triangle at every three consecutive vertices
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x < _terrain->width(); x++) {
            Vec3f normal = _terrain->getNormal(x, z);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(x, _terrain->getHeight(x, z), z);
            normal = _terrain->getNormal(x, z + 1);
            glNormal3f(normal[0], normal[1], normal[2]);
            glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
        }
        glEnd();
    } 
    draw_cube();
    for(size_t k=0;k<fossil.size();k++)
    {
        glPushMatrix();
        glTranslatef(fossil[k].first,heightAt(_terrain,fossil[k].first,fossil[k].second),fossil[k].second);
        glColor3f(0.0f, 0.0f, 0.0f);
        glutSolidCube(fossil_size);
        glPopMatrix();
    }
    glutSwapBuffers();
}
void update() 
{

    if(prevpitch<0 && pitch==0)
    {
        ret=-0.005;
    }
    prevpitch=pitch;
    if(vel>0)
    vel=vel+acc+ret-(0.001*sinf(DEG2RAD(pitch)));
    else //if(vel==0)
    {
        ret=0;
       // 
        //if(pitch<0)
       // {
            vel=vel+acc-(0.001*sinf(DEG2RAD(pitch)));
      //  }
  //  else
    //vel=vel+acc;
    }
    if(deltaRotate==-1)
        yaw+=5;
    else if(deltaRotate==1)
        yaw-=5;
    if( -45< (roll+(5*ifrolled)) && roll+(5*ifrolled)<45)
        roll=roll+(5*ifrolled);
    if(ifrolled)
        if(vel!=0 && vel>0.05)
        {
            yaw+=-0.7*tanf(DEG2RAD(roll))/vel;

        }
    glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------

void draw_cube()
{
    glColor3f(1.0,1.0,0.0); 
    glPushMatrix();
    glTranslatef(bike_x,0,bike_z);
    glRotatef(yaw,0,1,0);
    glRotatef(-pitch,1,0,0);
    glRotatef(roll,0,0,1);
    glTranslatef(0,bike_y+0.05,0);//to keep it till ground
    glPushMatrix();
    glTranslatef(0.0,0.0,-0.125);
    glutSolidCube(0.25);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0,0.0,0.125);
    glutSolidCube(0.25);
    glTranslatef(0.0,0.0,0.125);
    
    //headlight
    glPushMatrix();
    glColor3f(1.0,1.0,1.0); 
    glTranslatef(0.0,0.175,0.0);
    glutSolidCone(0.05,0.1,20,20);
    glPopMatrix();
    
    //handle
    glPushMatrix();
    glColor3f(1.0,0.0,0.0); 
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex3f(0.125,0.125,0.0);
    glVertex3f(0.375,0.375,0.0);
    glVertex3f(-0.125,0.125,0.0);
    glVertex3f(-0.375,0.375,0.0);
    glEnd();
    glPopMatrix();
    
    glPopMatrix();


    tyre_angle+=vel/0.06;
   
    //back wheel
    glPushMatrix();
    glColor3f(0.0,0.0,0.0); 
    glTranslatef(0.0,-0.125,-0.375);
    glRotatef(-90,1,0,0);
    glRotatef(90,0,1,0);
    glRotatef(RAD2DEG(tyre_angle),0,0,1);//wheel rotation
    glutWireTorus(0.06,0.12,10,10);
    glPopMatrix();

    //front wheel
    glPushMatrix();
    glColor3f(0.0,0.0,0.0);
    glTranslatef(0.0,-0.125,0.375);
    glRotatef(-90,1,0,0);
    glRotatef(90,0,1,0);
    glRotatef(RAD2DEG(tyre_angle),0,0,1);//wheel rotation
    glutWireTorus(0.06,0.12,10,10);
    glPopMatrix();

    glPopMatrix();
  }
float  view=1;
void Set_Camera()
{
    if(cammode%5==0)
    {
        // Driver View
        gluLookAt(
                bike_x-(2*sin(DEG2RAD(yaw))), bike_y+1,bike_z-(2*cos(DEG2RAD(yaw))),
                bike_x,      bike_y+1,      bike_z,
                0.0,    1.0,    0.0
                );
        
    }
    else if(cammode%5==1)
    {
        // Wheel View
        gluLookAt(
                bike_x,      bike_y+0.25,      bike_z,
                bike_x+2*sinf(DEG2RAD(yaw)), bike_y+0.25,bike_z+2*cosf(DEG2RAD(yaw)),
                0.0,    1.0,    0.0
                );

    }
    else if(cammode%5==2)
    {
        //OverHead View
        gluLookAt(
                bike_x,      bike_y+10,      bike_z,
                bike_x+sin(DEG2RAD(yaw)), bike_y,bike_z+cos(DEG2RAD(yaw)),
                0.0,    1.0,    0.0
                );
    }
    else if(cammode%5==3)
    {

        // Hellicopter View
        //not right //myview
        gluLookAt(
                bike_x-3,     bike_y,bike_z,
                bike_x, bike_y+0.25, bike_z,
                0.0,    1.0,    0.0
                );
    }
    else if(cammode%5==4)
    {
        // Follow Cam
        gluLookAt(
                bike_x-(3.5*sin(DEG2RAD(yaw))), bike_y+1,bike_z-(3.5*cos(DEG2RAD(yaw))),
                bike_x,      bike_y+1,      bike_z,
                0.0,    1.0,    0.0
                );

    }
}
void processNormalKeys(unsigned char key, int xx, int yy)
{
    if (key == ESC || key == 'q' || key == 'Q'){
        cleanup();
        exit(0);
    }
    if(key=='v')
    {
        cammode++;
    }
    if(key=='a')
    {
        deltaRotate=-1;
        yaw-=5;
    }
    else if(key=='d')
    {
        deltaRotate = 1.0; 
        yaw+=5;
    }
} 
void KeyUp(unsigned char key, int x, int y) 
{
    switch (key) {
        case 'a' :
            deltaRotate = 0.0; 
            break;
        case 'd':
            deltaRotate = 0.0; 
            break;
    }
}  
void pressSpecialKey(int key, int xx, int yy)
{
    switch (key) {
        case GLUT_KEY_UP : 
            deltaMove = 1.0;
            acc=0.005;
            break;
        case GLUT_KEY_DOWN : 
            deltaMove = -1.0;
            brake=1;
            // ret=-1; 
            break;
        case GLUT_KEY_RIGHT :
            ifrolled=1;
            break;
        case GLUT_KEY_LEFT : 
            ifrolled=-1;
            break;
    }
} 

void releaseSpecialKey(int key, int x, int y) 
{
    switch (key) {
        case GLUT_KEY_UP : 
            deltaMove = 0.0; 
            ret=-0.005;
            acc=0;
            break;
        case GLUT_KEY_DOWN : 
            deltaMove = 0.0; 
            acc=0;
            brake=0;
            break;
        case GLUT_KEY_LEFT :
            deltaRotate = 0.0; 
            roll=0;
            ifrolled=0;
            break;
        case GLUT_KEY_RIGHT : 
            deltaRotate = 0.0; 
            roll=0;
            ifrolled=0;
            break;
    }
} 

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 400);

    glutCreateWindow("test");
    initRendering();

    _terrain = loadTerrain("heightmap.bmp", 20);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(handleResize);
    glutIdleFunc(update); // incremental update 
    glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
    glutKeyboardFunc(processNormalKeys); // process standard key clicks
    glutSpecialFunc(pressSpecialKey); // process special key pressed
    glutKeyboardUpFunc(KeyUp); 
    // Warning: Nonstandard function! Delete if desired.
    glutSpecialUpFunc(releaseSpecialKey); // process special key release
    srand(time(NULL));
    for(int i =0; i<6;i++)
        for(int j=0;j<6;j++)
        {
            fossil.push_back(make_pair((rand()%119 + 1),(rand()%119 + 1)));
        }
    glutMainLoop();
    return 0;
}
