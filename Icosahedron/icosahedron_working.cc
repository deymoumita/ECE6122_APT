// Draw an Icosahedron
// ECE4893/8893 Project 4
// MOUMITA DEY

#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

#define DEPTH_LIMIT 4 
#define MUL_C1 15
#define MUL_C2 23
#define MUL_C3 10

bool rotate = false;
int count = 0;


// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int testNumber; // Global variable indicating which test number is desired

int winW = 512;
int winH = 512; 
float aspect = (float)winW/(float)winH;
static GLfloat rotX = 0.0;
static GLfloat rotY = 0.0;
static GLfloat dX = 1.0;
static GLfloat dY = 1.0;
static GLfloat updateRate = 10.0;
GLuint maxDepth = 4;

void normalize(GLfloat v[3])
{
  GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  if (d == 0.0)
  {
    return;
  }
  v[0] = v[0] / d;
  v[1] = v[1] / d;
  v[2] = v[2] / d;
}

void drawTriangle(GLfloat *v1, GLfloat *v2, GLfloat *v3)
{
  normalize(v1);
  normalize(v2);
  normalize(v3);

  glBegin(GL_TRIANGLES);

  long num1 = count * MUL_C1;
  num1 = num1 % 100;
  float num_f1 = num1 / 100.0;

  long num2 = count * MUL_C2;
  num2 = num2 % 100;
  float num_f2 = num2 / 100.0;

  long num3 = count * MUL_C3;
  num3 = num3 % 100;
  float num_f3 = num3 / 100.0;

  glColor3f(num_f1, num_f2, num_f3); // colorful triangle face
  glVertex3fv(v1);
  glVertex3fv(v2);
  glVertex3fv(v3);
  glEnd();

  glBegin(GL_LINE_LOOP);
  //glColor3f(0.0, 0.0, 0.0); // black outline of triangle
  glColor3f(1.0, 1.0, 1.0); // white outline of triangle
  glVertex3fv(v1);
  glVertex3fv(v2);
  glVertex3fv(v3); 
  glEnd();

}

void subDivide(GLfloat *v1, GLfloat *v2, GLfloat *v3, int depth)
{
  count++;
  if (depth == 0)
  {
    drawTriangle(v1, v2, v3);
    return;
  }
  GLfloat v12[3];
  GLfloat v23[3];
  GLfloat v31[3];

  for (int i = 0; i < 3; i++)
  {
    v12[i] = (v1[i] + v2[i]) / 2.0;
    v23[i] = (v2[i] + v3[i]) / 2.0;
    v31[i] = (v3[i] + v1[i]) / 2.0;
  }

  normalize(v12);
  normalize(v23);
  normalize(v31);

  depth--;

  subDivide(v1, v12, v31, depth);
  subDivide(v2, v12, v23, depth);
  subDivide(v31, v3, v23, depth);
  subDivide(v12, v31, v23, depth); 
  
}

void drawIcosahedron(int depth)
{
  count = 0;
  for (int i = 0; i < 20; i++)
  {
    subDivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], depth);
  }
}

void display(void)
{
  //glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  // works 
  glTranslatef(winW/2, winH/2, -winW/2);
  glScalef(winW/2, winW/2, -winW/2);

  if(rotate == true)
  {
    glRotatef(rotX, 1.0, 0.0, 0.0);
    glRotatef(rotY, 0.0, 1.0, 0.0);  
    rotX += dX;
    rotY += dY;
  }

  drawIcosahedron(maxDepth);

  glutSwapBuffers();
  glFlush();
}

void init()
{
  //select clearing (background) color
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  int width = h * aspect;
  int left = (w - width) / 2; 
  glViewport(left, 0, width, (GLsizei)h);

  glOrtho(0.0, winW, 0.0, winH, -w, w);
  glMatrixMode(GL_MODELVIEW);
  
}

void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0 / updateRate, timer, 0);
}

// Test cases.  Fill in your code for each test case
void Test1()
{
  rotate = false;
  glPushMatrix();
  maxDepth = 0;
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  //glutTimerFunc(1000.0 / updateRate, timer, 0);
  glPopMatrix();
}

void Test2()
{
  rotate = true;
  glPushMatrix();
  maxDepth = 0;
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  glPopMatrix();
}

void Test3()
{
  rotate = false;
  glPushMatrix();
  maxDepth = 1;
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  //glutTimerFunc(1000.0 / updateRate, timer, 0);
  glPopMatrix();

}

void Test4()
{
  rotate = true;
  glPushMatrix();
  maxDepth = 1;
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  glPopMatrix();


}

void Test5(int depth)
{
  rotate = false;
  glPushMatrix();
  maxDepth = depth;
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  //glutTimerFunc(1000.0 / updateRate, timer, 0);
  glPopMatrix();


}

void Test6(int depth)
{
  rotate = true;
  glPushMatrix();
  maxDepth = depth;
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  glPopMatrix();


}

int main(int argc, char** argv)
{
  if (argc < 2)
    {
      cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
  // Set the global test number
  testNumber = atol(argv[1]);
  // Initialize glut  and create your window here
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(600, 600);
  glutInitWindowPosition(100, 10);
  glutCreateWindow("Icosahedron");
  init();

  // Set your glut callbacks here
  if (testNumber == 1)
    Test1();
  else if (testNumber == 2)
    Test2();
  else if (testNumber == 3)
    Test3();
  else if (testNumber == 4)
    Test4();
  else if (testNumber == 5)
  {
    if (argc < 3)
    {
      cout << "Usage: icosahedron testnumber depth" << endl;
      exit(1);
    }
    if(atoi(argv[2]) > 4)
    {
      cout<< "Depth should be less than 5" << endl;
      exit(1);
    }
    Test5(atoi(argv[2]));        
  }
  else if (testNumber == 6)
  {
    if (argc < 3)
    {
      cout << "Usage: icosahedron testnumber depth" << endl;
      exit(1);
    }
    if(atoi(argv[2]) > 4)
    {
      cout<< "Depth should be less than 5" << endl;
      exit(1);
    }
    Test6(atoi(argv[2]));        
  }
  else 
  {
    cout << "Invalid testnumber" << endl;
    exit(1);
  }

  // Enter the glut main loop here
  glutMainLoop();

  return 0;
}

