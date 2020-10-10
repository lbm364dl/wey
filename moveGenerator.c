#include <stdio.h>
#include <stdlib.h>
#ifndef MOVESTACK
#include "moveStack.h"
#define MOVESTACK
#endif
#ifndef MOVEGENERATOR 
#include "moveGenerator.h"
#define MOVEGENERATOR 
#endif
#ifndef BOARDREPRESENTATION 
#include "boardRepresentation.h"
#define BOARDREPRESENTATION
#endif
#include <time.h>
#include <assert.h>
MoveStack hist;

char kingsLocation[] = {e1, e8};

const char *moveFlagsToAscii[] = {
	[1] = "enPassantWhite", 
	[2] = "enPassantBlack", 
	[4] = "wKingCastle",
	[8] = "wQueenCastle",
	[16] = "bKingCastle",
	[32] = "bQueenCastle",
	[64] = "qProm",
	[128] = "rProm",
	[256] = "bProm",
	[512] = "nProm"
};

char offsets[][9] = {
	[1] = {17, 15, -17, -15, 0}, 					//bishop
	[2] = {1, 16, -1, -16, 0}, 						//rook
	[4] = {1, 16, -1, -16, 17, 15, -17, -15, 0}, 	//queen
	[8] = {18, 14, 33, 31, -18, -14, -33, -31, 0},	//knight
	[16] = {1, 16, -1, -16, 17, 15, -17, -15, 0}	//king
};

int isSquareAttacked(int tSquare, int side){
	Piece **p = (side == WHITE) ? whitePieces : blackPieces;
	int isAttacked = 0;
	for( ; ((*p) -> pieceValue) != -2 && !isAttacked; p++){
		if(((*p) -> pieceValue) != -1){
			isAttacked = isSquareAttackedByPiece((*p) -> pieceLocation, tSquare, 
				pieceValuesToDeltaPieces[(*p) -> pieceValue]);
		}
	}
	return isAttacked;
}

int isSquareAttackedByPiece(int fSquare, int tSquare, int pieceDelta){
	int isAttacked = 1;
	//printf("%d ", isAttacked);
	int diffSquares = tSquare - fSquare + 119;
	int conceivablePieces = delta[diffSquares];
	//printf("fSquare: %d (%s), tSquare: %d (%s), tSquare - fSquare: %d \n", 
		//fSquare, squareToCoords[fSquare], tSquare, squareToCoords[tSquare], diffSquares);
	if(!(conceivablePieces & pieceDelta)){ //the piece can't reach that square in one move
		//printf("Piece can't attack that square\n");
		return 0;
	}
	else if(!(pieceDelta & (queen+rook+bishop))){
		//printf("Piece was knight, pawn or king, it can get there in one jump\n");
		return 1; 
	}
	else{
		int deltaToSquare = slidingDelta[diffSquares];
		int currentSquare;
		for(currentSquare = fSquare+deltaToSquare ; isAttacked && currentSquare != tSquare ; currentSquare += deltaToSquare){
			//printf("I'm at: %s\n", squareToCoords[currentSquare]);
			if(board[currentSquare] != NULL){
				isAttacked = 0;
				//printf("There's a piece blocking the way to that square.\n");
			}
		}
	}
	/*if(isAttacked)
		printf("Piece can slide to that square\n");*/
	return isAttacked;
} 

Move *generatePseudoLegalMoves(){
	Move *pseudoLegalMoves = calloc(325, sizeof(Move)); //321 upper bound for legal moves
	Move *startP = pseudoLegalMoves;
	Piece **p = (turn == WHITE) ? whitePieces : blackPieces;
	char pValue = (*p) -> pieceValue;
	char pLocation = (*p) -> pieceLocation;
	char pSide = (*p) -> pieceSide;
	for(; pValue != -2; p++){
		pValue = (*p) -> pieceValue;
		pLocation = (*p) -> pieceLocation;
		pSide = (*p) -> pieceSide;
		if(pValue != -1){
			assert(pValue != 0);
			//Piece is a white pawn
			if(pieceValuesToDeltaPieces[pValue] == whitePawn){
				if(board[pLocation + 16] == NULL){ //advance one square if no piece blocking
					pseudoLegalMoves = generatePawnPromotions(pLocation, pseudoLegalMoves, 16, WHITE, *p);
					if((pLocation >> 4) == 1 && board[pLocation + 32] == NULL){ //pawn in rank 2 can advance two squares
					*(pseudoLegalMoves++) = (Move){**p, pLocation + 32, quiet};
					}
				}
				Piece *targetLocationPiece = board[pLocation+17];
				if(!((pLocation+17)&0x88) && targetLocationPiece != NULL && targetLocationPiece->pieceSide == BLACK){ //Up-right eating piece
					pseudoLegalMoves = generatePawnPromotions(pLocation, pseudoLegalMoves, 17, WHITE, *p);
				}
				targetLocationPiece = board[pLocation+15];
				if(!((pLocation+15)&0x88) && targetLocationPiece != NULL && targetLocationPiece->pieceSide == BLACK){ //Up-left eating piece
					pseudoLegalMoves = generatePawnPromotions(pLocation, pseudoLegalMoves, 15, WHITE, *p);
				}
				if(pLocation+15 == enPassantSquare){
					*(pseudoLegalMoves++) = (Move){**p, pLocation + 15, enPassantWhite};  //en passant eating pawn
				}else if(pLocation+17 == enPassantSquare){
					*(pseudoLegalMoves++) = (Move){**p, pLocation + 17, enPassantWhite};
				}

			//Piece is a black pawn
			}else if(pieceValuesToDeltaPieces[pValue] == blackPawn){
				if(board[pLocation - 16] == NULL){ //advance one square if no piece blocking
					pseudoLegalMoves = generatePawnPromotions(pLocation, pseudoLegalMoves, -16, BLACK, *p);
					if((pLocation >> 4) == 6 && board[pLocation - 32] == NULL){ //pawn in rank 7 can advance two squares
					*(pseudoLegalMoves++) = (Move){**p, pLocation - 32, quiet};
					}
				}
				Piece *targetLocationPiece = board[pLocation-17];
				if(!((pLocation-17)&0x88) && targetLocationPiece != NULL && targetLocationPiece->pieceSide == WHITE){ //Down-left eating piece
					pseudoLegalMoves = generatePawnPromotions(pLocation, pseudoLegalMoves, -17, BLACK, *p);
				}
				targetLocationPiece = board[pLocation-15];
				if(!((pLocation-15)&0x88) && targetLocationPiece != NULL && targetLocationPiece->pieceSide == WHITE){ //Down-left eating piece
					pseudoLegalMoves = generatePawnPromotions(pLocation, pseudoLegalMoves, -15, BLACK, *p);
				}
				if(pLocation-15 == enPassantSquare){
					*(pseudoLegalMoves++) = (Move){**p, pLocation - 15, enPassantBlack};  //en passant eating pawn
				}else if(pLocation-17 == enPassantSquare){
					*(pseudoLegalMoves++) = (Move){**p, pLocation - 17, enPassantBlack};
				}
			}else if(pieceValuesToDeltaPieces[pValue] == king){
				char *kingOffsets = offsets[king];
				for( ; *kingOffsets ; kingOffsets++){ //check king natural moves
					char targetSquare = pLocation+*kingOffsets;
					Piece *targetPiece = board[targetSquare];
					//if square is on the board and target is empty or it's an enemy piece
					if(!(targetSquare & 0x88) && (targetPiece == NULL || pSide ^ (targetPiece -> pieceSide))){
						*(pseudoLegalMoves++) = (Move){**p, targetSquare, generateCaptureFlags(targetSquare)};
					}
				}
				if(pSide == WHITE && (castlingRights & (KC+QC)) && !isSquareAttacked(e1, BLACK)){ 	//if white can castle
					int canKingCastle = castlingRights & KC;
					int canQueenCastle = castlingRights & QC;
					if(canKingCastle && board[f1] == NULL && board[g1] == NULL && !isSquareAttacked(f1, BLACK)){
						*(pseudoLegalMoves++) = (Move){**p, g1, wKingCastle};
					}
					if(canQueenCastle && board[b1] == NULL && board[c1] == NULL && board[d1] == NULL 
						&& !isSquareAttacked(d1, BLACK)){
						*(pseudoLegalMoves++) = (Move){**p, c1, wQueenCastle};
					}
				}else if(pSide == BLACK && (castlingRights & (kc+qc)) && !isSquareAttacked(e8, WHITE)){												//if black can castle
					int canKingCastle = castlingRights & kc;
					int canQueenCastle = castlingRights & qc;
					if(canKingCastle && board[f8] == NULL && board[g8] == NULL && !isSquareAttacked(f8, WHITE)){
						*(pseudoLegalMoves++) = (Move){**p, g8, bKingCastle};
					}
					if(canQueenCastle && board[b8] == NULL && board[c8] == NULL && board[d8] == NULL
						&& !isSquareAttacked(d8, WHITE)){
						*(pseudoLegalMoves++) = (Move){**p, c8, bQueenCastle};
					}
				}
			}else if(pieceValuesToDeltaPieces[pValue] == knight){
				char *knightOffsets = offsets[knight];
				for( ; *knightOffsets ; knightOffsets++){ //check king natural moves
					char targetSquare = pLocation+*knightOffsets;
					Piece *targetPiece = board[targetSquare];
					//if square is on the board and target is empty or it's an enemy piece
					if(!(targetSquare & 0x88) && (targetPiece == NULL || pSide ^ (targetPiece -> pieceSide))){
						*(pseudoLegalMoves++) = (Move){**p, targetSquare, generateCaptureFlags(targetSquare)};
					}
				}
			}else{
				char *pieceOffset = offsets[pieceValuesToDeltaPieces[pValue]];
				for( ; *pieceOffset ; pieceOffset++){
					char targetSquare = pLocation + *pieceOffset;
					Piece *targetPiece = board[targetSquare];
					while(!(targetSquare & 0x88) && targetPiece == NULL){
						*(pseudoLegalMoves++) = (Move){**p, targetSquare, quiet};
						targetSquare+=*pieceOffset;
						targetPiece = board[targetSquare];
					}
					//printf("%d %d\n", targetSquare, targetPiece);
					assert((targetSquare & 0x88) || targetPiece != NULL);
					if(!(targetSquare & 0x88) && pSide ^ (targetPiece -> pieceSide)){
						*(pseudoLegalMoves++) = (Move){**p, targetSquare, generateCaptureFlags(targetSquare)};
					}
				}
			}
		}
	}
	return startP;
}

Move *generateLegalMoves(int *n_moves){
	Move *pseudoLegal = generatePseudoLegalMoves();
	Move *startPseudoLegal = pseudoLegal;
	Move *legalMovesList = calloc(325, sizeof(Move));
	Move *startLegalMovesList = legalMovesList;
	for( ; (pseudoLegal->piece).pieceValue ; pseudoLegal++){
		makeMove(pseudoLegal);
		if(!isSquareAttacked(kingsLocation[!turn], turn)){
			*legalMovesList = *pseudoLegal;
			legalMovesList++;
			(*n_moves)++;
		}
		undoMove();
	}
	free(startPseudoLegal);
	return startLegalMovesList;
}

int generateCaptureFlags(char targetSquare){
	int captFlags = quiet;
	if(board[targetSquare] != NULL){
		switch(board[targetSquare] -> pieceValue){
			case p:
			case P:
				captFlags = pawnCapture;
				break;
			case r:
			case R:
				captFlags = rookCapture;
				break;
			case n:
			case N:
				captFlags = knightCapture;
				break;
			case b:
			case B:
				captFlags = bishopCapture;
				break;
			case q:
			case Q:
				captFlags = queenCapture;
				break;
		}
	}
	return captFlags;
}

Move *generatePawnPromotions(char pLocation, Move *pseudoMoves, char deltaPawn, char side, Piece *pPiece){
	char sideProm = (side == WHITE) ? 6 : 1;
	int captFlags = generateCaptureFlags(pLocation + deltaPawn);
	if(pLocation >> 4 == sideProm){ //generating all possible promotions for white pawn
		*(pseudoMoves++) = (Move){*pPiece, pLocation + deltaPawn, qProm + captFlags};
		*(pseudoMoves++) = (Move){*pPiece, pLocation + deltaPawn, rProm + captFlags};
		*(pseudoMoves++) = (Move){*pPiece, pLocation + deltaPawn, nProm + captFlags};
		*(pseudoMoves++) = (Move){*pPiece, pLocation + deltaPawn, bProm + captFlags};
	}else{
		*(pseudoMoves++) = (Move){*pPiece, pLocation + deltaPawn, captFlags};
	}
	return pseudoMoves;
}

void printMoves(int legalOrPseudo){
	int mainkra = 0;
	Move *listMoves = (legalOrPseudo == LEGAL) ? generateLegalMoves(&mainkra) : generatePseudoLegalMoves();
	Move *startListMoves = listMoves;
	printf("Possible moves for %s:\n", sideToAscii[turn]);
	for( ; (listMoves->piece).pieceValue ; listMoves++){
		printf("Piece: %c, fSquare: %s, tSquare: %s, ", 
			ascii[(listMoves->piece).pieceValue], 
			squareToCoords[(listMoves->piece).pieceLocation], 
			squareToCoords[listMoves->tSquare]);
		printMoveInfo(listMoves->moveDetails);
		printf("\n");
	}
	printf("\n");
	free(startListMoves);
}

void printBoardMoves(int legalOrPseudo){
	int mainkra = 0;
	char *reachableSquaresMove = calloc(128, sizeof(int));
	Move *listMoves = (legalOrPseudo == LEGAL) ? generateLegalMoves(&mainkra) : generatePseudoLegalMoves();
	Move *startListMoves = listMoves;

	for( ; (listMoves->piece).pieceValue ; listMoves++){
		reachableSquaresMove[listMoves->tSquare] = 1;
	}
	free(startListMoves);
	int i, j;
	printf("\n\n");
	for(i = 0 ; i < ROWS ; i++){
		printf("%d  ",ROWS - i); // prints rank numbers
		for(j = 0 ; j < COLS ; j++){
			printf("%c ", reachableSquaresMove[16*(7-i) + j] ? 'x' : '.'); // print x if the square is reachable
		}
		printf("\n");
	}
	printf("\n   A B C D E F G H\n\n"); // prints file letters
	free(reachableSquaresMove);
}

void printMoveInfo(int flags){
	if(flags & enPassantWhite) printf("enPassantWhite ");
	if(flags & enPassantBlack) printf("enPassantBlack ");
	if(flags & wKingCastle) printf("wKingCastle ");
	if(flags & wQueenCastle) printf("wQueenCastle ");
	if(flags & bKingCastle) printf("bKingCastle ");
	if(flags & bQueenCastle) printf("bQueenCastle ");
	if(flags & qProm) printf("queenProm ");
	if(flags & rProm) printf("rookProm ");
	if(flags & bProm) printf("bishopProm ");
	if(flags & nProm) printf("knightProm ");
	if(flags & pawnCapture) printf("pawnCapt ");
	if(flags & rookCapture) printf("rookCapt ");
	if(flags & knightCapture) printf("knightCapt ");
	if(flags & bishopCapture) printf("bishopCapt ");
	if(flags & queenCapture) printf("queenCapt ");
}

void printAttackedSquares(int side){
	int i, j;
	for(i = 0 ; i < ROWS ; i++){
		printf("%d  ",ROWS - i); // prints rank numbers
		for(j = 0 ; j < COLS ; j++){
			int testAttack = isSquareAttacked(16*(7 - i) + j, side);
			printf("%c ", (testAttack) ? 'x' : '.'); // print the piece in the square converted to ascii char
		}
		printf("\n");
	}
	printf("\n   A B C D E F G H\n\n"); // prints file letters
}

unsigned long long perft(int depth, char from, char to){

	  int nMoves = 0, i;
	  Move *move_list = generateLegalMoves(&nMoves);
	  if (depth == 1) {
	  	//printf("prof 1 %s%s: %d\n", squareToCoords[from], squareToCoords[to], nMoves);
	  	free(move_list);
	    return (unsigned long long)nMoves;
	}
	  
	  Move *startMove_list = move_list;
	  unsigned long long nodes = 0;
	  
	  for (i = 0 ; i < nMoves ; i++) {
	    makeMove(move_list);
	    nodes += perft(depth - 1, (move_list->piece).pieceLocation, move_list->tSquare);
	    undoMove();
	    move_list++;
  	}
  if(depth == 4)
  	printf("prof 4 %s%s: %d\n", squareToCoords[from], squareToCoords[to], nodes);

  free(startMove_list);
  return nodes;
}

int main(void){
	printf("\n\nFEN: %s\n", fen);
	resetBoard();
	setBoardFromFEN(fen);
	printBoard();
	initializeMoveStack();
	//makeMove(&(Move){{K, g1, WHITE}, h1, 0});
	clock_t start = clock();
	//printMoves(LEGAL);
	unsigned long long nNodes = perft(5, 0, 0);
	printf("Number of nodes: %d\n", nNodes);
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time: %.10f\n", seconds);
	float nps = nNodes/seconds;
	printf("Nodes/sec: %.10f\n", nps);
	printf("memTest: %d\n", memTest);
	return 0;
}