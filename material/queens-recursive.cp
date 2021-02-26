// Implementation of the eight queens problem
// YOUR NAME HERE

#include "cs1372.h"
#include "qdisplay.h"
#include "RectHelpers.h"
#include <Qt/QPainter.h>
#include <iostream>
#include <sstream>


using namespace std;

const int nrows = 8;
const int ncols = 8;

// For the board, zero means unoccupied and not attacked
// -1 means occupied
// Non-zero is the number of previously placed pieces attacking this space
int board[nrows][ncols];
int nSolutions = 0;

void PrintBoard()
{ // Debug
  for (int r = 0; r < nrows; ++r)
    {
      for (int c = 0; c < ncols; ++c)
        {
          if (board[r][c] < 0)      cout << " Q ";
          else if (board[r][c] > 0) cout << " " << board[r][c] << " ";
          else                      cout << "   ";
        }
      cout << endl;
    }
}

void PrintSolution()
{ // Debug
  for (int r = 0; r < nrows; ++r)
    {
      for (int c = 0; c < ncols; ++c)
        {
          if (board[r][c] < 0)      cout << " Q ";
          else                      cout << " - ";
        }
      cout << endl;
    }
}

  
void Place(int r, int c)
{
  board[r][c] = -1; // Occupied
  for (int c1 = 0; c1 < ncols; ++c1)
    {
      if (board[r][c1] < 0 && c != c1)
        { // this check not necssary, for debug now
          cout << "Oops, found occupied square r " << r << " c " << c1 << endl;
          exit(0);
        }
      if (c != c1) board[r][c1]++; // attacked
    }
  for (int r1 = 0; r1 < nrows; ++r1)
    {
      if (board[r1][c] < 0 && r != r1)
        { // this check not necssary, for debug now
          cout << "Oops, found occupied square r " << r1 << " c " << c << endl;
          exit(0);
        }
      if (r != r1) board[r1][c]++; // attacked
    }
  int r1 = r - 1;
  int c1 = c - 1;
  // upper left diagonal
  while (r1 >= 0 && c1 >= 0)
    {
      board[r1--][c1--]++;
    }
  r1 = r - 1;
  c1 = c + 1;
  // upper right diagonal
  while (r1 >= 0 && c1 < ncols)
    {
      board[r1--][c1++]++;
    }
  r1 = r + 1;
  c1 = c - 1;
  // lower left diagonal
  while (r1 < nrows && c1 >= 0)
    {
      board[r1++][c1--]++;
    }
  r1 = r + 1;
  c1 = c + 1;
  // lower right diagonal
  while (r1 < nrows && c1 < ncols)
    {
      board[r1++][c1++]++;
    }
}

void Remove(int r, int c)
{
  board[r][c] = 0; // Not occupied
  for (int c1 = 0; c1 < ncols; ++c1)
    {
      if (board[r][c1] < 0 && c != c1)
        { // this check not necssary, for debug now
          cout << "Oops, found occupied square r " << r << " c " << c1 << endl;
          exit(0);
        }
      if (c1 != c) board[r][c1]--; // no longer attacked
    }
  for (int r1 = 0; r1 < nrows; ++r1)
    {
      if (board[r1][c] < 0 && r != r1)
        { // this check not necssary, for debug now
          cout << "Oops, found occupied square r " << r1 << " c " << c << endl;
          exit(0);
        }
      if (r1 != r) board[r1][c]--; // no longer attacked
    }
  int r1 = r - 1;
  int c1 = c - 1;
  // upper left diagonal
  while (r1 >= 0 && c1 >= 0)
    {
      board[r1--][c1--]--;
    }
  r1 = r - 1;
  c1 = c + 1;
  // upper right diagonal
  while (r1 >= 0 && c1 < ncols)
    {
      board[r1--][c1++]--;
    }
  r1 = r + 1;
  c1 = c - 1;
  // lower left diagonal
  while (r1 < nrows && c1 >= 0)
    {
      board[r1++][c1--]--;
    }
  r1 = r + 1;
  c1 = c + 1;
  // lower right diagonal
  while (r1 < nrows && c1 < ncols)
    {
      board[r1++][c1++]--;
    }
}

      
static int saveIndex = 0;

void DisplayBoard(QDisplay& d, int row, int col)
{
  QPainter p(d.Pixmap());
  QRect r(0, 0, d.width(), d.height());
  EraseRect(p, r);
  // Find height of Q letter
  QRect txt = p.boundingRect(r, Qt::AlignCenter, "Q");
  unsigned h = txt.height() + txt.height()/2; // Allow 1/4 margin top/bottom
  // Draw horizontal and vertical lines
  unsigned y = h/2; // Leave room for column arrow
  unsigned x = h/2; // And left margin
  
  for (int y1 = 0; y1 < 9; ++y1)
    { // Horizontal lines
      p.drawLine(x, y, x + 8 * h, y);
      y += h;
    }
  y = h/2;
  for (int x1 = 0; x1 < 9; ++x1)
    { // Vertical lines
      p.drawLine(x, y, x, y + 8 * h);
      x += h;
    }
  if (col >= 0)
    {
      // Draw the column arrow
      unsigned xc = h/2 + h/2 + col * h;
      unsigned yc = h/2;
      p.drawLine(xc, 0, xc, yc);
      p.drawLine(xc, yc, xc - 4, yc - 4);
      p.drawLine(xc, yc, xc + 4, yc - 4);
    }
  if (row >= 0)
    {
      // Draw the row arrow
      unsigned xc = h/2;
      unsigned yc = h/2 + h/2 + row * h;
      p.drawLine(0, yc, xc, yc);
      p.drawLine(xc, yc, xc - 4, yc - 4);
      p.drawLine(xc, yc, xc - 4, yc + 4);
    }
  
  // Now draw the Queen icons
  for (int c = 0; c < ncols; ++c)
    {
      for (int r = 0; r < nrows; ++r)
        {
          QRect r1(0, 0, h-2, h-2);
          r1 = MoveRectRightBy(r1, (c) * h + h/2 + 1);
          r1 = MoveRectDownBy(r1, (r) * h + h/2 + 1);
          if (board[r][c] < 0)
            { // Place a queen here
              p.drawText(r1, Qt::AlignCenter, "Q");
            }
          else if (board[r][c] > 0)
            { // Square is attacked, grey out
              QBrush grayBrush(QColor(0xc0, 0xc0, 0xc0));
              p.fillRect(r1, grayBrush);
            }
          else if (r < row && c == col)
            { // Already checked this square, draw a small x
              p.drawText(r1, Qt::AlignCenter, "x");
            }
        }
    }
  d.Update();
  // Save the image
  ostringstream oss;
  oss.width(5);
  oss.fill('0');
  oss << saveIndex++;
  string sn = "Queens" + oss.str() + ".png";
  //string sn = "Solutions" + oss.str() + ".png";
  d.Pixmap()->save(sn.c_str());
}

          
  
void Try(QDisplay& d, int c)
{
  if (c == ncols)
    { // Solution found
      nSolutions++;
      cout << "Found solution number " << nSolutions 
           << " on frame " << saveIndex
           << endl;
      PrintSolution();
      DisplayBoard(d, -1, -1);
      return;
    }
  
  for (int r = 0; r < nrows; ++r)
    {
      DisplayBoard(d, r, c);
      if (board[r][c] == 0)
        { // We can place on this square
          //cout << "Placing square r "  << r << " c " << c << endl;
          Place(r,c);
          DisplayBoard(d, r, c);
          //PrintBoard();
          //cout << endl << endl;
          Try(d, c + 1);
          Remove(r,c);
          //DisplayBoard(d, r, c);
        }
    }
  DisplayBoard(d, nrows, c); // Show we advance row counter off end
}

int main(int argc, char** argv)
{
  QApp app(argc, argv);
  QDisplay d(app);
  d.BlankPixmap(256, 256);
  d.Show();
  Try(d, 0);
  cout << "Found " << nSolutions << " solutions" << endl;
}

