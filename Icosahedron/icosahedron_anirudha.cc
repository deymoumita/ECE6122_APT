// Draw an Icosahedron
// ECE4893/8893 Project 4
// YOUR NAME HERE

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

static const float DEG2RAD = M_PI/180;
static int updateRate = 50;


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
int depth = 0;

void drawCircle()
{
  // Draw a circle by drawing 360 small lines
  GLfloat radius = 1.0;
  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < 360; i++)
    {
      float degInRad = i*DEG2RAD;
      glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
   }
  glEnd();
  // Add spokes each 30 degrees
  for (int i = 0; i < 360; i += 30)
    {
      // Just for fun make spokes different colors
      glColor3f(i / 360.0, 360.0 - i / 360.0, 0.5);
      float degInRad = i*DEG2RAD;
      glBegin(GL_LINES);
      glVertex2f(0.0, 0.0);
      glVertex2f(cos(degInRad), sin(degInRad));
      glEnd();
    }
}



void Normalize(GLfloat v[3]) {

    GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

    if (d == 0) return;

    v[0] /= d;
    v[1] /= d;
    v[2] /= d;

}

void DrawTriangle(GLfloat* v1, GLfloat* v2, GLfloat* v3) {
  //glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  //glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    

    Normalize(v1);
	Normalize(v2);
	Normalize(v3);
    glBegin(GL_TRIANGLES);
    //glColor3f((GLfloat)(rand() % 256) / 256, (GLfloat)(rand() % 256) / 256, (GLfloat)(rand() % 256) / 256);
    glColor3f((GLfloat)((v1[1]+v2[1]+v3[1])/((v1[1]*v2[1]*v3[1]))), (GLfloat)((v1[2]+v2[2]+v3[2])/((v1[2]*v2[2]*v3[2]))), (GLfloat)((v1[3]+v2[3]+v3[3])/((v1[3]*v2[3]*v3[3]))));
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glEnd();


    glBegin(GL_LINE_LOOP);
    glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glEnd();

}


void Subdivide(GLfloat* v1, GLfloat* v2, GLfloat* v3, int localDepth) {

    if(localDepth == 0) {
        
        //if ((v1[2] > 0) || (v2[2] > 0) || (v3[2] > 0)) return;
        /*
        v1[3] = max((float)0.0, (float)v1[3]);
        v2[3] = max((float)0.0, (float)v2[3]);
        v3[3] = max((float)0.0, (float)v3[3]);
        */
        DrawTriangle(v1, v2, v3);
        return;

    }

    GLfloat v12[3];
    GLfloat v23[3];
    GLfloat v31[3];

    for(int i=0; i<3; i++) {

        v12[i] = v1[i] + v2[i];
        v23[i] = v2[i] + v3[i];
        v31[i] = v3[i] + v1[i];

    }

    Normalize(v12);
    Normalize(v23);
    Normalize(v31);

    Subdivide(v1, v12, v31, localDepth-1);
    Subdivide(v2, v23, v12, localDepth-1);
    Subdivide(v3, v31, v23, localDepth-1);
    Subdivide(v12, v23, v31, localDepth-1);
    


}

void DrawIcosahedron(int localDepth) {

   for(int i=0; i<NFACE; i++) {

        Subdivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0], localDepth);

   } 

}

void display(void)
{
  static int pass;
  
  cout << "Displaying pass " << ++pass << endl;
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  // Clear the matrix
  glLoadIdentity();
  // Set the viewing transformation
  //gluLookAt(-250.0, -250.0, 30.0, -250.0, -250.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  //gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  // Add some slight animation
  /*
  static GLfloat transX = 0.0;
  static GLfloat transY = 0.0;
  static GLfloat transZ = 0.0;
  static bool    adding = true;
  glTranslatef(transX, transY, transZ);
  //glTranslatef(transX, transY, 0.0);
  if (adding)
    {
      transX += 1.0;
      transY += 1.0;
      transZ += 1.0;
      if (transX > 400) adding = false;
    }
  else
    {
      transX -= 1.0;
      transY -= 1.0;
      transZ -= 1.0;
      if (transX < 0.0) adding = true;
    }
  */
  //glScalef(1000.0, 1000.0, 0);
  glScalef(100.0, 100.0, 0);
  //glScalef(1.0, 1.0, 0);
  // Try rotating
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY -= 1.0;
  //drawIcosahedron();
  drawCircle();
  // Flush buffer
  //glFlush(); // If single buffering
  glutSwapBuffers(); // If double buffering
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
    //glViewport(/*(GLsizei)w/2*/0,0/* (GLsizei)h/2*/, (GLsizei)w, (GLsizei)h);
    glOrtho(-1*(GLdouble)w/2, (GLdouble)w/2, -1*(GLdouble)h/2, (GLdouble)h/2, (GLdouble)30, (GLdouble)-w);
    //glFrustum(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)1, (GLdouble)40);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0 ,(GLsizei)w, (GLsizei)h);
}


void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0 / updateRate, timer, 0);
}




    

// Test cases.  Fill in your code for each test case
void Test1()
{
       
  static int pass;
  
  cout << "Displaying pass " << ++pass << endl;
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  // Clear the matrix
  glLoadIdentity();
  // Set the viewing transformation
  //gluLookAt(-250.0, -250.0, 30.0, -250.0, -250.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  //gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  // Add some slight animation
  //glScalef(1000.0, 1000.0, 0);
  glScalef(100.0, 100.0, 0);
  //glScalef(1.0, 1.0, 0);
  DrawIcosahedron(0);
  //drawCircle();
  // Flush buffer
  //glFlush(); // If single buffering
  glutSwapBuffers(); // If double buffering
}

void Test2()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);

  static int pass;
  cout << "Displaying pass " << ++pass << endl;
  // clear all
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Clear the matrix
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  // Set the viewing transformation
  //gluLookAt(-250.0, -250.0, 30.0, -250.0, -250.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  //gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glScalef(100.0, 100.0, 0);
  //glScalef(1.0, 1.0, 0);
  // Try rotating
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY -= 1.0;
  DrawIcosahedron(0);
  //drawCircle();
  // Flush buffer
  glutSwapBuffers(); // If double buffering
  glFlush(); // If single buffering
}

void Test3()
{
       
  static int pass;
  int localDepth = depth;
  
  cout << "Displaying pass " << ++pass << endl;
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  // Clear the matrix
  glLoadIdentity();
  // Set the viewing transformation
  //gluLookAt(-250.0, -250.0, 30.0, -250.0, -250.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  //gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  // Add some slight animation
  //glScalef(1000.0, 1000.0, 0);
  glScalef(100.0, 100.0, 0);
  //glScalef(1.0, 1.0, 0);
  DrawIcosahedron(localDepth);
  //drawCircle();
  // Flush buffer
  //glFlush(); // If single buffering
  glutSwapBuffers(); // If double buffering
}

void Test4()
{
  static int pass;
  int localDepth = depth;
  cout << "Displaying pass " << ++pass << endl;
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  // Clear the matrix
  glLoadIdentity();
  // Set the viewing transformation
  //gluLookAt(-250.0, -250.0, 30.0, -250.0, -250.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // try frustrum
  //gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glScalef(100.0, 100.0, 0);
  //glScalef(1.0, 1.0, 0);
  // Try rotating
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY -= 1.0;
  DrawIcosahedron(localDepth);
  //drawCircle();
  // Flush buffer
  //glFlush(); // If single buffering
  glutSwapBuffers(); // If double buffering
}

void Test5()
{
}

void Test6()
{
}



int main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
  // Set the global test number
  testNumber = atol(argv[1]);

  // Initialize glut  and create your window here
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Icosahedron");
    init();
    //glPushMatrix();

    switch(testNumber) {

        case 1:
            glutDisplayFunc(Test1);
            break;
        case 2:
            glutDisplayFunc(Test2);
            break;
        case 3:
            depth = 1;
            glutDisplayFunc(Test3);
            break;
        case 4:
            depth = 1;
            glutDisplayFunc(Test4);
            break;
        case 5:
            depth = atol(argv[2]);
            glutDisplayFunc(Test5);
            break;
        case 6:
            depth = atol(argv[2]);
            glutDisplayFunc(Test6);
            break;
            

    }



    //glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(1000.0 / updateRate, timer, 0);
   // glPopMatrix();

    // Set your glut callbacks here
  // Enter the glut main loop here
    glutMainLoop();
  return 0;
}

