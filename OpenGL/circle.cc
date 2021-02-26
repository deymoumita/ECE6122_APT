// given code

// Draw simple circle
// George F. Riley, Georgia Tech, Fall  2011

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

static const float DEG2RAD = M_PI/180;
static int updateRate = 50;

void drawCircle()
{
  // Draw a circle by drawing 360 small lines
  GLfloat radius = 1.0; // 1 pixel radius
  glColor3f(1.0, 1.0, 1.0);	//line color
  glLineWidth(1.0);	// line width
  glBegin(GL_LINE_LOOP); //GL_LINE_LOOP=loop back the last point of the last line to the first point of the first line
  for (double i = 0; i < 360; i++)
    {
      float degInRad = i*DEG2RAD;
      glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
   }
  glEnd();
}

void display(void)
{
  static int pass;
 	int w = 500;
	int h = 500; 
  cout << "Displaying pass " << ++pass << endl;
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  // Clear the matrix
  glLoadIdentity();
  // Set the viewing transformation
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  //glTranslatef(250, 250, 0);
  glTranslatef(w/2, h/2, 0);

  //glScalef(100.0, 100.0, 0);
  glScalef(w/2, h/2, 0);	// make it bigger 
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
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)-w, (GLdouble)w);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0 / updateRate, timer, 0);
}

int main(int argc, char** argv)
{
  if (argc > 1) updateRate = atof(argv[1]);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Circle");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  glutMainLoop();
  return 0;
}
