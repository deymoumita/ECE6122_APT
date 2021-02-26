/* 
 * File:   MBSet.cu
 * 
 * Created on June 24, 2012
 * 
 * Purpose:  This program displays Mandelbrot set using the GPU via CUDA and
 * OpenGL immediate mode.
 * 
 */

#include <iostream>
#include <stack>
#include <cuda_runtime_api.h>
#include <stdio.h>
#include "Complex.cu"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

// Size of window in pixels, both width and height
#define WINDOW_DIM 512
#define THREADSPERBLOCK 16
#define NUMBLOCKS (WINDOW_DIM*WINDOW_DIM/THREADSPERBLOCK)

// all variables
// Min and max complex plane values
Complex  minC(-2.0, -1.2);
Complex  maxC( 1.0, 1.8);
int maxIt = 2000;     // Max iterations for the set computations
float aspect;
int win_id;
int N;
bool button_down = false;
float x_down, y_down, x_up, y_up, x_current, y_current;
bool selectionDone = false;
Complex new_minC(minC.r, minC.i);
Complex new_maxC(maxC.r, maxC.i);
bool drawBox_flag = false;

vector<float> color1;
vector<float> color2;
vector<float> color3;
int MBdata [262144];
vector<float> minCdata_r;
vector<float> minCdata_i;
vector<float> maxCdata_r;
vector<float> maxCdata_i;
int *dev_MBdata;  // device copy

// function declarations
void display(void);
void init();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char c, int x, int y);
void thread_create();
void drawBox();
void InitializeColors();

void InitializeColors()
{
srand48(time(NULL));
  for(int i = 0; i < 2000; i++)
  { 
    color1.push_back(drand48());
    color2.push_back(drand48());
    color3.push_back(drand48());
  }  
}

void thread_create()
{
  cudaMalloc( (void**)&dev_MBdata, WINDOW_DIM*WINDOW_DIM*sizeof(int));
  generateMB<<< NUMBLOCKS, THREADSPERBLOCK >>>(new_minC, new_maxC, dev_MBdata, maxIt);
}


void display(void)
{
 // Your OpenGL display code here
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();


  int MBdata_cell = 0;
  for (int i = 0; i < winH; i++)
  {
    for (int j = 0; j < winW; j++)
    {
      MBdata_cell = MBdata[i * winW + j];
      if(MBdata_cell != maxIt && MBdata_cell > 5)
      {
        //not a part of Mandelbrot Set
        glColor3f(color1[MBdata_cell], color2[MBdata_cell], color3[MBdata_cell]); //random color
        glBegin(GL_POINTS); //starts drawing of points
        glVertex2f(j, winW - i);
        glEnd();//end drawing of points       
      }
      else if (MBdata_cell <= 5)
      {
        //not a part of Mandelbrot Set
        glColor3f(1.0, 1.0, 1.0); //white color
        glBegin(GL_POINTS); //starts drawing of points
        glVertex2f(j, winW - i);
        glEnd();//end drawing of points         
      }
      else
      {
        glColor3f(0.0, 0.0, 0.0); //black color
        glBegin(GL_POINTS); //starts drawing of points
        glVertex2f(j, winW - i);
        glEnd();//end drawing of points           
      } 

      // if(j < 5)
      // {
      //  glColor3f(1.0, 0.0, 0.0); //black color
      //  glBegin(GL_POINTS); //starts drawing of points
      //  glVertex2f(j, winW - i);
      //  glEnd();//end drawing of points         
      // }  
      // if(i < 5)
      // {
      //  glColor3f(0.0, 0.0, 1.0); //black color
      //  glBegin(GL_POINTS); //starts drawing of points
      //  glVertex2f(j, winW - i);
      //  glEnd();//end drawing of points 
      // }  
    }
  }

  if(drawBox_flag)
  {
    drawBox();
  }

  glutSwapBuffers(); // If double buffering  
}

void drawBox()
{
  drawBox_flag = false;
  glColor3f(1.0, 0.0, 0.0); //red color
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP); //starts drawing of points
  glVertex2f(x_down, winW - y_down);
  glVertex2f(x_down, winW - y_current);
  glVertex2f(x_current, winW - y_current);
  glVertex2f(x_current, winW - y_down);
  glEnd();

}

void reshape(int w, int h)
{ // Your OpenGL window reshape code here
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  int width = h * aspect;
  int left = (w - width) / 2; 
  glViewport(left, 0, width, (GLsizei)h);
  glOrtho(0.0, winW, 0.0, winH, -winW, winW);
  glMatrixMode(GL_MODELVIEW); 
  glutPostRedisplay();

}

void mouse(int button, int state, int x, int y)
{ // Your mouse click processing here
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.

  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    selectionDone = false;
    button_down = true;
    x_down = x;
    y_down = y;

    // cout<<x_down<<" "<<y_down<<"\t";
  }
  if((button_down == true) && (button == GLUT_LEFT_BUTTON && state == GLUT_UP))
  {
    button_down = false;
    x_up = x;
    y_up = y;
    selectionDone = true;
    // cout<<x_up<<" "<<y_up<<endl;
  } 
  if(selectionDone == true)
  {

    // first click top left
    if ((x_down < x_up) && (y_down < y_up))
    {
      float delta = min((x_up - x_down), (y_up - y_down));
      x_up = x_down + delta;
      y_up = y_down + delta;
    }
    // first click top right
    else if ((x_down > x_up) && (y_down < y_up))
    {
      float delta = min((x_down - x_up), (y_up - y_down));
      x_up = x_down - delta;
      y_up = y_down + delta;
      int temp_x_down = x_up;
      int temp_x_up = x_down;
      x_down = temp_x_down;
      x_up = temp_x_up;
    }
    // first click bottom right
    else if ((x_down > x_up) && (y_down > y_up))
    {
      float delta = min((x_down - x_up), (y_down - y_up));
      x_up = x_down - delta;
      y_up = y_down - delta;
      int temp_x_down = x_up;
      int temp_x_up = x_down;
      int temp_y_down = y_up;
      int temp_y_up = y_down;
      x_down =  temp_x_down;
      x_up = temp_x_up;
      y_down = temp_y_down;
      y_up = temp_y_up;
    }
    // first click bottom left
    else if((x_down < x_up) && (y_down > y_up))
    {
      float delta = min((x_up - x_down), (y_down - y_up));
      x_up = x_down + delta;
      y_up = y_down - delta;
      int temp_y_down = y_up;
      int temp_y_up = y_down;
      y_up = temp_y_up;
      y_down = temp_y_down;
    }

    // x_down = 0;
    // y_down = 0;
    // x_up = 511;
    // y_up = 511;

    float x_down_scaled = new_minC.r + (float)x_down*(new_maxC.r - new_minC.r)/(float)winW;
    float x_up_scaled = new_minC.r + (float)x_up*(new_maxC.r - new_minC.r)/(float)winW;
    float y_down_scaled = new_minC.i + (float)y_down*(new_maxC.i - new_minC.i)/(float)winW;
    float y_up_scaled = new_minC.i + (float)y_up*(new_maxC.i - new_minC.i)/(float)winW;

    // cout<<x_down<<"\t"<<x_down_scaled<<"  "<<endl;
    // cout<<y_down<<"\t"<<y_down_scaled<<"  "<<endl;
    // cout<<x_up<<"\t"<<x_up_scaled<<"  "<<endl;
    // cout<<y_up<<"\t"<<y_up_scaled<<"  "<<endl;

    minCdata_r.push_back(new_minC.r);
    minCdata_i.push_back(new_minC.i);

    maxCdata_r.push_back(new_maxC.r);
    maxCdata_i.push_back(new_maxC.i);

    new_minC.r = x_down_scaled;
    new_minC.i = y_down_scaled;
    new_maxC.r = x_up_scaled;
    new_maxC.i = y_up_scaled;

    // cout<<endl<<"final values: "<<new_minC.r<<" "<<new_minC.i<<"\t"<<new_maxC.r<<" "<<new_maxC.i<<endl;
    selectionDone = false;
    drawBox_flag = false;

      thread_create();
        
    display();
    glutPostRedisplay();
  }

}

void motion(int x, int y)
{
  drawBox_flag = false;
  if(button_down)
  {
    x_current = x;
    y_current = y;
    // first click top left
    if ((x_down < x_current) && (y_down < y_current))
    {
      float delta = min((x_current - x_down), (y_current - y_down));
      x_current = x_down + delta;
      y_current = y_down + delta;
    }
    // first click top right
    else if ((x_down > x_current) && (y_down < y_current))
    {
      float delta = min((x_down - x_current), (y_current - y_down));
      x_current = x_down - delta;
      y_current = y_down + delta;
    }
    // first click bottom right
    else if ((x_down > x_up) && (y_down > y_up))
    {
      float delta = min((x_down - x_current), (y_down - y_current));
      x_current = x_down - delta;
      y_current = y_down - delta;
    }
    // first click bottom left
    else if((x_down < x_current) && (y_down > y_current))
    {
      float delta = min((x_current - x_down), (y_down - y_current));
      x_current = x_down + delta;
      y_current = y_down - delta;
    }
    drawBox_flag = true;
    // display();
    glutPostRedisplay();
  }

  
}

void keyboard(unsigned char c, int x, int y)
{
  if (c == 'b')

  {
    if(minCdata_i.size())
    {
      glutPostRedisplay();
      new_minC.r = minCdata_r.back();
      new_minC.i = minCdata_i.back();
      new_maxC.r = maxCdata_r.back();
      new_maxC.i = maxCdata_i.back();
      minCdata_r.pop_back();
      minCdata_i.pop_back();
      maxCdata_r.pop_back();
      maxCdata_i.pop_back();

        thread_create();
      display();
    }
  }

  else if (c == 'e')
  {
    glutDestroyWindow(win_id);
    exit(0);    
  }
  
}

void init()
{ // Your OpenGL initialization code here
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glShadeModel(GL_FLAT);
}


__global__ void generateMB(Complex new_minC, Complex new_maxC, int* dev_MBdata, int maxIt)
{
  int index = threadId.x + blockIdx * blockDim.x;
  int i = index % WINDOW_DIM;
  int j = index / WINDOW_DIM;

  Complex c;  

  float c_r = new_minC.r + (float)j*(new_maxC.r - new_minC.r)/(float)winW;
  float c_i = new_minC.i + (float)i*(new_maxC.i - new_minC.i)/(float)winH;

  c.r = c_r;
  c.i  = c_i;
  Complex z = Complex(c.r, c.i);
  int iter_no = 0;

  while((z.magnitude2() < 4.0) && (iter_no < maxIt))
  {
    z = z * z + c;
    iter_no++;
  }

  dev_MBdata[i * winW + j] = iter_no;
}

int main(int argc, char** argv)
{
  winW = 512;
  winH = 512; 
  N = winW;
  aspect = (float)winW/(float)winH;
  
  // initialize colors
  InitializeColors();

  pthread_barrier_init(&barrier, NULL, nThreads);     // Initializing the barrier

    thread_create();

  // Initialize OpenGL, but only on the "master" thread or process.
  // See the assignment writeup to determine which is "master" 
  // and which is slave.
  // pthread_barrier_wait(&barrier);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(winW, winH);
  glutInitWindowPosition(300, 100);
  
  string title = "Mandelbrot Set";
  win_id = glutCreateWindow(title.c_str());
  init();


  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);

  glutMainLoop();
  
  return 0;
  
}
