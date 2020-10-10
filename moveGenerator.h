#ifndef BOARDREPRESENTATION 
#include "boardRepresentation.h"
#define BOARDREPRESENTATION
#endif

#define LEGAL 0
#define PSEUDOLEGAL 1

extern char kingsLocation[];

extern char offsets[][9];

enum moveFlags{
	quiet = 1, enPassantWhite = 2, enPassantBlack = 4, wKingCastle = 8, wQueenCastle = 16, bKingCastle = 32, bQueenCastle = 64,
	rProm = 128, nProm = 256, bProm = 512, qProm = 1024, pawnCapture = 2048, rookCapture = 4096, knightCapture = 8192,
	bishopCapture = 16384, queenCapture = 32768
};

extern const char *moveFlagsToAscii[];

typedef struct Move{
	Piece piece;		//piece with value and position (fSquare)
	char tSquare;	//destination (tSquares)
	int moveDetails;
} Move;

Move *generateLegalMoves(int *);
Move *generatePseudoLegalMoves();
Move *generatePawnPromotions(char, Move *, char, char, Piece *);
int generateCaptureFlags(char);
int isSquareAttacked(int, int);
int isSquareAttackedByPiece(int, int, int);
void printAttackedSquares(int);
void printMoves(int);
void printMoveInfo();
void printBoardMoves(int);
unsigned long long perft(int, char, char);