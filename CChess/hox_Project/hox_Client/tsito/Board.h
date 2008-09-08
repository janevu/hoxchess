#ifndef	__BOARD_H__
#define __BOARD_H__

/*
 * Board.h (c) Noah Roberts 2003-02-23
 * class interface, keeps track of the current state of the board.
 */

#include	<vector>
#include	<string>
#include	<iostream>


#include	"Move.h"

typedef enum { BLUE = 0, RED = 8, NOCOLOR = 16} color;
typedef enum { EMPTY, ZU, PAO, CHE, MA, XIANG, SHI, LAO } piece;
#define JIANG LAO


#define BOARD_AREA	90
#define BOARD_WIDTH	9
#define	BOARD_HEIGHT	10


#define COLOR_SWITCH_KEY 0x80000000

typedef unsigned long u_int32;

class Board;

class BoardObserver
{
 public:
  virtual void boardChanged(Board *board, int changeType) {}
};

enum { GAME_OVER, MOVE_MADE, MOVE_UNDONE, BOARD_ALTERED };

class Board
{
 private:
  unsigned char		board[90];
  int			_sideToMove;

  std::vector<BoardObserver*> observers;

  bool			_gameOver;
  std::vector<Move>	moveHistory;
  std::string		_startPos;

  int	kings[2];
  std::vector< std::vector<int> >	rpieces;
  std::vector< std::vector<int> >   	bpieces;

  // Note: since the random values are generated by the
  // constructor, these may not be the same across boards;
  // therefore the hash keys will not match if the keys
  // are generated by different instances.
  static u_int32	hashValues[2][90][15];
  static bool 		hashValuesFilled;
  u_int32		_primaryHash;
  u_int32 		_secondaryHash;
  
  void generateValues(); // Generates the hash values for zoberist keys
  // hash key management...
  void alterHashes(int loc)
    {
      _primaryHash   ^= hashValues[0][loc][board[loc]];
      _secondaryHash ^= hashValues[1][loc][board[loc]];
    }

  // Piece index management...
  void addPiece(int location);
  void removePiece(int location);
  void movePiece(int origin, int dest);
  
 public:
  Board();
  Board(std::string position);

  // Move management...
  void makeMove(Move &theMove);
  void unmakeMove(); // unmakes the top move in moveHistory.
  void makeNullMove() { makeMove(Move::nullMove()); }
  void unmakeNullMove() { unmakeMove(); }

  // Square based access operators
  piece pieceAt(int index) { return (piece)(board[index]&7); }
  color colorAt(int index) { return (color)(board[index]&8); }
  unsigned char operator[](int index) { return board[index]; }
  
  color sideToMove() { return (color)_sideToMove; }

  // Position managament...
  bool setPosition(std::string fen);
  std::string getPosition();
  void resetBoard();

  // Game move history access...
  std::vector<Move>& history() { return moveHistory; }
  // Game starting position...
  std::string& startingPosition() { return _startPos; }

  bool gameOver() { return _gameOver; }
  void gameOver(bool go) { _gameOver = go; notifyObservers(GAME_OVER);}

  // Piece index access...
  int king(color c) { return kings[c==RED?1:0]; }
  std::vector<int> pieces(color c, piece p);

  // Zoberist keys...
  u_int32 primaryHash() { return _primaryHash; }
  u_int32 secondaryHash() { return _secondaryHash; }

  // Display...
  friend std::ostream& operator<<(std::ostream& out, Board &theBoard);

  void addObserver(BoardObserver *observer) { observers.push_back(observer); }
  //void removeObserver(BoardObserver *observer);
  void notifyObservers(int message);
};

#endif /* __BOARD_H__ */
