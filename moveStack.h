#ifndef MOVEGENERATOR 
#include "moveGenerator.h"
#define MOVEGENERATOR 
#endif
#ifndef BOARDREPRESENTATION 
#include "boardRepresentation.h"
#define BOARDREPRESENTATION
#endif

extern int memTest;
extern int pawnPromCast[];
extern int pieceCapturedCast[];

enum memLeak{
	newPieceL = 1, updatedRookL = 2, promotedL = 4, lastMovePieceL = 8, pieceWithCurrentLocationL = 16,
	undoNewPieceL = 32, pieceCapturedL = 64, undoUpdatedRookL = 128, 
};

typedef struct MoveStack{
	Move *moveList;
	Move *pNextMove;
	int *fiftyMoveCounterList;
	int *pFiftyCounter;
	int *castlingRightsList;
	int *pCastlingRights;
	int size;
	int capacity;
} MoveStack;

extern MoveStack hist;

void initializeMoveStack();
void makeMove(Move *);
void undoMove();
void printMovesList();
void updatePiece(Piece *, Piece *, char);