// Main program for model/view/controller example using Tic Tac Toe
// George F. Riley, Georgia Tech, Fall 2011

#ifdef OSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "ttt.h"

int main(int argc, char** argv)
{ 
  // Instantiate the model, view and controller
  TTTModel*      m = new TTTModel();
  TTTController* c = new TTTController(m);
  TTTView*       v = new TTTView(argc, argv, m, c);
  // Opengl main loop
  glutMainLoop();
}

