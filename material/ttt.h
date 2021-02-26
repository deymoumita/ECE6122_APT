// Demonstrate the Model/View/Controller design pattern using TicTacToe
// George F. Riley, Georgia Tech, Fall 2011

// Define three classes for the Tic Tac Toe game
// 1) The Model class keeps track of the state of the board
// 2) The View class displays the model
// 3) The controller responds to actions and updates the model as needed.

#define N_SQUARE 9
#define N_ROW    3
#define N_COL    3
#define N_DIAG   2

class TTTController;
class TTTModel 
{
public:
  TTTModel();
  void Move(int square, char player);
  bool LegalMove(int square);   // True of square is empty and game not over
  bool GameOver();              // True if game is over
  void MoveRow(int r);          // Move 'o' to empty cell on row r
  void MoveCol(int c);          // Move 'o' to empty cell on col c
  void MoveDiag(int d);         // Move 'o' to empty cell on diag d
  void Clear();                 // New game
  int  GetSquare(int r, int c); // Get square number from row/col
  int  GetCol(int s);           // Get col number from square
  int  GetRow(int s);           // Get row number from square
  // Helpers for controller
  int  FindEmptyRow(int c);     // Find empty row in specified col
  int  FindEmptyCol(int r);     // Find empty col in specified row
  int  FindEmptyDiag(int d);    // Find empty cell in specified diag
  int  FindEmptyCorner();       // Find an empty corner
  int  FindEmptySide();         // Find empty side
public:
  // Maintain the board in several different representations
  // to ease the next move decision process
  int  nMoves;             // Total number of squares occupied
  int  xRowCount[N_ROW];   // number x moves on each row
  int  oRowCount[N_ROW];   // Number o moves on each row
  int  xColCount[N_COL];   // number x moves on each col
  int  oColCount[N_COL];   // Number o moves on each col
  int  xDiagCount[N_DIAG]; // number x moves on each diag
  int  oDiagCount[N_DIAG]; // Number o moves on each diag
  char board[N_SQUARE];    // State of board
  bool draw;               // True if draw
  bool xWins;              // True if x wins
  bool oWins;              // True if o wins
  int  winRow;             // Row number of winning row
  int  winCol;             // Col number of winning col
  int  winDiag;            // Diag number of winning diag
};

class TTTView 
{ // Resposible for displaying the board 
public:
  // Since the view uses OpenGL and responds to mouse clicks
  // the view needs to be aware of both the model (to update the board)
  // and the controller to determine next move.  Also needs
  // argc and argv for opengl init
  TTTView(int argc, char** argv, TTTModel*, TTTController*);
public:
  // Static member functions and variables
  static void reshape(int, int); // Called by GLUT
  static void display(); // Called by GLUT
  static void mouse(int, int, int, int); // Also called by glut
  static TTTModel*      model;
  static TTTController* controller;
  static int winW;    // Window width and height
  static int winH;
  static int squareW; // Width of each square
  static int squareH; // Height of each square
};

class TTTController 
{
public:
  // Controller needs access to the model to determine next move
  TTTController(TTTModel*);
  void Move(int square);   // Process player (x) move
public:
  TTTModel* model;
private:
  // Move helpers
  void Response1();       // Choose 'o' move in response to first move
  void Response2();       // Choose 'o' move in response to players second move
  void ResponseOther();   // Choose 'o' move after later moves
  int  BlockingMove();    // Choose 'o' to block player's win
};

