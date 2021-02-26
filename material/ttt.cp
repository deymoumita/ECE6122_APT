// Implement the model, view, and controller for the Tic Tac Toe game
// George F. Riley, Georgia Tech, Fall 2011

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <iostream>
#include <math.h>
#include "ttt.h"

using namespace std;

// Model implementation
TTTModel::TTTModel()
{
  Clear();
}

void TTTModel::Move(int square, char player)
{ // returns true if legal move
  int r = square / N_COL;
  int c = square % N_ROW;
  if (player == 'x')
    { // Make the moves
      xRowCount[r]++;
      xColCount[c]++;
      if (square == 0 || square == 4 || square == 8) xDiagCount[0]++;
      if (square == 2 || square == 4 || square == 6) xDiagCount[1]++;
      if (xRowCount[r] == N_ROW || xColCount[c] == N_COL || 
          xDiagCount[0] == 3 || xDiagCount[1] == 3)
        {
          xWins = true;
          if (xRowCount[r] == N_ROW) winRow = r;
          if (xColCount[r] == N_COL) winCol = c;
          if (xDiagCount[0] == 3) winDiag = 0;
          if (xDiagCount[1] == 3) winDiag = 1;
          cout << "X Wins!" << endl;
        }
    }
  if (player == 'o')
    { // Make the moves
      oRowCount[r]++;
      oColCount[c]++;
      if (square == 0 || square == 4 || square == 8) oDiagCount[0]++;
      if (square == 2 || square == 4 || square == 6) oDiagCount[1]++;
      if (oRowCount[r] == N_ROW || oColCount[c] == N_COL || 
          oDiagCount[0] == 3 || oDiagCount[1] == 3)
        {
          oWins = true;
          if (oRowCount[r] == N_ROW) winRow = r;
          if (oColCount[r] == N_COL) winCol = c;
          if (oDiagCount[0] == 3) winDiag = 0;
          if (oDiagCount[1] == 3) winDiag = 1;
          cout << "O Wins!" << endl;
        }
    }
  // Update board character and count moves
  board[square] = player;
  nMoves++;
  if (!xWins && !oWins && (nMoves == N_SQUARE)) draw = true;
  if (draw) cout << "Draw!" << endl;
}

bool TTTModel::LegalMove(int square)
{ // Determine if legal move
  if (board[square] != ' ') return false;
  if (draw || oWins || xWins) return false;
  return true;
}

bool TTTModel::GameOver()
{
  return draw || xWins || oWins;
}

void TTTModel::Clear()
{ // Set up a new game
  nMoves = 0;
  for (int i = 0; i < N_SQUARE; ++i) board[i] = ' ';
  for (int i = 0; i < N_ROW; ++i) 
    {
      xRowCount[i] = 0;
      oRowCount[i] = 0;
    }
  for (int i = 0; i < N_COL; ++i) 
    {
      xColCount[i] = 0;
      oColCount[i] = 0;
    }
  for (int i = 0; i < N_DIAG; ++i) 
    {
      xDiagCount[i] = 0;
      oDiagCount[i] = 0;
    }
  draw = false;
  xWins = false;
  oWins = false;
  winRow = -1;
  winCol = -1;
  winDiag = -1;
}

int TTTModel::GetSquare(int r, int c)
{
  return r * N_COL + c;
}

// Model helpers
int TTTModel::FindEmptyRow(int c)
{
  for (int r = 0; r < N_ROW; ++r)
    {
      int s = GetSquare(r, c);
      if (board[s] == ' ') return s;
    }
  return -1; // Not found (should never happen)
}

int TTTModel::FindEmptyCol(int r)
{
  for (int c = 0; c < N_COL; ++c)
    {
      int s = GetSquare(r, c);
      if (board[s] == ' ') return s;
    }
  return -1; // Not found, should never happen
}

int TTTModel::FindEmptyDiag(int d)
{
  if (d == 0)
    { // square 0, 4, or 8
      if (board[0] == ' ') return 0;
      if (board[4] == ' ') return 4;
      if (board[8] == ' ') return 8;
    }
  else
    { // square 2, 4 or 6
      if (board[2] == ' ') return 2;
      if (board[4] == ' ') return 4;
      if (board[6] == ' ') return 6;
    }
  return -1;
}

int TTTModel::FindEmptyCorner()
{
  if (board[0] == ' ') return 0;
  if (board[2] == ' ') return 2;
  if (board[6] == ' ') return 6;
  if (board[8] == ' ') return 8;
}

int TTTModel::FindEmptySide()
{
  if (board[1] == ' ') return 1;
  if (board[3] == ' ') return 3;
  if (board[5] == ' ') return 5;
  if (board[7] == ' ') return 7;
}


      
// Implement the view

// Firt the static variables
TTTModel*      TTTView::model = 0;
TTTController* TTTView::controller = 0;
int            TTTView::winW = 300;
int            TTTView::winH = 300;
int            TTTView::squareW = winW / N_COL;
int            TTTView::squareH = winH / N_ROW;

TTTView::TTTView(int argc, char** argv, TTTModel* m, TTTController* c)
{
  model = m;       // Save the model pointer
  controller = c;  // Save the controller pointer
  winW = 300;      // Window width and height
  winH = 300;
  squareW = winW / N_COL;
  squareH = winH / N_ROW;

  // Initialize OpenGL
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(winW, winH);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Tic Tac Toe");
  glClearColor(1.0, 1.0, 1.0, 0.0);   // white background
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutReshapeFunc(reshape);
}

void TTTView::reshape(int w, int h)
{
  winW = w;
  winH = h;
  squareW = winW / N_COL;
  squareH = winH / N_ROW;
  
  glViewport(0, 0, (GLsizei)winW, (GLsizei)winH );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, winW, winH, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void TTTView::display()
{ // Display the board
  // First clear
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(0, 0, 0);       // Black lines
  glLineWidth(3);           // SLightly thick lines
  
  // Dividing liens
  glBegin(GL_LINES);
  for (int v = 1; v < N_ROW; ++v)
    {
      GLint x = TTTView::winW / 3 * v;
      GLint y = TTTView::winH / 50;
      glVertex2i(x, y);
      glVertex2i(x, TTTView::winH - y);
    }
  for (int v = 1; v < N_COL; ++v)
    {
      GLint x = TTTView::winW / 50;
      GLint y = TTTView::winH / 3 * v;
      glVertex2i(x, y);
      glVertex2i(TTTView::winW - x, y);
    }
  glEnd();
  // Now the occupied sauares
  for (int s = 0; s < N_SQUARE; ++s)
    {
      int r = s / N_COL;
      int c = s % N_ROW;
      glPushMatrix();
      // Move to center of square
      glTranslatef(c * squareW + squareW / 2,
                   r * squareH + squareH / 2,
                   0);
      if (model->board[s] == 'x')
        { // Draw the x
          glColor3f(1, 0, 0); // red
          int w2 = squareW / 2;
          int h2 = squareH / 2;
          int x0 = w2 - w2 / 10;
          int x1 = -x0;
          int y0 = h2 - h2 / 10;
          int y1 = -y0;
          glBegin(GL_LINES);
          glVertex2i(x0, y0);
          glVertex2i(x1, y1);
          glVertex2i(x1, y0);
          glVertex2i(x0, y1);
          glEnd();
        }
      else if (model->board[s] == 'o')
        { // Draw the o
          int radius = squareW / 2 - squareW / (2 * 10);
          glColor3f(0, 0, 1); // blue
          glBegin(GL_LINE_LOOP);
          for (int i = 0; i < 360; ++i)
            {
              double radians = (double)i / 360.0 * 2.0 * M_PI;
              double x1 = cos(radians) * radius;
              double y1 = sin(radians) * radius;
              glVertex2d(x1, y1);
            }
          glEnd();
        }
      glPopMatrix();
    }
  // See if winning row/col/diag
  if (model->winRow >= 0)
    {
      int x0 = squareW / 10;
      int x1 = squareW * N_COL - squareW / 10;
      int y = squareH / 2 + model->winRow * squareW;
      glLineWidth(5);
      glColor4f(0.5, 0.5, 0.5, 0.5);
      glBegin(GL_LINES);
      glVertex2i(x0, y);
      glVertex2i(x1, y);
      glEnd();
    }
  if (model->winCol >= 0)
    {
      int y0 = squareH / 10;
      int y1 = squareH * N_ROW - squareH / 10;
      int x = squareW / 2 + model->winCol * squareH;
      glLineWidth(5);
      glColor4f(0.5, 0.5, 0.5, 0.5);
      glBegin(GL_LINES);
      glVertex2i(x, y0);
      glVertex2i(x, y1);
      glEnd();
    }
  glFlush();
}

void TTTView::mouse(int button, int state, int x, int y)
{
  if (button == 2) 
    {
      model->Clear();
      glutPostRedisplay();
      return;
    }
  if (button == 0 && state == 0)
    { // Pressed, find which square
      int c = x / squareW;
      int r = y / squareH;
      // Player move is always x
      controller->Move(r * N_COL + c);
    }
}

// Implement the controller
TTTController::TTTController(TTTModel* m)
{
  model = m;
}

void TTTController::Move(int square)
{
  // Do nothing of not legal
  if (!model->LegalMove(square)) return;
  // First note the player's move
  model->Move(square, 'x');
  if (!model->GameOver())
    {
      if (model->nMoves == 1)
        {
          Response1();
        }
      else if (model->nMoves == 3)
        {
          Response2();
        }
      else
        {
          ResponseOther();
        }
    }
  glutPostRedisplay();
}

void TTTController::Response1()
{ // Respond to first player move is simple.
  // If he takes middle, take corner, otherwise take middle
  if (model->board[4] == ' ')
    {
      model->Move(4, 'o');
    }
  else
    {
      model->Move(0, 'o');
    }
}

void TTTController::Response2()
{ // Respond to players first move
  // Make sure we don't need a blocking move
  int s = BlockingMove();
  if (s >= 0)
    {
      model->Move(s, 'o');
      return;
    }
  // If either diag has 2 x's, we must take a side
  if (model->xDiagCount[0] == 2 || model->xDiagCount[1] == 2)
    {
      int s = model->FindEmptySide();
      model->Move(s, 'o');
      return;
    }
  // Else take a corner
  s = model->FindEmptyCorner();
  model->Move(s, 'o');
  // This likely needs more work
}

void TTTController::ResponseOther()
{ // Respond to players thrid and beyond moves
  // First see if we have a winning move            
  for (int r = 0; r < N_ROW; ++r)
    {
      if (model->oRowCount[r] == 2 && model->xRowCount[r] == 0)
        {
          int s = model->FindEmptyCol(r);
          model->Move(s, 'o');
          return;
        }
    }
  for (int c = 0; c < N_COL; ++c)
    {
      if (model->oColCount[c] == 2 && model->xColCount[c] == 0)
        {
          int s = model->FindEmptyRow(c);
          model->Move(s, 'o');
          return;
        }
    }
  for (int d = 0; d < N_DIAG; ++d)
    {
      if (model->oDiagCount[d] == 2 && model->xDiagCount[d] == 0)
        {
          int s = model->FindEmptyDiag(d);
          model->Move(s, 'o');
          return;
        }
    }
  // See if we need to block player's winning move
  int s = BlockingMove();
  if (s >= 0)
    {
      model->Move(s, 'o');
      return;
    }
  // No immediate needs; choose a corner if availble else take a side
  s = model->FindEmptyCorner();
  if (s >= 0)
    {
      model->Move(s, 'o');
      return;
    }
  s = model->FindEmptySide();
  // No need to check -1 here as must be available
  model->Move(s, 'o');
}

int TTTController::BlockingMove()
{ // See if blocking move needed
  for (int r = 0; r < N_ROW; ++r)
    {
      if (model->xRowCount[r] == 2 && model->oRowCount[r] == 0)
        {
          int s = model->FindEmptyCol(r);
          return s;
        }
    }
  for (int c = 0; c < N_COL; ++c)
    {
      if (model->xColCount[c] == 2 && model->oColCount[c] == 0)
        {
          int s = model->FindEmptyRow(c);
          return s;
        }
    }
  for (int d = 0; d < N_DIAG; ++d)
    {
      if (model->xDiagCount[d] == 2 && model->oDiagCount[d] == 0)
        {
          int s = model->FindEmptyDiag(d);
          return s;
        }
    }
  return -1; // No blocking move found
}
