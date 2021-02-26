// Calculate and display the Mandelbrot set
// ECE4893/8893 final project, Fall 2011

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <vector>
#include <cmath>

#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "complex.h"

using namespace std;

// Min and max complex plane values
Complex  minC(-2.0, -1.2);
Complex  maxC( 1.0, 1.8);
int maxIt = 2000;     // Max iterations for the set computations
int winW, winH, aspect;
int win_id;
int nThreads = 16; 
int N;
pthread_barrier_t barrier;
bool button_down = false;
float x_down, y_down, x_up, y_up, x_current, y_current;
bool selectionDone = false;
Complex new_minC(minC.real, minC.imag);
Complex new_maxC(maxC.real, maxC.imag);
bool drawBox_flag = false;
pthread_mutex_t exitMutex;	// mutex to communicate to main thread
pthread_cond_t exitCond;	// condition to communicate to main thread


vector<float> color1;
vector<float> color2;
vector<float> color3;
int MBdata [262144];
vector<float> minCdata_real;
vector<float> minCdata_imag;
vector<float> maxCdata_real;
vector<float> maxCdata_imag;

// function declarations
void generateMB(int rowsPerThread, int startingRow);
void display(void);
void init();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char c, int x, int y);
void thread_create();
void drawBox();


void display(void)
{ // Your OpenGL display code here
	
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
			// 	glColor3f(1.0, 0.0, 0.0); //black color
			// 	glBegin(GL_POINTS); //starts drawing of points
			// 	glVertex2f(j, winW - i);
			// 	glEnd();//end drawing of points					
			// }	
			// if(i < 5)
			// {
			// 	glColor3f(0.0, 0.0, 1.0); //black color
			// 	glBegin(GL_POINTS); //starts drawing of points
			// 	glVertex2f(j, winW - i);
			// 	glEnd();//end drawing of points	
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

		float x_down_scaled = new_minC.real + (float)x_down*(new_maxC.real - new_minC.real)/(float)winW;
		float x_up_scaled = new_minC.real + (float)x_up*(new_maxC.real - new_minC.real)/(float)winW;
		float y_down_scaled = new_minC.imag + (float)y_down*(new_maxC.imag - new_minC.imag)/(float)winW;
		float y_up_scaled = new_minC.imag + (float)y_up*(new_maxC.imag - new_minC.imag)/(float)winW;

		// cout<<x_down<<"\t"<<x_down_scaled<<"  "<<endl;
		// cout<<y_down<<"\t"<<y_down_scaled<<"  "<<endl;
		// cout<<x_up<<"\t"<<x_up_scaled<<"  "<<endl;
		// cout<<y_up<<"\t"<<y_up_scaled<<"  "<<endl;

		minCdata_real.push_back(new_minC.real);
		minCdata_imag.push_back(new_minC.imag);

		maxCdata_real.push_back(new_maxC.real);
		maxCdata_imag.push_back(new_maxC.imag);

		new_minC.real = x_down_scaled;
		new_minC.imag = y_down_scaled;
		new_maxC.real = x_up_scaled;
		new_maxC.imag = y_up_scaled;

		// cout<<endl<<"final values: "<<new_minC.real<<" "<<new_minC.imag<<"\t"<<new_maxC.real<<" "<<new_maxC.imag<<endl;
		selectionDone = false;
		drawBox_flag = false;

	   	thread_create();
				
		display();
		glutPostRedisplay();
	}
  
}

void motion(int x, int y)
{ // Your mouse motion here, x and y coordinates are as above
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
{ // Your keyboard processing here
	if (c == 'b')

	{
		if(minCdata_imag.size())
		{
			glutPostRedisplay();
			new_minC.real = minCdata_real.back();
			new_minC.imag = minCdata_imag.back();
			new_maxC.real = maxCdata_real.back();
			new_maxC.imag = maxCdata_imag.back();
			minCdata_real.pop_back();
			minCdata_imag.pop_back();
			maxCdata_real.pop_back();
			maxCdata_imag.pop_back();

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

void* generateMB(void* v)
{
	unsigned long myId = (unsigned long)v;
	int rowsPerThread = N / nThreads;
	int startingRow = myId * rowsPerThread;
	Complex c;	

	for(int i = startingRow; i < startingRow + rowsPerThread; i++)
	{	
		for(int j = 0; j < winW; j++)
		{
			float c_real = new_minC.real + (float)j*(new_maxC.real - new_minC.real)/(float)winW;
			float c_imag = new_minC.imag + (float)i*(new_maxC.imag - new_minC.imag)/(float)winH;

			c.real = c_real;
			c.imag  = c_imag;
			Complex z = Complex(c.real, c.imag);
			int iter_no = 0;

			while((z.Mag2() < 4.0) && (iter_no < maxIt))
			{
				z = z * z + c;
				iter_no++;
			}

			MBdata[i * winW + j] = iter_no;
		}
	}
	pthread_barrier_wait(&barrier);
	if (myId == 0)
	{	
		pthread_mutex_lock(&exitMutex);
		pthread_cond_signal(&exitCond);
		pthread_mutex_unlock(&exitMutex);
	}

}

void thread_create()
{
	for(int i = 0; i < nThreads; i++)
	{
		pthread_t pt;
		pthread_create(&pt, 0, generateMB, (void*)i);
	}
	pthread_cond_wait(&exitCond, &exitMutex);
}

int main(int argc, char** argv)
{

	winW = 512;
	winH = 512;	
	N = winW;
	aspect = (float)winW/(float)winH;
	
	for(int i = 0; i < 2000; i++)
	{	
		srand(time(NULL) * (i + 1) / 13);
		color1.push_back((rand() % 100) / 100.0);
		srand(time(NULL) * (i + 2) / 53);
		color2.push_back((rand() % 100) / 100.0);
		srand(time(NULL) * (i + 7) / 23);
		color3.push_back((rand() % 100) / 100.0);
	}

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

