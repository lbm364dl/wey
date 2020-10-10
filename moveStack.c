#include <stdio.h>
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
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int memTest = 0;
MoveStack hist;
int pawnPromCast[] = {[1] = R, [2] = N, [4] = B, [8] = Q};
int pieceCapturedCast[] = {[1] = P, [2] = R, [4] = N, [8] = B, [16] = Q};

void initializeMoveStack(){
	hist.moveList = calloc(200, sizeof(Move));

	hist.fiftyMoveCounterList = calloc(201, sizeof(int));
	hist.pFiftyCounter = hist.fiftyMoveCounterList;
	*(hist.pFiftyCounter++) = fiftyMoveCounter;

	hist.castlingRightsList = calloc(201, sizeof(int));
	hist.pCastlingRights = hist.castlingRightsList;
	*(hist.pCastlingRights++) = castlingRights;

	hist.capacity = 200;
	hist.pNextMove = hist.moveList;
	hist.size = 0;
}

void makeMove(Move *m){
	//suppose valid movement (use from pseudo-move generator list)
	if((m->piece).pieceValue == K){
		kingsLocation[WHITE] = m->tSquare;
	}else if((m->piece).pieceValue == k){
		kingsLocation[BLACK] = m->tSquare;
	}



	Piece *newPiece;
	memTest |= newPieceL;

	int flags = m -> moveDetails;

	Piece *updatedRook;

	//update position info
	if((m->piece).pieceValue == P || (m->piece).pieceValue == p 
			|| flags & (pawnCapture+knightCapture+bishopCapture+queenCapture+rookCapture)){
		fiftyMoveCounter = 0;
	}else{
		fiftyMoveCounter = *(hist.pFiftyCounter-1) + 1;
	}
	

	if((m->piece).pieceValue == P && ((m->piece).pieceLocation >> 4) == 1 && (m->tSquare >> 4) == 3){
		enPassantSquare = m->tSquare - 16;
	}else if((m->piece).pieceValue == p && ((m->piece).pieceLocation >> 4) == 6 && (m->tSquare >> 4) == 4){
		enPassantSquare = m->tSquare + 16;
	}else{
		enPassantSquare = 10;
	}

	if(castlingRights != 0){
		switch((m->piece).pieceValue){
			case R:
				if((m->piece).pieceLocation == h1){
					castlingRights &= QC+kc+qc;
				}else if((m->piece).pieceLocation == a1){
					castlingRights &= KC+kc+qc;
				}
				break;
			case r:
				if((m->piece).pieceLocation == h8){
					castlingRights &= qc+KC+QC;
				}else if((m->piece).pieceLocation == a8){
					castlingRights &= kc+KC+QC;
				}
				break;
			case K:
				castlingRights &= kc+qc;
				break;
			case k:
				castlingRights &= KC+QC;
				break;
		}

		if(flags & rookCapture){
			switch(m->tSquare){
				case a8:
					castlingRights &= KC+QC+kc;
					break;
				case h8:
					castlingRights &= KC+QC+qc;
					break;
				case a1:
					castlingRights &= KC+kc+qc;
					break;
				case h1:
					castlingRights &= QC+kc+qc;
					break;
			}
		}
	}
	
	

	//update board and pieces
	
	if(!(flags & (rProm+bProm+nProm+qProm))){ //if move is not a pawn promotion
		newPiece = malloc(sizeof(Piece));
		*newPiece = (Piece){(m->piece).pieceValue, m->tSquare, (m->piece).pieceSide};
		if(flags & quiet){ //quiet move, no promotion, most common
			assert(board[(m->tSquare)] == NULL);
			board[m->tSquare] = newPiece;
			updatePiece(&(m->piece), newPiece, (m->piece).pieceSide);

		//common capture, not en passant
		}else if(flags & (pawnCapture+rookCapture+knightCapture+queenCapture+bishopCapture)){
			assert(board[(m->tSquare)] != NULL);
			updatePiece(board[m->tSquare], pNoPiece, board[m->tSquare] -> pieceSide);
			free(board[m->tSquare]);
			board[m->tSquare] = newPiece;
			updatePiece(&(m->piece), newPiece, (m->piece).pieceSide);

		}else if(flags & enPassantWhite){
			assert(board[(m->tSquare) - 16] != NULL);
			updatePiece(board[(m->tSquare) - 16], pNoPiece, BLACK);
			free(board[(m->tSquare) - 16]);
			board[(m->tSquare) - 16] = NULL;
			board[m->tSquare] = newPiece;
			updatePiece(&(m->piece), newPiece, WHITE);
		}else if(flags & enPassantBlack){
			assert(board[(m->tSquare) + 16] != NULL);
			updatePiece(board[(m->tSquare) + 16], pNoPiece, WHITE);
			free(board[(m->tSquare) + 16]);
			board[(m->tSquare) + 16] = NULL;
			board[m->tSquare] = newPiece;
			updatePiece(&(m->piece), newPiece, BLACK);
		}else{ //castling
			switch(flags){
				case wKingCastle:
					memTest |= updatedRookL;
					updatedRook = malloc(sizeof(Piece));
					*updatedRook = (Piece){R, f1, WHITE};
					assert(board[h1] != NULL);
					updatePiece(board[h1], updatedRook, WHITE);
					free(board[h1]);
					board[h1] = NULL;
					assert(board[f1] == NULL);
					board[f1] = updatedRook;
					board[m->tSquare] = newPiece;
					updatePiece(&(m->piece), newPiece, WHITE);
					break;
				case wQueenCastle:
					memTest |= updatedRookL;
					updatedRook = malloc(sizeof(Piece));
					*updatedRook = (Piece){R, d1, WHITE};
					assert(board[a1] != NULL);
					updatePiece(board[a1], updatedRook, WHITE);
					free(board[a1]);
					board[a1] = NULL;
					assert(board[d1] == NULL);
					board[d1] = updatedRook;
					board[m->tSquare] = newPiece;
					updatePiece(&(m->piece), newPiece, WHITE);
					break;
				case bKingCastle:
					memTest |= updatedRookL;
					updatedRook = malloc(sizeof(Piece));
					*updatedRook = (Piece){r, f8, BLACK};
					assert(board[h8] != NULL);
					updatePiece(board[h8], updatedRook, BLACK);
					free(board[h8]);
					board[h8] = NULL;
					assert(board[f8] == NULL);
					board[f8] = updatedRook;
					board[m->tSquare] = newPiece;
					updatePiece(&(m->piece), newPiece, BLACK);
					break;
				case bQueenCastle:
					memTest |= updatedRookL;
					updatedRook = malloc(sizeof(Piece));
					*updatedRook = (Piece){r, d8, BLACK};
					assert(board[a8] != NULL);
					updatePiece(board[a8], updatedRook, BLACK);
					free(board[a8]);
					board[a8] = NULL;
					assert(board[d8] == NULL);
					board[d8] = updatedRook;
					board[m->tSquare] = newPiece;
					updatePiece(&(m->piece), newPiece, BLACK);
					break;
			}
		}
	}else{ //pawn promotion
		int piecePromotion = (pawnPromCast[flags >> 7 & 0xF]) + 6 * (m->piece).pieceSide;
		Piece *promoted = malloc(sizeof(Piece));
		memTest |= promotedL;
		*promoted = (Piece){(char)piecePromotion, m->tSquare, (m->piece).pieceSide};
		if(!(flags&quiet)){
			updatePiece(board[m->tSquare], pNoPiece, board[m->tSquare] -> pieceSide);
			free(board[m->tSquare]);
		}
		board[m->tSquare] = promoted;
		updatePiece(&(m->piece), promoted, (m->piece).pieceSide);
	}

	free(board[(m->piece).pieceLocation]);
	board[(m->piece).pieceLocation] = NULL;

	if(hist.size + 1 == hist.capacity){
		Move *moves = hist.moveList;
		int *fiftyMoves = hist.fiftyMoveCounterList;
		int *castling = hist.castlingRightsList;

		hist.capacity += 100;
		hist.moveList = calloc(hist.capacity, sizeof(Move));
		memcpy(hist.moveList, moves, hist.size*sizeof(Move));
		free(moves);
		hist.pNextMove = hist.moveList + hist.size;

		hist.fiftyMoveCounterList = calloc(hist.capacity+1, sizeof(int));
		memcpy(hist.fiftyMoveCounterList, fiftyMoves, (hist.size+1)*sizeof(int));
		free(fiftyMoves);
		hist.pFiftyCounter = hist.fiftyMoveCounterList + hist.size + 1;

		hist.castlingRightsList = calloc(hist.capacity+1, sizeof(int));
		memcpy(hist.castlingRightsList, castling, (hist.size+1)*sizeof(int));
		free(castling);
		hist.pCastlingRights = hist.castlingRightsList + hist.size + 1;
	}

	turn = !turn;
	*(hist.pNextMove++) = *m;
	*(hist.pCastlingRights++) = castlingRights;
	*(hist.pFiftyCounter++) = fiftyMoveCounter;
	hist.size++;
}

void undoMove(){
	hist.pNextMove--;

	Piece *lastMovePiece = malloc(sizeof(Piece));
	memTest |= lastMovePieceL;
	*lastMovePiece = hist.pNextMove -> piece;

	Piece *pieceWithCurrentLocation = malloc(sizeof(Piece));
	memTest |= pieceWithCurrentLocationL;

	char lastMoveTSquare = hist.pNextMove -> tSquare;
	int flags = hist.pNextMove -> moveDetails;

	Piece *newPiece = malloc(sizeof(Piece));
	memTest |= undoNewPieceL;
	Piece *pieceCaptured;
	Piece *updatedRook;

	
	
	if(lastMovePiece->pieceValue == K){
		kingsLocation[WHITE] = lastMovePiece->pieceLocation;
	}else if(lastMovePiece->pieceValue == k){
		kingsLocation[BLACK] = lastMovePiece->pieceLocation;
	}

	//update position info
	hist.pFiftyCounter--;
	fiftyMoveCounter = *(hist.pFiftyCounter-1);

	if(flags & (enPassantWhite+enPassantBlack)){
		enPassantSquare = lastMoveTSquare;
	}else{
		enPassantSquare = 10;
	}

	hist.pCastlingRights--;
	castlingRights = *(hist.pCastlingRights-1);

	//update board and pieces
	if(!(flags & (rProm+bProm+nProm+qProm))){ //if move is not a pawn promotion
		*newPiece =  (Piece){lastMovePiece->pieceValue, lastMovePiece->pieceLocation, lastMovePiece->pieceSide};
		board[lastMovePiece->pieceLocation] = newPiece;
		*pieceWithCurrentLocation = (Piece){lastMovePiece->pieceValue, lastMoveTSquare, lastMovePiece->pieceSide};

		updatePiece(pieceWithCurrentLocation, newPiece, lastMovePiece -> pieceSide);

		free(board[lastMoveTSquare]);
		board[lastMoveTSquare] = NULL;
		//common capture, not en passant
		if(flags & (pawnCapture+rookCapture+knightCapture+queenCapture+bishopCapture)){
			pieceCaptured = malloc(sizeof(Piece));
			memTest |= pieceCapturedL;
			int pieceCapturedValue = pieceCapturedCast[flags >> 11] + 6 * !lastMovePiece->pieceSide;
			*pieceCaptured = (Piece){(char)pieceCapturedValue, lastMoveTSquare, !lastMovePiece->pieceSide};
			updatePiece(pNoPiece, pieceCaptured, !(lastMovePiece -> pieceSide));
			board[lastMoveTSquare] = pieceCaptured;
		}else if(flags & enPassantWhite){
			pieceCaptured = malloc(sizeof(Piece));
			memTest |= pieceCapturedL;
			*pieceCaptured = (Piece){p, lastMoveTSquare - 16, BLACK};
			updatePiece(pNoPiece, pieceCaptured, BLACK);
			board[lastMoveTSquare - 16] = pieceCaptured;
		}else if(flags & enPassantBlack){
			pieceCaptured = malloc(sizeof(Piece));
			memTest |= pieceCapturedL;
			*pieceCaptured = (Piece){P, lastMoveTSquare + 16, WHITE};
			updatePiece(pNoPiece, pieceCaptured, WHITE);
			board[lastMoveTSquare + 16] = pieceCaptured;
		}else{ //castling
			switch(flags){
				case wKingCastle:
					updatedRook = malloc(sizeof(Piece));
					memTest |= undoUpdatedRookL;
					*updatedRook = (Piece){R, h1, WHITE};
					assert(board[f1] != NULL);
					updatePiece(board[f1], updatedRook, WHITE);
					free(board[f1]);
					board[f1] = NULL;
					assert(board[h1] == NULL);
					board[h1] = updatedRook;
					break;
				case wQueenCastle:
					updatedRook = malloc(sizeof(Piece));
					memTest |= undoUpdatedRookL;
					*updatedRook = (Piece){R, a1, WHITE};
					assert(board[d1] != NULL);
					updatePiece(board[d1], updatedRook, WHITE);
					free(board[d1]);
					board[d1] = NULL;
					assert(board[a1] == NULL);
					board[a1] = updatedRook;
					break;
				case bKingCastle:
					updatedRook = malloc(sizeof(Piece));
					memTest |= undoUpdatedRookL;
					*updatedRook = (Piece){r, h8, BLACK};
					assert(board[f8] != NULL);
					updatePiece(board[f8], updatedRook, BLACK);
					free(board[f8]);
					board[f8] = NULL;
					assert(board[h8] == NULL);
					board[h8] = updatedRook;
					break;
				case bQueenCastle:
					updatedRook = malloc(sizeof(Piece));
					memTest |= undoUpdatedRookL;
					*updatedRook = (Piece){r, a8, BLACK};
					assert(board[d8] != NULL);
					updatePiece(board[d8], updatedRook, BLACK);
					free(board[d8]);
					board[d8] = NULL;
					assert(board[a8] == NULL);
					board[a8] = updatedRook;
					break;
			}
		}
	}else{ //pawn promotion
		*newPiece = (Piece){P + 6*lastMovePiece->pieceSide, lastMovePiece->pieceLocation, lastMovePiece->pieceSide};
		assert((newPiece->pieceValue == P && newPiece->pieceSide == WHITE) ||
			newPiece->pieceValue == p && newPiece->pieceSide == BLACK);
		int piecePromotion = pawnPromCast[flags >> 7 && 0xF] + 6 * lastMovePiece->pieceSide;
		assert((piecePromotion >= 1 && piecePromotion <= 6 && lastMovePiece->pieceSide == WHITE) ||
			(piecePromotion >= 7 && piecePromotion <= 12 && lastMovePiece->pieceSide == BLACK));
		*pieceWithCurrentLocation = (Piece){(char)piecePromotion, lastMoveTSquare, lastMovePiece->pieceSide};
		updatePiece(pieceWithCurrentLocation, newPiece, lastMovePiece -> pieceSide);
		free(board[lastMoveTSquare]);
		board[lastMoveTSquare] = NULL;
		board[lastMovePiece->pieceLocation] = newPiece;
		if(!(flags&quiet)){
			pieceCaptured = malloc(sizeof(Piece));
			memTest |= pieceCapturedL;
			int pieceCapturedValue = pieceCapturedCast[flags >> 11] + 6 * !lastMovePiece->pieceSide;
			*pieceCaptured = (Piece){(char)pieceCapturedValue, lastMoveTSquare, !lastMovePiece->pieceSide};
			updatePiece(pNoPiece, pieceCaptured, pieceCaptured -> pieceSide);
			board[lastMoveTSquare] = pieceCaptured;
		}
	}

	free(lastMovePiece);
	free(pieceWithCurrentLocation);
	turn = !turn;
	hist.size--;
}

void updatePiece(Piece *lastP, Piece *newP, char side){

	Piece **pPieces = (side == WHITE) ? whitePieces : blackPieces;
	while(((*pPieces) -> pieceLocation) != (lastP -> pieceLocation)){
		assert((*pPieces) != NULL);
		pPieces++;
	}
	*pPieces = newP;
	assert((*pPieces)->pieceValue != 0);
}

void printMovesList(){
	Move *pMoves = hist.moveList;
	int *pCastling = hist.castlingRightsList + 1;
	int *pFiftyMoves = hist.fiftyMoveCounterList + 1;
	int i;
	for( i = 0; i < hist.size ; i++){
		printf("%c %s %s %d %d\n",ascii[(pMoves -> piece).pieceValue], 
			squareToCoords[(pMoves -> piece).pieceLocation], squareToCoords[pMoves -> tSquare],
			*pCastling, *pFiftyMoves);
		pMoves++;
	}
	printf("\n");
	printf("Capacity: %d, size: %d\n", hist.capacity, hist.size);
}